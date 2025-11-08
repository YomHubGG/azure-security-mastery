// 🔐 Azure Key Vault Certificate with Auto-Renewal Policy
// Creates self-signed certificate with automatic renewal before expiration

@description('Name of the existing Key Vault')
param keyVaultName string

@description('Name of the certificate to create')
param certificateName string

@description('Certificate subject (CN=...)')
param subject string = 'CN=secure-app.yomhubgg.dev'

@description('Certificate validity period in months')
@minValue(1)
@maxValue(24)
param validityInMonths int = 12

@description('Number of days before expiry to trigger auto-renewal')
@minValue(7)
@maxValue(90)
param renewalDaysBeforeExpiry int = 30

@description('Environment tag')
@allowed([
  'production'
  'staging'
  'development'
  'learning'
])
param environment string = 'learning'

// Reference existing Key Vault
resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' existing = {
  name: keyVaultName
}

// Create certificate with auto-renewal
resource certificate 'Microsoft.KeyVault/vaults/certificates@2023-07-01' = {
  parent: keyVault
  name: certificateName
  properties: {
    certificatePolicy: {
      // Issuer: Self-signed (change to 'DigiCert' or 'GlobalSign' for production)
      issuerParameters: {
        name: 'Self'
      }
      
      // Key properties
      keyProperties: {
        exportable: true
        keySize: 2048
        keyType: 'RSA'
        reuseKey: false
      }
      
      // Lifetime actions (auto-renewal)
      lifetimeActions: [
        {
          trigger: {
            daysBeforeExpiry: renewalDaysBeforeExpiry
          }
          action: {
            actionType: 'AutoRenew'
          }
        }
        {
          trigger: {
            daysBeforeExpiry: 7
          }
          action: {
            actionType: 'EmailContacts'
          }
        }
      ]
      
      // Secret properties (how cert is stored)
      secretProperties: {
        contentType: 'application/x-pkcs12'
      }
      
      // X.509 certificate properties
      x509CertificateProperties: {
        subject: subject
        validityInMonths: validityInMonths
        
        // Subject Alternative Names (SANs)
        subjectAlternativeNames: {
          dnsNames: [
            'secure-app.yomhubgg.dev'
            '*.yomhubgg.dev'
          ]
        }
        
        // Key usage
        keyUsage: [
          'digitalSignature'
          'keyEncipherment'
        ]
        
        // Extended Key Usage (EKU)
        ekus: [
          '1.3.6.1.5.5.7.3.1' // Server Authentication (TLS/SSL)
          '1.3.6.1.5.5.7.3.2' // Client Authentication
        ]
      }
    }
  }
  tags: {
    Environment: environment
    Purpose: 'TLS/SSL'
    AutoRenew: 'true'
    RenewalDays: string(renewalDaysBeforeExpiry)
    ValidityMonths: string(validityInMonths)
    ManagedBy: 'Bicep'
  }
}

// Outputs
output certificateId string = certificate.id
output certificateName string = certificate.name
output certificateThumbprint string = certificate.properties.thumbprint
output expiresOn string = certificate.properties.attributes.expires
output subject string = subject
output validityMonths int = validityInMonths
output autoRenewalDays int = renewalDaysBeforeExpiry
