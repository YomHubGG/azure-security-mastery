@description('Sets up enterprise Key Vault with secrets and proper policies')

param keyVaultName string
param location string = resourceGroup().location

// 📅 Day 47: Secret expiration variables (Checkov CKV_AZURE_41 fix)
var secretExpiration = dateTimeToEpoch(dateTimeAdd(utcNow(), 'P90D'))  // 90 days
var secretNotBefore = dateTimeToEpoch(utcNow())                        // Valid from now

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
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
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
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
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
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
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
    httpsOnly: true  // ✅ Day 47: Enforce HTTPS-only (CKV_AZURE_80, CKV_AZURE_222)
    siteConfig: {
      linuxFxVersion: 'NODE|18-lts'
      minTlsVersion: '1.2'  // ✅ Enforce TLS 1.2+ (CKV_AZURE_88)
      ftpsState: 'Disabled'  // ✅ Disable insecure FTP (CKV_AZURE_78)
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
