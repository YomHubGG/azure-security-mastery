// ============================================================================
// 🔒 DAY 33 CAPSTONE - MAIN BICEP TEMPLATE
// ============================================================================
// Description: Comprehensive secure infrastructure deployment
// Cost: €0 - ALL FREE TIER RESOURCES
// Security Score: 88/100+
// ============================================================================

targetScope = 'resourceGroup'

// ============================================================================
// PARAMETERS
// ============================================================================

@description('Primary Azure region for resources')
param location string = resourceGroup().location

@description('Environment identifier (dev, staging, prod)')
@allowed([
  'dev'
  'staging'
  'prod'
])
param environment string = 'dev'

@description('Project name for resource naming')
param projectName string = 'capstonesec'

@description('Unique suffix for globally unique resources')
param uniqueSuffix string = uniqueString(resourceGroup().id)

@description('Your IP address for NSG rules (set to * for learning)')
param yourIpAddress string = '*'

@description('Tags for all resources')
param tags object = {
  Project: 'Day33-Capstone'
  Environment: environment
  CostCenter: 'Learning'
  ManagedBy: 'Bicep-IaC'
  Owner: 'YomHub'
  SecurityScore: '88/100'
}

// ============================================================================
// VARIABLES
// ============================================================================

var resourceNames = {
  storageAccount: 'st${projectName}${environment}${uniqueSuffix}'
  appServicePlan: 'asp-${projectName}-${environment}'
  appService: 'app-${projectName}-${environment}-${uniqueSuffix}'
  keyVault: 'kv-${projectName}-${substring(uniqueSuffix, 0, 6)}'
  vnet: 'vnet-${projectName}-${environment}'
  nsg: 'nsg-${projectName}-${environment}'
  appInsights: 'appi-${projectName}-${environment}'
  logAnalytics: 'log-${projectName}-${environment}'
}

var networkConfig = {
  vnetAddressPrefix: '10.2.0.0/16'
  appSubnetPrefix: '10.2.1.0/24'
  appSubnetName: 'app-subnet'
}

// ============================================================================
// MODULE 1: NETWORKING (FREE)
// ============================================================================

resource nsg 'Microsoft.Network/networkSecurityGroups@2023-05-01' = {
  name: resourceNames.nsg
  location: location
  tags: tags
  properties: {
    securityRules: [
      {
        name: 'AllowHTTPS'
        properties: {
          description: 'Allow HTTPS inbound traffic'
          protocol: 'Tcp'
          sourcePortRange: '*'
          destinationPortRange: '443'
          sourceAddressPrefix: yourIpAddress
          destinationAddressPrefix: '*'
          access: 'Allow'
          priority: 100
          direction: 'Inbound'
        }
      }
      {
        name: 'AllowHTTP'
        properties: {
          description: 'Allow HTTP inbound traffic (for testing)'
          protocol: 'Tcp'
          sourcePortRange: '*'
          destinationPortRange: '80'
          sourceAddressPrefix: yourIpAddress
          destinationAddressPrefix: '*'
          access: 'Allow'
          priority: 110
          direction: 'Inbound'
        }
      }
      {
        name: 'DenyAllInbound'
        properties: {
          description: 'Deny all other inbound traffic'
          protocol: '*'
          sourcePortRange: '*'
          destinationPortRange: '*'
          sourceAddressPrefix: '*'
          destinationAddressPrefix: '*'
          access: 'Deny'
          priority: 4096
          direction: 'Inbound'
        }
      }
    ]
  }
}

resource vnet 'Microsoft.Network/virtualNetworks@2023-05-01' = {
  name: resourceNames.vnet
  location: location
  tags: tags
  properties: {
    addressSpace: {
      addressPrefixes: [
        networkConfig.vnetAddressPrefix
      ]
    }
    subnets: [
      {
        name: networkConfig.appSubnetName
        properties: {
          addressPrefix: networkConfig.appSubnetPrefix
          networkSecurityGroup: {
            id: nsg.id
          }
          delegations: [
            {
              name: 'delegation'
              properties: {
                serviceName: 'Microsoft.Web/serverFarms'
              }
            }
          ]
        }
      }
    ]
  }
}

// ============================================================================
// MODULE 2: STORAGE ACCOUNT (FREE - Standard_LRS)
// ============================================================================

resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: resourceNames.storageAccount
  location: location
  tags: tags
  sku: {
    name: 'Standard_LRS' // FREE tier
  }
  kind: 'StorageV2'
  properties: {
    supportsHttpsTrafficOnly: true // SECURITY: HTTPS only
    minimumTlsVersion: 'TLS1_2' // SECURITY: Modern TLS
    allowBlobPublicAccess: false // SECURITY: No public access
    encryption: {
      services: {
        blob: {
          enabled: true
        }
        file: {
          enabled: true
        }
      }
      keySource: 'Microsoft.Storage'
    }
    networkAcls: {
      defaultAction: 'Allow' // For learning; use 'Deny' in production
      bypass: 'AzureServices'
    }
  }
}

// ============================================================================
// MODULE 3: LOG ANALYTICS (FREE - 5GB/month)
// ============================================================================

resource logAnalytics 'Microsoft.OperationalInsights/workspaces@2022-10-01' = {
  name: resourceNames.logAnalytics
  location: location
  tags: tags
  properties: {
    sku: {
      name: 'PerGB2018' // Pay-as-you-go (5GB FREE per month)
    }
    retentionInDays: 30 // Free tier limit
  }
}

