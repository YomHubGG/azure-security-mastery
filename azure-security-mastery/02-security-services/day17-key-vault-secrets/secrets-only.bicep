@description('Sets up Key Vault secrets for enterprise management demo')

param keyVaultName string

// 📅 Day 47: Secret expiration variables (Checkov CKV_AZURE_41 fix)
// Secrets expire after 90 days to enforce credential rotation
var secretExpiration = dateTimeToEpoch(dateTimeAdd(utcNow(), 'P90D'))  // 90 days from now
var secretNotBefore = dateTimeToEpoch(utcNow())                        // Valid from now

// Existing Key Vault
resource keyVault 'Microsoft.KeyVault/vaults@2023-02-01' existing = {
  name: keyVaultName
}

// Enterprise-grade secrets with proper naming and attributes
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'prod-database-connection'
  parent: keyVault
  properties: {
    value: 'Server=myserver.${environment().suffixes.sqlServerHostname};Database=proddb;User Id=produser;Password=SecurePass123!;'
    attributes: {
      enabled: true
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
    }
    contentType: 'database connection string'
  }
}

resource apiKeySecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'external-service-api-key'
  parent: keyVault
  properties: {
    value: 'sk-prod-1234567890abcdef'
    attributes: {
      enabled: true
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
    }
    contentType: 'API key'
  }
}

resource jwtSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'jwt-signing-secret'
  parent: keyVault
  properties: {
    value: base64('MyJWTSigningKeyForProductionEnvironment2025')
    attributes: {
      enabled: true
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
    }
    contentType: 'JWT signing key'
  }
}

resource storageSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  name: 'storage-account-key'
  parent: keyVault
  properties: {
    // SECURITY-EXCEPTION: Educational template with placeholder connection string
    // In production: Use Key Vault references or managed identity instead
    value: 'DefaultEndpointsProtocol=https;AccountName=PLACEHOLDER;AccountKey=EDUCATIONAL-EXAMPLE;EndpointSuffix=${environment().suffixes.storage}'
    attributes: {
      enabled: true
      exp: secretExpiration  // ✅ Expires in 90 days
      nbf: secretNotBefore   // ✅ Valid from deployment time
    }
    contentType: 'Storage account connection string - EDUCATIONAL TEMPLATE'
  }
}

output keyVaultUri string = keyVault.properties.vaultUri
output secretNames array = [
  databaseSecret.name
  apiKeySecret.name
  jwtSecret.name
  storageSecret.name
]
