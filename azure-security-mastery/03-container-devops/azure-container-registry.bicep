// Azure Container Registry with secure configuration
param location string = resourceGroup().location
param environmentPrefix string = 'dev'
param acrName string = '${environmentPrefix}acr${uniqueString(resourceGroup().id)}'

// Container Registry with security features
resource containerRegistry 'Microsoft.ContainerRegistry/registries@2023-01-01-preview' = {
  name: acrName
  location: location
  sku: {
    name: 'Basic'  // Start with Basic, can upgrade to Premium for more features
  }
  properties: {
    adminUserEnabled: false  // Use Azure AD authentication instead
    publicNetworkAccess: 'Enabled'  // For learning, will restrict later
    networkRuleBypassOptions: 'AzureServices'
  }
}

// Output for connecting to the registry
output registryLoginServer string = containerRegistry.properties.loginServer
output registryName string = containerRegistry.name

// Resource lock to prevent accidental deletion (NOT CLICKING!)
resource registryLock 'Microsoft.Authorization/locks@2020-05-01' = {
  name: '${acrName}-lock'
  scope: containerRegistry
  properties: {
    level: 'CanNotDelete'
    notes: 'Prevent accidental deletion of container registry'
  }
}