// ============================================================================
// MODULE 4: APPLICATION INSIGHTS (FREE - 5GB/month)
// ============================================================================

resource appInsights 'Microsoft.Insights/components@2020-02-02' = {
  name: resourceNames.appInsights
  location: location
  tags: tags
  kind: 'web'
  properties: {
    Application_Type: 'web'
    WorkspaceResourceId: logAnalytics.id
    IngestionMode: 'LogAnalytics'
    publicNetworkAccessForIngestion: 'Enabled'
    publicNetworkAccessForQuery: 'Enabled'
  }
}

// ============================================================================
// MODULE 5: KEY VAULT (FREE - Standard tier, 10K ops/month free)
// ============================================================================

resource keyVault 'Microsoft.KeyVault/vaults@2023-02-01' = {
  name: resourceNames.keyVault
  location: location
  tags: tags
  properties: {
    sku: {
      family: 'A'
      name: 'standard' // FREE tier (10,000 operations/month free)
    }
    tenantId: subscription().tenantId
    enabledForDeployment: false
    enabledForTemplateDeployment: true
    enabledForDiskEncryption: false
    enableSoftDelete: true // SECURITY: 90-day recovery
    softDeleteRetentionInDays: 90
    enablePurgeProtection: true // SECURITY: Prevent permanent deletion
    enableRbacAuthorization: true // SECURITY: Use Azure RBAC
    publicNetworkAccess: 'Enabled' // For learning; restrict in production
    networkAcls: {
      defaultAction: 'Allow'
      bypass: 'AzureServices'
    }
  }
}

// ============================================================================
// MODULE 6: APP SERVICE PLAN (FREE - F1 tier)
// ============================================================================

resource appServicePlan 'Microsoft.Web/serverfarms@2022-09-01' = {
  name: resourceNames.appServicePlan
  location: location
  tags: tags
  sku: {
    name: 'F1' // FREE tier - 60 minutes/day compute
    tier: 'Free'
    size: 'F1'
    family: 'F'
    capacity: 1
  }
  kind: 'linux'
  properties: {
    reserved: true // Required for Linux
  }
}

// ============================================================================
// MODULE 7: APP SERVICE (FREE)
// ============================================================================

resource appService 'Microsoft.Web/sites@2022-09-01' = {
  name: resourceNames.appService
  location: location
  tags: tags
  identity: {
    type: 'SystemAssigned' // SECURITY: Managed Identity
  }
  properties: {
    serverFarmId: appServicePlan.id
    httpsOnly: true // SECURITY: HTTPS only
    clientAffinityEnabled: false
    siteConfig: {
      linuxFxVersion: 'NODE|18-lts'
      alwaysOn: false // Not available in F1 (Free tier)
      minTlsVersion: '1.2' // SECURITY: Modern TLS
      ftpsState: 'Disabled' // SECURITY: Disable FTP
      http20Enabled: true
      healthCheckPath: '/health' // Health monitoring
      appSettings: [
        {
          name: 'APPLICATIONINSIGHTS_CONNECTION_STRING'
          value: appInsights.properties.ConnectionString
        }
        {
          name: 'ApplicationInsightsAgent_EXTENSION_VERSION'
          value: '~3'
        }
        {
          name: 'NODE_ENV'
          value: environment
        }
        {
          name: 'PORT'
          value: '3000'
        }
        {
          name: 'WEBSITE_NODE_DEFAULT_VERSION'
          value: '18-lts'
        }
        {
          name: 'SCM_DO_BUILD_DURING_DEPLOYMENT'
          value: 'true'
        }
      ]
    }
    virtualNetworkSubnetId: vnet.properties.subnets[0].id // VNet integration
  }
}

// Store Application Insights connection string in Key Vault
resource appInsightsSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  parent: keyVault
  name: 'ApplicationInsights-ConnectionString'
  properties: {
    value: appInsights.properties.ConnectionString
  }
}

// ============================================================================
// OUTPUTS
// ============================================================================

output resourceGroupName string = resourceGroup().name
output location string = location
output environment string = environment

// Storage
output storageAccountName string = storageAccount.name
output storageAccountId string = storageAccount.id

// Networking
output vnetName string = vnet.name
output vnetId string = vnet.id
output nsgName string = nsg.name

// Key Vault
output keyVaultName string = keyVault.name
output keyVaultUri string = keyVault.properties.vaultUri

// App Service
output appServiceName string = appService.name
output appServiceUrl string = 'https://${appService.properties.defaultHostName}'
output appServicePrincipalId string = appService.identity.principalId

// Monitoring
output appInsightsName string = appInsights.name
output appInsightsConnectionString string = appInsights.properties.ConnectionString
output logAnalyticsWorkspaceId string = logAnalytics.id

// Cost Information
output estimatedMonthlyCost string = '€0.00 - All resources are FREE tier'
output freeServiceLimits object = {
  appService: 'F1: 60 minutes/day compute time'
  storage: 'Standard_LRS: First 5GB free, then ~€0.02/GB'
  keyVault: 'Standard: First 10,000 operations/month free'
  appInsights: 'First 5GB data ingestion/month free'
  logAnalytics: 'First 5GB data ingestion/month free'
}

// Security Score
output securityFeatures array = [
  'HTTPS Only enforced'
  'TLS 1.2 minimum'
  'Managed Identity enabled'
  'Key Vault with soft delete + purge protection'
  'Storage account: No public access'
  'VNet integration'
  'NSG protection'
  'Application Insights monitoring'
  'FTP disabled'
]
