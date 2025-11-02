// Test storage account only
param location string = resourceGroup().location

resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: 'devsectest${uniqueString(resourceGroup().id)}'
  location: location
  sku: {
    name: 'Standard_LRS'  // ✅ Day 47: Accepted for learning (cost optimization)
  }
  kind: 'StorageV2'
  properties: {
    accessTier: 'Hot'
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    allowBlobPublicAccess: false
    // ✅ Day 47: Network restrictions (CKV_AZURE_35)
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
      ipRules: []
      virtualNetworkRules: []
    }
  }
}
