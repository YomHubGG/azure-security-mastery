// 🏗️ Advanced Application Security Stack
// Leveraging C++ developer's understanding of complex systems
// Date: September 9, 2025

// 📝 PARAMETERS (like function parameters in C++)
@description('Environment name (dev/test/prod)')
@allowed(['dev', 'test', 'prod'])
param environment string = 'dev'

@description('Application name prefix')
param appName string = 'secureapp'

@description('Location for all resources')
param location string = resourceGroup().location

// 🧮 VARIABLES (computed values, like const expressions)
// Fixed: Shorter names to comply with Azure naming limits
var keyVaultName = 'kv-${substring(uniqueString(resourceGroup().id), 0, 8)}' // Max 24 chars
var appServicePlanName = 'asp-${appName}-${environment}'
var appServiceName = 'app-${appName}-${environment}-${substring(uniqueString(resourceGroup().id), 0, 6)}'
var storageAccountName = 'st${appName}${environment}${substring(uniqueString(resourceGroup().id), 0, 6)}' // Max 24 chars

// 🏷️ COMMON TAGS (like #define constants)
var commonTags = {
  Environment: environment
  Application: appName
  ManagedBy: 'Bicep-IaC'
  CostCenter: 'Learning'
  Owner: 'YomHub'
}

// 🔐 KEY VAULT (secure secrets management)
resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' = {
  name: keyVaultName
  location: location
  tags: commonTags
  
  properties: {
    tenantId: subscription().tenantId
    sku: {
      family: 'A'
      name: 'standard'
    }
    
    // Security configuration (enterprise-grade)
    enableSoftDelete: true
    softDeleteRetentionInDays: 7  // Shorter for learning (saves costs)
    // enablePurgeProtection: removed - Azure tenant constraint
    enableRbacAuthorization: true // Modern RBAC instead of access policies
    
    // Network security
    publicNetworkAccess: 'Enabled'
    networkAcls: {
      defaultAction: 'Allow'
      bypass: 'AzureServices'
    }
  }
}

// 💾 STORAGE ACCOUNT (for app data)
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  tags: commonTags
  
  sku: {
    name: 'Standard_LRS'
  }
  kind: 'StorageV2'
  
  properties: {
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    allowBlobPublicAccess: false
    defaultToOAuthAuthentication: true
  }
}

// 🌐 APP SERVICE PLAN (compute resources)
resource appServicePlan 'Microsoft.Web/serverfarms@2022-09-01' = {
  name: appServicePlanName
  location: location
  tags: commonTags
  
  sku: {
    name: 'F1'  // Free tier for learning
    capacity: 1
  }
  
  properties: {
    reserved: true  // Linux hosting
  }
}

// 🚀 APP SERVICE (web application)
resource appService 'Microsoft.Web/sites@2022-09-01' = {
  name: appServiceName
  location: location
  tags: commonTags
  
  properties: {
    serverFarmId: appServicePlan.id
    
    siteConfig: {
      linuxFxVersion: 'NODE|18-lts'  // Node.js runtime
      alwaysOn: false  // Disabled for free tier
      ftpsState: 'Disabled'  // Security: no FTP
      
      // Security headers (like security flags in C++)
      httpLoggingEnabled: true
      detailedErrorLoggingEnabled: true
      
      appSettings: [
        {
          name: 'WEBSITES_ENABLE_APP_SERVICE_STORAGE'
          value: 'false'
        }
        {
          name: 'KEY_VAULT_URL'
          value: keyVault.properties.vaultUri
        }
        {
          name: 'STORAGE_ACCOUNT_NAME'
          value: storageAccount.name
        }
      ]
    }
    
    httpsOnly: true  // Force HTTPS (security best practice)
  }
  
  // Managed Identity (passwordless authentication) - separate from properties
  identity: {
    type: 'SystemAssigned'
  }
}

// 🔑 RBAC ASSIGNMENT (grant app access to Key Vault)
resource keyVaultAccessPolicy 'Microsoft.Authorization/roleAssignments@2022-04-01' = {
  name: guid(keyVault.id, appService.id, 'Key Vault Secrets User')
  scope: keyVault
  
  properties: {
    roleDefinitionId: subscriptionResourceId('Microsoft.Authorization/roleDefinitions', '4633458b-17de-408a-b874-0445c86b69e6') // Key Vault Secrets User
    principalId: appService.identity.principalId
    principalType: 'ServicePrincipal'
  }
}

// 📤 OUTPUTS (return values, like function returns)
@description('Key Vault URL')
output keyVaultUrl string = keyVault.properties.vaultUri

@description('App Service URL')  
output appServiceUrl string = 'https://${appService.properties.defaultHostName}'

@description('App Service Managed Identity')
output managedIdentityPrincipalId string = appService.identity.principalId

@description('Storage Account Name (connection details stored in Key Vault)')
output storageAccountName string = storageAccount.name

// 🎓 ADVANCED PATTERNS FOR C++ DEVELOPERS:
// 1. Resource dependencies = like object construction order
// 2. RBAC assignments = like friend class permissions  
// 3. Managed Identity = like automatic credential management
// 4. Output values = like function return values
// 5. Variables and parameters = like const and function args
// 6. Security: Never output secrets (like avoiding plaintext passwords)
