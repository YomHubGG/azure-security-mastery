// 🔐 Azure Key Vault - Secure Secrets Management
// Date: September 9, 2025
// Purpose: Central secrets management for application security

// 📝 PARAMETERS
@description('Name of the Key Vault (must be globally unique)')
param keyVaultName string = 'kv-appsec-${uniqueString(resourceGroup().id)}'

@description('Location for the Key Vault')
param location string = resourceGroup().location

@description('Your Azure AD Object ID (for initial access)')
param adminObjectId string

// 🏗️ KEY VAULT RESOURCE
resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' = {
  name: keyVaultName
  location: location
  
  properties: {
    // 🏢 Tenant configuration
    tenantId: subscription().tenantId
    
    // 💰 Pricing tier
    sku: {
      family: 'A'
      name: 'standard'  // Standard tier (not premium - cost conscious!)
    }
    
    // 🔒 SECURITY CONFIGURATION
    enabledForDeployment: false          // Don't allow VM deployments to access
    enabledForDiskEncryption: false      // Don't allow disk encryption access  
    enabledForTemplateDeployment: true   // Allow ARM/Bicep template access
    enableSoftDelete: true               // Soft delete protection (90 days)
    softDeleteRetentionInDays: 90        // Retention period for deleted secrets
    enablePurgeProtection: true          // Prevent permanent deletion
    
    // 🌐 Network access controls
    publicNetworkAccess: 'Enabled'       // Allow public access (we'll secure via policies)
    networkAcls: {
      defaultAction: 'Allow'             // Default allow (we'll use RBAC instead)
      bypass: 'AzureServices'            // Allow trusted Azure services
    }
    
    // 👤 ACCESS POLICIES
    accessPolicies: [
      {
        // Admin access (you)
        tenantId: subscription().tenantId
        objectId: adminObjectId
        permissions: {
          secrets: [
            'get'
            'list'
            'set'
            'delete'
            'backup'
            'restore'
            'recover'
            'purge'
          ]
          keys: [
            'get'
            'list'
            'create'
            'delete'
            'backup'
            'restore'
            'recover'
            'purge'
          ]
          certificates: [
            'get'
            'list'
            'create'
            'delete'
            'backup'
            'restore'
            'recover'
            'purge'
          ]
        }
      }
    ]
  }
  
  // 🏷️ Tags for organization
  tags: {
    Environment: 'Learning'
    Project: 'AppSecurity'
    CostCenter: 'Training'
  }
}

// 📤 OUTPUTS
@description('Name of the created Key Vault')
output keyVaultName string = keyVault.name

@description('Resource ID of the Key Vault')
output keyVaultId string = keyVault.id

@description('URI of the Key Vault')
output keyVaultUri string = keyVault.properties.vaultUri

// 🎓 LEARNING NOTES:
// - uniqueString() ensures globally unique Key Vault name
// - Soft delete + purge protection = enterprise security
// - Access policies control who can access secrets
// - Network ACLs provide additional security layers
// - Tags help with cost management and organization
