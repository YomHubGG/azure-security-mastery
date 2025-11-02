@description('Secure deployment of minishell container on Azure Container Instances')

param location string = resourceGroup().location
param containerRegistryName string = 'devacr4uybw3c2lbkwm'
param minishellVersion string = 'v1.0'

// Reference to existing Container Registry
resource containerRegistry 'Microsoft.ContainerRegistry/registries@2023-01-01-preview' existing = {
  name: containerRegistryName
}

// Virtual Network for container (security best practice)
resource vnet 'Microsoft.Network/virtualNetworks@2023-04-01' = {
  name: 'vnet-minishell'
  location: location
  properties: {
    addressSpace: {
      addressPrefixes: ['10.1.0.0/16']
    }
    subnets: [
      {
        name: 'container-subnet'
        properties: {
          addressPrefix: '10.1.1.0/24'
          delegations: [
            {
              name: 'Microsoft.ContainerInstance.containerGroups'
              properties: {
                serviceName: 'Microsoft.ContainerInstance/containerGroups'
              }
            }
          ]
        }
      }
    ]
  }
}

// Container Group with Managed Identity and Security
resource containerGroup 'Microsoft.ContainerInstance/containerGroups@2023-05-01' = {
  name: 'cg-minishell-secure'
  location: location
  identity: {
    type: 'SystemAssigned'
  }
  properties: {
    containers: [
      {
        name: 'minishell'
        properties: {
          image: '${containerRegistry.properties.loginServer}/minishell:${minishellVersion}'
          ports: [
            {
              port: 80
              protocol: 'TCP'
            }
          ]
          resources: {
            requests: {
              cpu: 1
              memoryInGB: 2
            }
            limits: {
              cpu: 2
              memoryInGB: 4
            }
          }
          environmentVariables: [
            {
              name: 'ENVIRONMENT'
              value: 'production'
            }
            {
              name: 'AZURE_TENANT_ID'
              value: tenant().tenantId
            }
          ]
          // ✅ Day 47: Security context (CKV_AZURE_133)
          securityContext: {
            privileged: false  // Don't run as root
          }
        }
      }
    ]
    osType: 'Linux'
    restartPolicy: 'OnFailure'
    ipAddress: {
      type: 'Public'
      ports: [
        {
          port: 80
          protocol: 'TCP'
        }
      ]
      dnsNameLabel: 'minishell-${uniqueString(resourceGroup().id)}'
    }
    subnetIds: [
      {
        id: vnet.properties.subnets[0].id
      }
    ]
    imageRegistryCredentials: [
      {
        server: containerRegistry.properties.loginServer
        username: containerRegistry.name
        // Will use managed identity - credentials handled separately
      }
    ]
  }
}

output containerGroupName string = containerGroup.name
output fqdn string = containerGroup.properties.ipAddress.fqdn
output managedIdentityPrincipalId string = containerGroup.identity.principalId
output publicIP string = containerGroup.properties.ipAddress.ip
