// 🔐 Secure Container Instance with Managed Identity & Key Vault
// This demonstrates passwordless authentication to Azure services!

@description('Name of the container instance')
param containerName string = 'secure-keyvault-app'

@description('Location for the container')
param location string = resourceGroup().location

@description('Container image from ACR')
param containerImage string = 'devacr4uybw3c2lbkwm.azurecr.io/secure-keyvault-app:v1'

@description('Key Vault URL for the application')
param keyVaultUrl string = 'https://kvlearning4uybw3c2lbkwm.vault.azure.net/'

@description('ACR login server')
param acrLoginServer string = 'devacr4uybw3c2lbkwm.azurecr.io'

@description('Environment for the deployment')
param environment string = 'dev'

// 🆔 User Assigned Managed Identity for passwordless authentication
resource managedIdentity 'Microsoft.ManagedIdentity/userAssignedIdentities@2023-01-31' = {
  name: '${containerName}-identity'
  location: location
  tags: {
    Environment: environment
    Purpose: 'Key Vault Access'
    Project: 'Secure Application'
  }
}

// 🏗️ Container Instance with Managed Identity
resource containerInstance 'Microsoft.ContainerInstance/containerGroups@2023-05-01' = {
  name: containerName
  location: location
  tags: {
    Environment: environment
    Purpose: 'Secure Key Vault Demo'
    Project: 'Azure Security Mastery'
    ManagedIdentity: 'Enabled'
  }
  
  // 🔑 MAGIC: Assign Managed Identity to container
  identity: {
    type: 'UserAssigned'
    userAssignedIdentities: {
      '${managedIdentity.id}': {}
    }
  }
  
  properties: {
    // ACR authentication using admin credentials
    imageRegistryCredentials: [
      {
        server: acrLoginServer
        username: acrLoginServer
        password: listCredentials(resourceId('Microsoft.ContainerRegistry/registries', split(acrLoginServer, '.')[0]), '2023-07-01').passwords[0].value
      }
    ]
    
    containers: [
      {
        name: 'secure-app'
        properties: {
          image: containerImage
          
          // 🌍 Environment variables (no secrets!)
          environmentVariables: [
            {
              name: 'KEYVAULT_URL'
              value: keyVaultUrl
            }
            {
              name: 'NODE_ENV'
              value: environment
            }
            {
              name: 'PORT'
              value: '3000'
            }
            // 🔑 MAGIC: Managed Identity client ID for authentication
            {
              name: 'AZURE_CLIENT_ID'
              value: managedIdentity.properties.clientId
            }
          ]
          
          // Resource allocation
          resources: {
            requests: {
              cpu: 1
              memoryInGB: 1
            }
          }
          
          // Port configuration
          ports: [
            {
              port: 3000
              protocol: 'TCP'
            }
          ]
        }
      }
    ]
    
    // Network configuration
    ipAddress: {
      type: 'Public'
      ports: [
        {
          port: 3000
          protocol: 'TCP'
        }
      ]
    }
    
    // Operating system
    osType: 'Linux'
    
    // Restart policy
    restartPolicy: 'OnFailure'  // Better than 'Always' for cost control
  }
}

// 🔐 RBAC: Grant Managed Identity access to Key Vault secrets
resource keyVaultSecretsUserRole 'Microsoft.Authorization/roleAssignments@2022-04-01' = {
  name: guid(containerInstance.id, managedIdentity.id, 'Key Vault Secrets User')
  scope: resourceGroup()
  properties: {
    roleDefinitionId: subscriptionResourceId('Microsoft.Authorization/roleDefinitions', '4633458b-17de-408a-b874-0445c86b69e6') // Key Vault Secrets User
    principalId: managedIdentity.properties.principalId
    principalType: 'ServicePrincipal'
  }
}

// 📊 Outputs
output containerName string = containerInstance.name
output containerFqdn string = containerInstance.properties.ipAddress.fqdn
output containerIp string = containerInstance.properties.ipAddress.ip
output managedIdentityId string = managedIdentity.id
output managedIdentityClientId string = managedIdentity.properties.clientId

// 🎯 Application URLs
output healthCheckUrl string = 'http://${containerInstance.properties.ipAddress.ip}:3000/health'
output configUrl string = 'http://${containerInstance.properties.ipAddress.ip}:3000/config'
output databaseTestUrl string = 'http://${containerInstance.properties.ipAddress.ip}:3000/database-test'

// 🎓 LEARNING NOTES:
// 1. Managed Identity = passwordless authentication
// 2. AZURE_CLIENT_ID tells Azure SDK which identity to use
// 3. RBAC assignment grants Key Vault access
// 4. No secrets in environment variables!
// 5. Container pulls image using ACR admin credentials
// 6. OnFailure restart policy prevents cost surprises
