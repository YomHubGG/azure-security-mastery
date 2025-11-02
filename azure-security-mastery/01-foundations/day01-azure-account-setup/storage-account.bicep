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
    name: 'Standard_LRS'  // Locally Redundant Storage (cheapest for learning)
    // 📝 SECURITY NOTE: In production, use Standard_GRS for geo-replication
    // Trade-off: Cost (LRS €0.02/GB vs GRS €0.04/GB) vs Resilience
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
    
    // 🛡️ NETWORK SECURITY (Day 47: Checkov CKV_AZURE_35 fix)
    // Default: DENY all network access, explicitly allow trusted sources
    networkAcls: {
      defaultAction: 'Deny'           // Block all by default
      bypass: 'AzureServices'         // Allow trusted Azure services
      ipRules: []                     // Add specific IPs if needed
      virtualNetworkRules: []         // Add VNet integration if needed
    }
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

// 📊 Day 47 Security Improvements (Checkov scan):
// ✅ FIXED: CKV_AZURE_35 - Added networkAcls (deny by default)
// ⚠️ ACCEPTED: CKV_AZURE_206 - Using LRS for learning (cost optimization)
//    Production recommendation: Standard_GRS for geo-redundancy
// ✅ ALREADY COMPLIANT: CKV_AZURE_43 - Naming convention with 'st' prefix
