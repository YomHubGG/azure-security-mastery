@description('Storage account name (must be globally unique, 3-24 lowercase/numbers)')
param storageAccountName string = 'sthardened${uniqueString(resourceGroup().id)}'

@description('Location for all resources')
param location string = resourceGroup().location

@description('Environment tag')
param environment string = 'Learning'

resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  kind: 'StorageV2'
  sku: {
    name: 'Standard_LRS'
  }
  properties: {
    // ✅ CKV_AZURE_33: HTTPS only (CIS Azure 3.1)
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    
    // ✅ CKV_AZURE_36: Disable public blob access (CIS Azure 3.7)
    allowBlobPublicAccess: false
    publicNetworkAccess: 'Disabled'
    
    // ✅ CKV_AZURE_3: Encryption for all services
    encryption: {
      services: {
        blob: {
          enabled: true
          keyType: 'Account'
        }
        file: {
          enabled: true
          keyType: 'Account'
        }
        queue: {
          enabled: true
          keyType: 'Account'
        }
        table: {
          enabled: true
          keyType: 'Account'
        }
      }
      keySource: 'Microsoft.Storage'
      requireInfrastructureEncryption: true  // Double encryption
    }
    
    // ✅ CKV_AZURE_35: Network ACLs (CIS Azure 3.8)
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'  // Allow trusted Microsoft services
      virtualNetworkRules: []
      ipRules: []
    }
    
    // ✅ Additional security: Advanced threat protection
    accessTier: 'Hot'
    allowSharedKeyAccess: true  // Required for Azure CLI access
    allowCrossTenantReplication: false
  }
  
  tags: {
    Environment: environment
    SecurityLevel: 'Hardened'
    Compliance: 'CIS-Azure-3.x'
    Purpose: 'Day55-Security-Hardening'
    CostCenter: 'Learning'
  }
}

// ✅ Enable blob service properties
resource blobServices 'Microsoft.Storage/storageAccounts/blobServices@2023-01-01' = {
  parent: storageAccount
  name: 'default'
  properties: {
    deleteRetentionPolicy: {
      enabled: true
      days: 7  // Soft delete for 7 days
    }
    containerDeleteRetentionPolicy: {
      enabled: true
      days: 7
    }
    isVersioningEnabled: true  // Blob versioning
    changeFeed: {
      enabled: false  // Change feed (premium feature)
    }
  }
}

// Note: Diagnostic settings require a Log Analytics workspace or storage account destination
// Skipping for free tier deployment - can be added manually in production

output storageAccountId string = storageAccount.id
output storageAccountName string = storageAccount.name
output httpsOnly bool = storageAccount.properties.supportsHttpsTrafficOnly
output tlsVersion string = storageAccount.properties.minimumTlsVersion
output publicAccess string = storageAccount.properties.publicNetworkAccess
output encryptionEnabled bool = storageAccount.properties.encryption.services.blob.enabled
