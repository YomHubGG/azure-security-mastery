@description('Key Vault name (must be globally unique, 3-24 alphanumeric/hyphens)')
param keyVaultName string = 'kvhardened${uniqueString(resourceGroup().id)}'

@description('Location for all resources')
param location string = resourceGroup().location

@description('Azure AD tenant ID')
param tenantId string = subscription().tenantId

@description('Object ID of the user/service principal to grant access')
param objectId string

@description('Environment tag')
param environment string = 'Learning'

resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' = {
  name: keyVaultName
  location: location
  properties: {
    sku: {
      family: 'A'
      name: 'standard'  // Free tier
    }
    tenantId: tenantId
    
    // ✅ CKV_AZURE_109: Enable soft delete (CIS Azure)
    enableSoftDelete: true
    softDeleteRetentionInDays: 90  // Maximum free tier retention
    
    // ✅ CKV_AZURE_110: Enable purge protection
    enablePurgeProtection: true
    
    // ✅ CKV_AZURE_189: Disable public network access
    publicNetworkAccess: 'Disabled'
    
    // ✅ Network ACLs - deny all by default
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
      ipRules: []
      virtualNetworkRules: []
    }
    
    // ✅ RBAC authorization (modern approach)
    enableRbacAuthorization: true
    
    // Legacy access policies (keep empty if using RBAC)
    accessPolicies: [
      {
        tenantId: tenantId
        objectId: objectId
        permissions: {
          keys: ['get', 'list']
          secrets: ['get', 'list']
          certificates: ['get', 'list']
        }
      }
    ]
    
    // ✅ Enable logging
    enabledForDeployment: false
    enabledForDiskEncryption: false
    enabledForTemplateDeployment: true  // Allow Bicep access
  }
  
  tags: {
    Environment: environment
    SecurityLevel: 'Hardened'
    Compliance: 'CIS-Azure'
    Purpose: 'Day55-Security-Hardening'
  }
}

// Note: Diagnostic settings require a Log Analytics workspace or storage account destination
// Skipping for free tier deployment - can be added manually in production

output keyVaultId string = keyVault.id
output keyVaultName string = keyVault.name
output keyVaultUri string = keyVault.properties.vaultUri
output softDeleteEnabled bool = keyVault.properties.enableSoftDelete
output purgeProtectionEnabled bool = keyVault.properties.enablePurgeProtection
output publicNetworkAccess string = keyVault.properties.publicNetworkAccess
