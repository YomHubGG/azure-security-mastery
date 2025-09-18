@description('Custom Azure Policy for enforcing security and governance standards')

// Policy: Require HTTPS for Storage Accounts
resource httpsStoragePolicy 'Microsoft.Authorization/policyDefinitions@2021-06-01' = {
  name: 'custom-require-https-storage'
  properties: {
    displayName: 'Require HTTPS for Storage Accounts'
    description: 'Ensures all storage accounts only accept HTTPS traffic'
    policyType: 'Custom'
    mode: 'All'
    parameters: {}
    policyRule: {
      if: {
        allOf: [
          {
            field: 'type'
            equals: 'Microsoft.Storage/storageAccounts'
          }
          {
            field: 'Microsoft.Storage/storageAccounts/supportsHttpsTrafficOnly'
            notEquals: true
          }
        ]
      }
      then: {
        effect: 'deny'
      }
    }
  }
}

// Policy: Require Resource Tags
resource requiredTagsPolicy 'Microsoft.Authorization/policyDefinitions@2021-06-01' = {
  name: 'custom-require-tags'
  properties: {
    displayName: 'Require Specific Tags on Resources'
    description: 'Ensures resources have required tags for governance'
    policyType: 'Custom'
    mode: 'All'
    parameters: {
      requiredTags: {
        type: 'Object'
        defaultValue: {
          Environment: ''
          Project: ''
          CostCenter: ''
          Owner: ''
        }
      }
    }
    policyRule: {
      if: {
        anyOf: [
          {
            field: 'tags[Environment]'
            exists: false
          }
          {
            field: 'tags[Project]'
            exists: false
          }
          {
            field: 'tags[CostCenter]'
            exists: false
          }
          {
            field: 'tags[Owner]'
            exists: false
          }
        ]
      }
      then: {
        effect: 'deny'
      }
    }
  }
}

// Policy: Allowed Resource Locations (GDPR Compliance)
resource allowedLocationsPolicy 'Microsoft.Authorization/policyDefinitions@2021-06-01' = {
  name: 'custom-allowed-locations'
  properties: {
    displayName: 'Allowed Resource Locations for GDPR'
    description: 'Restricts resource deployment to EU regions for GDPR compliance'
    policyType: 'Custom'
    mode: 'All'
    parameters: {
      allowedLocations: {
        type: 'Array'
        defaultValue: [
          'westeurope'
          'northeurope'
          'francecentral'
          'germanywestcentral'
        ]
      }
    }
    policyRule: {
      if: {
        not: {
          field: 'location'
          in: '[parameters(\'allowedLocations\')]'
        }
      }
      then: {
        effect: 'deny'
      }
    }
  }
}

// Policy Assignment: Apply to Management Group
resource policyAssignment 'Microsoft.Authorization/policyAssignments@2022-06-01' = {
  name: 'security-governance-policies'
  properties: {
    displayName: 'Security Learning Governance Policies'
    description: 'Applies security and governance policies to learning environment'
    policyDefinitionId: httpsStoragePolicy.id
    parameters: {}
    enforcementMode: 'Default'
  }
}

output httpsStoragePolicyId string = httpsStoragePolicy.id
output requiredTagsPolicyId string = requiredTagsPolicy.id
output allowedLocationsPolicyId string = allowedLocationsPolicy.id
output policyAssignmentId string = policyAssignment.id
