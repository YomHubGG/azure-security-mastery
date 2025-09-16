// 🕐 Container Scheduler - Start/Stop containers on schedule
// This creates a Logic App that can start containers at specific times

@description('Name for the Logic App scheduler')
param logicAppName string = 'container-scheduler'

@description('Location for resources')
param location string = resourceGroup().location

@description('Resource group name where containers will be created')
param containerResourceGroup string = resourceGroup().name

@description('Container image to deploy')
param containerImage string = 'devacr4uybw3c2lbkwm.azurecr.io/minishell-web:v8'

@description('ACR login server')
param acrLoginServer string = 'devacr4uybw3c2lbkwm.azurecr.io'

@description('Schedule expression (e.g., daily at 9 AM UTC)')
param scheduleExpression string = '0 0 9 * * *'  // 9 AM UTC daily

// 🔄 Logic App for container scheduling
resource containerScheduler 'Microsoft.Logic/workflows@2019-05-01' = {
  name: logicAppName
  location: location
  tags: {
    Purpose: 'Container Cost Management'
    Environment: 'Learning'
  }
  
  properties: {
    definition: {
      '$schema': 'https://schema.management.azure.com/providers/Microsoft.Logic/schemas/2016-06-01/workflowdefinition.json#'
      contentVersion: '1.0.0.0'
      
      triggers: {
        recurrence: {
          type: 'Recurrence'
          recurrence: {
            frequency: 'Day'
            interval: 1
            schedule: {
              hours: [9]  // 9 AM UTC
              minutes: [0]
            }
          }
        }
      }
      
      actions: {
        'Create-Container': {
          type: 'Http'
          inputs: {
            method: 'PUT'
            uri: 'https://management.azure.com/subscriptions/@{workflow().subscription.id}/resourceGroups/${containerResourceGroup}/providers/Microsoft.ContainerInstance/containerGroups/scheduled-minishell?api-version=2023-05-01'
            headers: {
              'Content-Type': 'application/json'
              'Authorization': 'Bearer @{workflow().token}'
            }
            body: {
              location: location
              properties: {
                containers: [
                  {
                    name: 'minishell-scheduled'
                    properties: {
                      image: containerImage
                      resources: {
                        requests: {
                          cpu: 0.5
                          memoryInGB: 1
                        }
                      }
                      ports: [
                        {
                          port: 3000
                          protocol: 'TCP'
                        }
                      ]
                      environmentVariables: [
                        {
                          name: 'NODE_ENV'
                          value: 'production'
                        }
                        {
                          name: 'PORT'
                          value: '3000'
                        }
                      ]
                    }
                  }
                ]
                osType: 'Linux'
                restartPolicy: 'OnFailure'
                ipAddress: {
                  type: 'Public'
                  ports: [
                    {
                      port: 3000
                      protocol: 'TCP'
                    }
                  ]
                  dnsNameLabel: 'scheduled-minishell-@{formatDateTime(utcnow(), 'MMddHHmm')}'
                }
                imageRegistryCredentials: [
                  {
                    server: acrLoginServer
                    username: acrLoginServer
                    // Note: In production, use Key Vault for password
                  }
                ]
              }
            }
          }
        }
        
        'Schedule-Cleanup': {
          type: 'Wait'
          inputs: {
            until: {
              timestamp: '@{addHours(utcnow(), 8)}'  // Auto-cleanup after 8 hours
            }
          }
          runAfter: {
            'Create-Container': ['Succeeded']
          }
        }
        
        'Delete-Container': {
          type: 'Http'
          inputs: {
            method: 'DELETE'
            uri: 'https://management.azure.com/subscriptions/@{workflow().subscription.id}/resourceGroups/${containerResourceGroup}/providers/Microsoft.ContainerInstance/containerGroups/scheduled-minishell?api-version=2023-05-01'
            headers: {
              'Authorization': 'Bearer @{workflow().token}'
            }
          }
          runAfter: {
            'Schedule-Cleanup': ['Succeeded']
          }
        }
      }
    }
    
    // Managed Identity for Azure API access
    identity: {
      type: 'SystemAssigned'
    }
  }
}

// 🔑 RBAC: Grant Logic App permission to manage containers
resource containerContributorRole 'Microsoft.Authorization/roleAssignments@2022-04-01' = {
  name: guid(containerScheduler.id, 'Container Contributor')
  scope: resourceGroup()
  
  properties: {
    roleDefinitionId: subscriptionResourceId('Microsoft.Authorization/roleDefinitions', 'b24988ac-6180-42a0-ab88-20f7382dd24c') // Contributor
    principalId: containerScheduler.identity.principalId
    principalType: 'ServicePrincipal'
  }
}

// 📊 Outputs
output logicAppName string = containerScheduler.name
output logicAppId string = containerScheduler.id
output scheduleInfo string = 'Container starts daily at 9 AM UTC and stops after 8 hours'
output manualTriggerUrl string = 'Check Azure Portal > Logic Apps > ${logicAppName} > Overview for trigger URL'

// 🎓 USAGE NOTES:
// 1. Deploy this template to create scheduled container management
// 2. Logic App will start container daily at 9 AM UTC
// 3. Container automatically stops after 8 hours to save costs
// 4. Modify schedule in Logic App designer as needed
// 5. Manual triggers available in Azure Portal
