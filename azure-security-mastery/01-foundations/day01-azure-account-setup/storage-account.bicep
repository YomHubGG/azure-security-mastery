// 🏗️ My First Bicep Template - Secure Storage Account
// Date: August 29, 2025
// Purpose: Learning Infrastructure as Code with security best practices

// 📋 PARAMETERS
// These are inputs to our template - makes it reusable
param storageAccountName string = 'stlearning${uniqueString(resourceGroup().id)}'
param location string = resourceGroup().location

// 💾 STORAGE ACCOUNT RESOURCE
// This defines what we want to create in Azure
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  
  // 💰 SKU = Pricing/Performance tier
  sku: {
    name: 'Standard_LRS'  // Locally Redundant Storage (cheapest, good for learning)
  }
  
  // 📦 Kind = Type of storage account
  kind: 'StorageV2'  // General purpose v2 (recommended, most features)
  
  // 🔒 SECURITY PROPERTIES
  properties: {
    // 🔐 Force HTTPS - no unencrypted connections allowed
    supportsHttpsTrafficOnly: true
    
    // 🔒 Minimum TLS 1.2 - reject older, insecure protocols
    minimumTlsVersion: 'TLS1_2'
    
    // 🚫 Disable public blob access - no anonymous downloads
    allowBlobPublicAccess: false
    
    // 🌡️ Access tier - how often data is accessed
    accessTier: 'Hot'  // Frequently accessed (vs Cool/Archive)
  }
}

// 📤 OUTPUTS
// These return information after deployment
output storageAccountName string = storageAccount.name
output storageAccountId string = storageAccount.id

// 🎓 LEARNING NOTES:
// - uniqueString() ensures globally unique names
// - resourceGroup() references our container
// - @ syntax specifies API version (important for consistency)
// - Security is configured from the start, not added later!
