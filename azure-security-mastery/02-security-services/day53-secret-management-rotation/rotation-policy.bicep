// 🔐 Key Vault Secret with Rotation Policy
// Automatically sets expiration date based on rotation interval

@description('Name of the existing Key Vault')
param keyVaultName string

@description('Name of the secret to create/update')
param secretName string

@description('Secret value (use secure parameter in production)')
@secure()
param secretValue string

@description('Number of days before secret should be rotated')
@minValue(30)
@maxValue(730)
param rotationIntervalDays int = 90

@description('Environment tag')
@allowed([
  'production'
  'staging'
  'development'
  'learning'
])
param environment string = 'learning'

@description('Current time (auto-generated)')
param currentTime string = utcNow()

// Calculate expiration date
var expirationDate = dateTimeAdd(currentTime, 'P${rotationIntervalDays}D')
var expirationEpoch = dateTimeToEpoch(expirationDate)

// Reference existing Key Vault
resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' existing = {
  name: keyVaultName
}

// Create secret with expiration
resource secret 'Microsoft.KeyVault/vaults/secrets@2023-07-01' = {
  parent: keyVault
  name: secretName
  properties: {
    value: secretValue
    attributes: {
      enabled: true
      exp: expirationEpoch
    }
    contentType: 'text/plain'
  }
  tags: {
    RotationPolicy: '${rotationIntervalDays}days'
    Environment: environment
    ManagedBy: 'Bicep'
    CreatedDate: currentTime
    ExpiresDate: expirationDate
  }
}

// Outputs
output secretUri string = secret.properties.secretUri
output secretId string = secret.id
output expiresOn string = expirationDate
output expiresOnEpoch int = expirationEpoch
output rotationIntervalDays int = rotationIntervalDays
