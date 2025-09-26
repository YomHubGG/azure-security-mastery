@description('Sets up enterprise Key Vault with secrets and proper policies')

param keyVaultName string
param location string = resourceGroup().location

// Existing Key Vault
resource keyVault 'Microsoft.KeyVault/vaults@2023-02-01' existing = {
  name: keyVaultName
}

// SECURITY-EXCEPTION: Educational demo secrets - NOT for production use
// These are placeholder values demonstrating Key Vault secret management patterns
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'database-connection-string'
  parent: keyVault
  properties: {
    value: 'PLACEHOLDER-DB-CONNECTION-FOR-LEARNING-ONLY'
    attributes: {
      enabled: true
    }
  }
}

resource apiKeySecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'external-api-key'
  parent: keyVault
  properties: {
    value: 'EXAMPLE-API-KEY-FOR-EDUCATIONAL-PURPOSE'
    attributes: {
      enabled: true
    }
  }
}

resource jwtSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'jwt-signing-key'
  parent: keyVault
  properties: {
    value: base64('DEMO-JWT-KEY-LEARNING-TEMPLATE-ONLY')
    attributes: {
      enabled: true
    }
  }
}

// App Service Plan for our demo app
resource appServicePlan 'Microsoft.Web/serverfarms@2022-09-01' = {
  name: 'asp-keyvault-demo'
  location: location
  sku: {
    name: 'F1'
    tier: 'Free'
  }
  properties: {
    reserved: true // Linux
  }
}

// App Service with Managed Identity
resource appService 'Microsoft.Web/sites@2022-09-01' = {
  name: 'app-keyvault-demo-${uniqueString(resourceGroup().id)}'
  location: location
  identity: {
    type: 'SystemAssigned'
  }
  properties: {
    serverFarmId: appServicePlan.id
    siteConfig: {
      linuxFxVersion: 'NODE|18-lts'
      appSettings: [
        {
          name: 'AZURE_KEY_VAULT_URL'
          value: keyVault.properties.vaultUri
        }
        {
          name: 'SCM_DO_BUILD_DURING_DEPLOYMENT'
          value: 'true'
        }
      ]
    }
  }
}

// Grant the App Service access to Key Vault
resource keyVaultAccessPolicy 'Microsoft.KeyVault/vaults/accessPolicies@2023-02-01' = {
  name: 'add'
  parent: keyVault
  properties: {
    accessPolicies: [
      {
        tenantId: tenant().tenantId
        objectId: appService.identity.principalId
        permissions: {
          secrets: ['get', 'list']
        }
      }
    ]
  }
}

output appServiceName string = appService.name
output appServiceURL string = appService.properties.defaultHostName
output managedIdentityPrincipalId string = appService.identity.principalId
output keyVaultUri string = keyVault.properties.vaultUri
