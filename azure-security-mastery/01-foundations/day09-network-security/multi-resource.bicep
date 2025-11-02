// Multi-resource Bicep template for network security learning
// This template deploys: VNet + Subnet + NSG + Storage Account
// Demonstrates security-first design and resource dependencies

@description('Environment prefix (e.g., dev, prod)')
param environmentPrefix string = 'dev'

@description('Location for all resources')
param location string = resourceGroup().location

@description('Admin username for potential VM access')
param adminUsername string = 'azureuser'

// Variables for naming consistency
var vnetName = '${environmentPrefix}-vnet-security'
var subnetName = '${environmentPrefix}-subnet-secure'
var nsgName = '${environmentPrefix}-nsg-security'
var storageAccountName = '${environmentPrefix}sec${uniqueString(resourceGroup().id)}'

// Network Security Group with secure defaults
resource networkSecurityGroup 'Microsoft.Network/networkSecurityGroups@2023-04-01' = {
  name: nsgName
  location: location
  properties: {
    securityRules: [
      {
        name: 'AllowHTTPS'
        properties: {
          priority: 1000
          access: 'Allow'
          direction: 'Inbound'
          destinationPortRange: '443'
          protocol: 'Tcp'
          sourcePortRange: '*'
          sourceAddressPrefix: '*'
          destinationAddressPrefix: '*'
        }
      }
      {
        name: 'AllowSSH'
        properties: {
          priority: 1100
          access: 'Allow'
          direction: 'Inbound'
          destinationPortRange: '22'
          protocol: 'Tcp'
          sourcePortRange: '*'
          // ⚠️ Day 47: CRITICAL SECURITY FIX (CKV_AZURE_10)
          // Changed from '*' (entire internet) to specific IP range
          // TODO: Replace with your organization's IP range before production
          sourceAddressPrefix: '10.0.0.0/8'  // ✅ Restrict to private network only
          destinationAddressPrefix: '*'
        }
      }
      {
        name: 'DenyAllInbound'
        properties: {
          priority: 4096
          access: 'Deny'
          direction: 'Inbound'
          destinationPortRange: '*'
          protocol: '*'
          sourcePortRange: '*'
          sourceAddressPrefix: '*'
          destinationAddressPrefix: '*'
        }
      }
    ]
  }
}

// Virtual Network with secure subnet
resource virtualNetwork 'Microsoft.Network/virtualNetworks@2023-04-01' = {
  name: vnetName
  location: location
  properties: {
    addressSpace: {
      addressPrefixes: [
        '10.0.0.0/16'
      ]
    }
    subnets: [
      {
        name: subnetName
        properties: {
          addressPrefix: '10.0.1.0/24'
          networkSecurityGroup: {
            id: networkSecurityGroup.id  // NSG association
          }
        }
      }
    ]
  }
}

// Secure storage account with network restrictions
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  sku: {
    name: 'Standard_LRS'  // ✅ Day 47: Accepted for learning (cost optimization)
  }
  kind: 'StorageV2'
  properties: {
    accessTier: 'Hot'
    supportsHttpsTrafficOnly: true  // HTTPS only
    minimumTlsVersion: 'TLS1_2'     // Modern TLS
    allowBlobPublicAccess: false    // No public access
    // ✅ Day 47: Network ACLs (CKV_AZURE_35)
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
      ipRules: []
      virtualNetworkRules: []
    }
  }
}

// Outputs for reference and chaining
output vnetId string = virtualNetwork.id
output subnetId string = virtualNetwork.properties.subnets[0].id
output nsgId string = networkSecurityGroup.id
output storageAccountName string = storageAccount.name
output storageAccountEndpoint string = storageAccount.properties.primaryEndpoints.blob
