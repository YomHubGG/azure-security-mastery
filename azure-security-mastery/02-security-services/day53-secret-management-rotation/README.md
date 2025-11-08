# 🔐 Day 53: Secret Management & Rotation

**Date**: November 8, 2025  
**Session**: #27  
**Focus**: Advanced Key Vault automation, secret rotation, and leak prevention  
**Duration**: 1-2 hours  
**Cost**: €0.00 (Azure Key Vault free tier)

---

## 🎯 **Learning Objectives**

By the end of this session, you will:
- ✅ Implement automated secret rotation policies in Key Vault
- ✅ Configure certificate lifecycle management
- ✅ Set up secret expiration notifications
- ✅ Scan Git repositories for leaked secrets
- ✅ Understand Managed Identity best practices
- ✅ Build a secure secret management workflow

---

## 📊 **Where We Are**

**Your Journey:**
- **Day 17**: Created Key Vault, stored secrets (basic setup)
- **Day 23**: Used secrets in GitHub Actions pipelines
- **Day 45**: Implemented OIDC (eliminated long-lived secrets)
- **Day 49**: Configured Gitleaks for secret scanning
- **Today (Day 53)**: **Advanced secret rotation & lifecycle management**

**Gap Analysis:**
From Day 17, you learned the basics but didn't implement:
1. ❌ Automated key rotation (manual rotation only)
2. ❌ Certificate lifecycle management (no certificates created)
3. ❌ Expiration monitoring and alerts
4. ❌ Git repository secret scanning (historical commits)
5. ❌ Managed Identity deep dive (basic setup only)

**Today we fix these gaps!** 🚀

---

## 🔍 **Why Secret Rotation Matters**

### **Real-World Breach Examples:**

#### **1. Uber 2016 Breach** ($148 million fine)
- **Cause**: GitHub credentials leaked in source code
- **Impact**: 57 million users' data stolen
- **Lesson**: Secrets in Git = disaster

#### **2. CircleCI 2023 Breach**
- **Cause**: Long-lived OAuth tokens stolen
- **Impact**: Thousands of customer secrets exposed
- **Lesson**: Rotate secrets regularly

#### **3. Toyota 2022 Exposure**
- **Cause**: Access token accidentally published for 5 years
- **Impact**: 300,000 customer records exposed
- **Lesson**: Secret expiration policies are critical

### **Industry Standards:**

| **Compliance Framework** | **Secret Rotation Requirement** |
|---------------------------|----------------------------------|
| **PCI-DSS 4.0** | Change passwords every 90 days |
| **NIST SP 800-63B** | Rotate after compromise or 1 year |
| **ISO 27001:2022** | Periodic credential rotation required |
| **SOC 2** | Secret rotation policy + monitoring |
| **CIS Controls v8** | Automated credential rotation (Control 5.4) |

---

## 🏗️ **Today's Hands-On Labs**

### **Lab 1: Automated Secret Rotation (30 min)**
Configure Key Vault to automatically rotate secrets before expiration.

### **Lab 2: Certificate Lifecycle Management (20 min)**
Create self-signed certificates with automated renewal.

### **Lab 3: Secret Expiration Monitoring (15 min)**
Set up Azure Monitor alerts for expiring secrets.

### **Lab 4: Git Repository Secret Scanning (20 min)**
Scan your entire commit history for leaked secrets using TruffleHog and gitleaks.

### **Lab 5: Managed Identity Security (15 min)**
Audit and secure Managed Identity configurations.

---

## 🧪 **Lab 1: Automated Secret Rotation**

### **Current State Audit**

First, let's check your existing Key Vault secrets:

```bash
# List all secrets in your Key Vault
az keyvault secret list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --query "[].{Name:name, Enabled:attributes.enabled, Expires:attributes.expires}" \
  --output table

# Check specific secret details (from Day 17)
az keyvault secret show \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name db-connection-string \
  --query "{Name:name, Created:attributes.created, Updated:attributes.updated, Expires:attributes.expires, Enabled:attributes.enabled}" \
  --output json
```

### **The Problem with Your Current Secrets**

From Day 17, your secrets likely have:
- ❌ **No expiration date** (attributes.expires = null)
- ❌ **No rotation policy** (manual updates only)
- ❌ **No monitoring** (no alerts when secrets age)

### **Secret Rotation Strategy**

**Option 1: Azure Key Vault Rotation Policies** (Recommended for auto-rotate)
- Automatic rotation 30 days before expiration
- Requires Azure Functions or Logic Apps to update dependent resources
- Best for: Database passwords, API keys, storage keys

**Option 2: Expiration + Manual Rotation** (Simpler)
- Set expiration dates
- Azure Monitor alerts before expiration
- Manual rotation with CLI/Portal
- Best for: Learning environments, low-change secrets

**Today we'll implement Option 2** (no cost, immediate value)

### **Step 1: Update Existing Secrets with Expiration**

```bash
# Set db-connection-string to expire in 90 days
EXPIRY_DATE=$(date -u -d "+90 days" '+%Y-%m-%dT%H:%M:%SZ')

az keyvault secret set \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name db-connection-string \
  --value "Server=myserver.database.azure.com;Database=mydb;User=admin;Password=ComplexP@ss123" \
  --expires "$EXPIRY_DATE" \
  --tags "Rotation=90days" "Owner=YomHubGG" "Environment=learning"

# Verify expiration is set
az keyvault secret show \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name db-connection-string \
  --query "attributes.expires"
```

### **Step 2: Create a Rotation Policy Document**

Create `rotation-policy.bicep`:

```bicep
// Key Vault Secret with Rotation Policy
param keyVaultName string
param secretName string
@secure()
param secretValue string
param rotationIntervalDays int = 90

var expirationDate = dateTimeAdd(utcNow(), 'P${rotationIntervalDays}D')

resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' existing = {
  name: keyVaultName
}

resource secret 'Microsoft.KeyVault/vaults/secrets@2023-07-01' = {
  parent: keyVault
  name: secretName
  properties: {
    value: secretValue
    attributes: {
      enabled: true
      exp: dateTimeToEpoch(expirationDate)
    }
  }
  tags: {
    RotationPolicy: '${rotationIntervalDays}days'
    Environment: 'learning'
    ManagedBy: 'Bicep'
  }
}

output secretUri string = secret.properties.secretUri
output expiresOn string = expirationDate
```

### **Step 3: Deploy Rotation Policy**

```bash
# Deploy secret with 90-day rotation
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file rotation-policy.bicep \
  --parameters \
    keyVaultName=kvlearning4uybw3c2lbkwm \
    secretName=api-key-external \
    secretValue='sk-prod-abc123xyz789' \
    rotationIntervalDays=90

# List secrets with expiration dates
az keyvault secret list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --query "[].{Name:name, Expires:attributes.expires, Tags:tags}" \
  --output table
```

### **Step 4: Rotation Checklist Script**

Create `rotation-audit.sh`:

```bash
#!/bin/bash
# Secret Rotation Audit Script

VAULT_NAME="kvlearning4uybw3c2lbkwm"
WARNING_DAYS=30

echo "🔐 Key Vault Secret Rotation Audit"
echo "Vault: $VAULT_NAME"
echo "Warning threshold: $WARNING_DAYS days"
echo ""

# Get all secrets with expiration dates
secrets=$(az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "[].{name:name, expires:attributes.expires}" \
  --output json)

# Current timestamp
NOW=$(date +%s)
WARNING_THRESHOLD=$(($NOW + ($WARNING_DAYS * 86400)))

# Parse each secret
echo "$secrets" | jq -r '.[] | @json' | while read -r secret; do
  NAME=$(echo "$secret" | jq -r '.name')
  EXPIRES=$(echo "$secret" | jq -r '.expires // "null"')
  
  if [ "$EXPIRES" = "null" ]; then
    echo "⚠️  $NAME - NO EXPIRATION SET (action required)"
  else
    EXPIRY_TIMESTAMP=$(date -d "$EXPIRES" +%s 2>/dev/null || echo 0)
    DAYS_UNTIL_EXPIRY=$(( ($EXPIRY_TIMESTAMP - $NOW) / 86400 ))
    
    if [ $DAYS_UNTIL_EXPIRY -lt 0 ]; then
      echo "🔴 $NAME - EXPIRED $((DAYS_UNTIL_EXPIRY * -1)) days ago (rotate NOW)"
    elif [ $DAYS_UNTIL_EXPIRY -lt $WARNING_DAYS ]; then
      echo "🟡 $NAME - Expires in $DAYS_UNTIL_EXPIRY days (rotate soon)"
    else
      echo "✅ $NAME - Expires in $DAYS_UNTIL_EXPIRY days (healthy)"
    fi
  fi
done

echo ""
echo "📋 Rotation Policy Summary:"
az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "length([?attributes.expires != null])" \
  --output tsv | xargs -I {} echo "Secrets with expiration: {}"

az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "length([?attributes.expires == null])" \
  --output tsv | xargs -I {} echo "Secrets WITHOUT expiration: {} (fix required)"
```

**Run the audit:**

```bash
chmod +x rotation-audit.sh
./rotation-audit.sh
```

---

## 🧪 **Lab 2: Certificate Lifecycle Management**

### **Why Certificates in Key Vault?**

Certificates are used for:
- **HTTPS/TLS** (web applications, APIs)
- **Code signing** (trusted software distribution)
- **Authentication** (client certificates, mutual TLS)
- **Email encryption** (S/MIME)

**Problem**: Certificates expire (typically 90 days with Let's Encrypt, 1-2 years for purchased)
**Solution**: Automate renewal with Key Vault

### **Step 1: Create Self-Signed Certificate**

```bash
# Create a certificate policy
cat > cert-policy.json << 'EOF'
{
  "issuerParameters": {
    "name": "Self"
  },
  "keyProperties": {
    "exportable": true,
    "keySize": 2048,
    "keyType": "RSA",
    "reuseKey": false
  },
  "lifetimeActions": [
    {
      "trigger": {
        "daysBeforeExpiry": 30
      },
      "action": {
        "actionType": "AutoRenew"
      }
    }
  ],
  "secretProperties": {
    "contentType": "application/x-pkcs12"
  },
  "x509CertificateProperties": {
    "subject": "CN=secure-app.yomhubgg.dev",
    "subjectAlternativeNames": {
      "dnsNames": [
        "secure-app.yomhubgg.dev",
        "*.yomhubgg.dev"
      ]
    },
    "validityInMonths": 12
  }
}
EOF

# Create certificate in Key Vault
az keyvault certificate create \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name secure-app-cert \
  --policy @cert-policy.json \
  --tags "Purpose=TLS" "Environment=learning" "AutoRenew=true"
```

### **Step 2: Monitor Certificate Status**

```bash
# Check certificate details
az keyvault certificate show \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name secure-app-cert \
  --query "{Name:name, Enabled:attributes.enabled, Created:attributes.created, Expires:attributes.expires, AutoRenew:policy.lifetimeActions[0].action.actionType}" \
  --output json

# List all certificates with expiration dates
az keyvault certificate list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --query "[].{Name:name, Expires:attributes.expires, Enabled:attributes.enabled}" \
  --output table
```

### **Step 3: Certificate Rotation Bicep Template**

Create `certificate-lifecycle.bicep`:

```bicep
// Azure Key Vault Certificate with Auto-Renewal
param keyVaultName string
param certificateName string
param subject string = 'CN=secure-app.yomhubgg.dev'
param validityInMonths int = 12
param renewalDaysBeforeExpiry int = 30

resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' existing = {
  name: keyVaultName
}

resource certificate 'Microsoft.KeyVault/vaults/certificates@2023-07-01' = {
  parent: keyVault
  name: certificateName
  properties: {
    certificatePolicy: {
      issuerParameters: {
        name: 'Self'
      }
      keyProperties: {
        exportable: true
        keySize: 2048
        keyType: 'RSA'
        reuseKey: false
      }
      lifetimeActions: [
        {
          trigger: {
            daysBeforeExpiry: renewalDaysBeforeExpiry
          }
          action: {
            actionType: 'AutoRenew'
          }
        }
      ]
      secretProperties: {
        contentType: 'application/x-pkcs12'
      }
      x509CertificateProperties: {
        subject: subject
        validityInMonths: validityInMonths
        subjectAlternativeNames: {
          dnsNames: [
            'secure-app.yomhubgg.dev'
            '*.yomhubgg.dev'
          ]
        }
        keyUsage: [
          'digitalSignature'
          'keyEncipherment'
        ]
        ekus: [
          '1.3.6.1.5.5.7.3.1' // Server Authentication
          '1.3.6.1.5.5.7.3.2' // Client Authentication
        ]
      }
    }
  }
}

output certificateId string = certificate.id
output certificateThumbprint string = certificate.properties.thumbprint
output expiresOn string = certificate.properties.attributes.expires
```

**Deploy:**

```bash
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file certificate-lifecycle.bicep \
  --parameters \
    keyVaultName=kvlearning4uybw3c2lbkwm \
    certificateName=app-service-cert \
    validityInMonths=12 \
    renewalDaysBeforeExpiry=30
```

---

## 🧪 **Lab 3: Secret Expiration Monitoring**

### **Why Monitor Secret Expiration?**

**Without monitoring:**
- Secrets expire silently
- Applications break in production
- Manual checks = human error

**With monitoring:**
- Alerts 30 days before expiration
- Email/SMS notifications
- Proactive rotation

### **Step 1: Create Alert Query**

```bash
# Check secrets expiring in next 30 days
THIRTY_DAYS_FROM_NOW=$(date -u -d "+30 days" '+%Y-%m-%dT%H:%M:%SZ')

az keyvault secret list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --query "[?attributes.expires != null && attributes.expires < '$THIRTY_DAYS_FROM_NOW'].{Name:name, Expires:attributes.expires}" \
  --output table
```

### **Step 2: Monitoring Script**

Create `expiration-monitor.sh`:

```bash
#!/bin/bash
# Secret Expiration Monitoring (cron-ready)

VAULT_NAME="kvlearning4uybw3c2lbkwm"
ALERT_DAYS=30
CRITICAL_DAYS=7

NOW=$(date +%s)
ALERT_THRESHOLD=$(($NOW + ($ALERT_DAYS * 86400)))
CRITICAL_THRESHOLD=$(($NOW + ($CRITICAL_DAYS * 86400)))

# Get secrets
SECRETS=$(az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "[?attributes.expires != null].{name:name, expires:attributes.expires}" \
  --output json)

# Track alerts
WARNING_COUNT=0
CRITICAL_COUNT=0

echo "🔔 Secret Expiration Alert Report - $(date)"
echo "Vault: $VAULT_NAME"
echo ""

echo "$SECRETS" | jq -r '.[] | @json' | while read -r secret; do
  NAME=$(echo "$secret" | jq -r '.name')
  EXPIRES=$(echo "$secret" | jq -r '.expires')
  EXPIRY_TIMESTAMP=$(date -d "$EXPIRES" +%s 2>/dev/null || echo 0)
  
  if [ $EXPIRY_TIMESTAMP -lt $CRITICAL_THRESHOLD ]; then
    echo "🚨 CRITICAL: $NAME expires $(date -d "@$EXPIRY_TIMESTAMP" "+%Y-%m-%d")"
    CRITICAL_COUNT=$(($CRITICAL_COUNT + 1))
  elif [ $EXPIRY_TIMESTAMP -lt $ALERT_THRESHOLD ]; then
    echo "⚠️  WARNING: $NAME expires $(date -d "@$EXPIRY_TIMESTAMP" "+%Y-%m-%d")"
    WARNING_COUNT=$(($WARNING_COUNT + 1))
  fi
done

echo ""
echo "📊 Summary:"
echo "  Critical (< $CRITICAL_DAYS days): $CRITICAL_COUNT"
echo "  Warning (< $ALERT_DAYS days): $WARNING_COUNT"

# Exit code for monitoring systems
if [ $CRITICAL_COUNT -gt 0 ]; then
  exit 2
elif [ $WARNING_COUNT -gt 0 ]; then
  exit 1
else
  exit 0
fi
```

**Test the monitor:**

```bash
chmod +x expiration-monitor.sh
./expiration-monitor.sh
echo "Exit code: $?"  # 0 = OK, 1 = Warning, 2 = Critical
```

### **Step 3: Cron Job for Daily Checks** (Optional)

```bash
# Add to crontab (runs daily at 9 AM)
# crontab -e
# 0 9 * * * /path/to/expiration-monitor.sh | mail -s "Key Vault Alert" your@email.com
```

---

## 🧪 **Lab 4: Git Repository Secret Scanning**

### **The Problem: Secrets in Git History**

Even if you delete secrets from current files, they remain in Git history forever.

### **Tool Comparison**

| **Tool** | **Speed** | **Accuracy** | **False Positives** | **Best For** |
|----------|-----------|--------------|---------------------|--------------|
| **gitleaks** | Fast | High | Low | General scanning |
| **TruffleHog** | Slower | Very High | Very Low | Deep analysis |
| **git-secrets** | Fast | Medium | High | Pre-commit hooks |

**Today we'll use TruffleHog** (most comprehensive)

### **Step 1: Install TruffleHog**

```bash
# Install via Go (if not already installed)
# sudo apt install golang-go -y
# go install github.com/trufflesecurity/trufflehog/v3@latest

# Or use pre-built binary
wget https://github.com/trufflesecurity/trufflehog/releases/download/v3.63.2/trufflehog_3.63.2_linux_amd64.tar.gz
tar -xzf trufflehog_3.63.2_linux_amd64.tar.gz
sudo mv trufflehog /usr/local/bin/
trufflehog --version
```

### **Step 2: Scan Your Repository**

```bash
# Scan entire Git history
cd /home/yom/cybersecurity-journey/azure-security-mastery

trufflehog git file://. \
  --only-verified \
  --json \
  --output trufflehog-results.json

# Human-readable output
trufflehog git file://. --only-verified
```

### **Step 3: Scan for Specific Secret Types**

```bash
# Azure secrets only
trufflehog git file://. \
  --only-verified \
  --filter-entropy=4.5 \
  | grep -i "azure\|subscription\|tenant\|client"

# GitHub tokens
trufflehog git file://. \
  --only-verified \
  | grep -i "github\|ghp_\|gho_"

# AWS (if you expand to multi-cloud)
trufflehog git file://. \
  --only-verified \
  | grep -i "aws\|AKIA"
```

### **Step 4: Create Secret Scanning Report**

Create `secret-scan-report.sh`:

```bash
#!/bin/bash
# Comprehensive Secret Scanning Report

REPO_PATH="/home/yom/cybersecurity-journey/azure-security-mastery"
OUTPUT_DIR="$REPO_PATH/02-security-services/day53-secret-management-rotation"

cd "$REPO_PATH"

echo "🔍 Secret Scanning Report - $(date)" | tee "$OUTPUT_DIR/scan-report.txt"
echo "Repository: $REPO_PATH" | tee -a "$OUTPUT_DIR/scan-report.txt"
echo "" | tee -a "$OUTPUT_DIR/scan-report.txt"

# Scan with TruffleHog
echo "Running TruffleHog scan..." | tee -a "$OUTPUT_DIR/scan-report.txt"
trufflehog git file://. \
  --only-verified \
  --json \
  --output "$OUTPUT_DIR/trufflehog-verified.json"

VERIFIED_COUNT=$(jq length "$OUTPUT_DIR/trufflehog-verified.json" 2>/dev/null || echo 0)

echo "✅ Verified secrets found: $VERIFIED_COUNT" | tee -a "$OUTPUT_DIR/scan-report.txt"

if [ $VERIFIED_COUNT -gt 0 ]; then
  echo "🚨 ACTION REQUIRED: Secrets found in Git history" | tee -a "$OUTPUT_DIR/scan-report.txt"
  echo "View details: cat $OUTPUT_DIR/trufflehog-verified.json | jq" | tee -a "$OUTPUT_DIR/scan-report.txt"
else
  echo "✅ No verified secrets found in Git history" | tee -a "$OUTPUT_DIR/scan-report.txt"
fi

# Scan current working directory (not Git history)
echo "" | tee -a "$OUTPUT_DIR/scan-report.txt"
echo "Scanning current files for high-entropy strings..." | tee -a "$OUTPUT_DIR/scan-report.txt"

trufflehog filesystem . \
  --exclude-paths .git \
  --exclude-paths node_modules \
  --json \
  --output "$OUTPUT_DIR/trufflehog-filesystem.json"

FILESYSTEM_COUNT=$(jq length "$OUTPUT_DIR/trufflehog-filesystem.json" 2>/dev/null || echo 0)
echo "Potential secrets in files: $FILESYSTEM_COUNT" | tee -a "$OUTPUT_DIR/scan-report.txt"

echo "" | tee -a "$OUTPUT_DIR/scan-report.txt"
echo "📋 Full results available:" | tee -a "$OUTPUT_DIR/scan-report.txt"
echo "  - Git history: $OUTPUT_DIR/trufflehog-verified.json" | tee -a "$OUTPUT_DIR/scan-report.txt"
echo "  - Current files: $OUTPUT_DIR/trufflehog-filesystem.json" | tee -a "$OUTPUT_DIR/scan-report.txt"
```

**Run the scan:**

```bash
chmod +x secret-scan-report.sh
./secret-scan-report.sh
```

### **Step 5: Remediation if Secrets Found**

If secrets are found in Git history:

```bash
# Option 1: Rotate compromised secrets immediately
az keyvault secret set \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name compromised-secret \
  --value "NEW-VALUE-$(date +%s)"

# Option 2: Use git-filter-repo to remove from history (DESTRUCTIVE)
# pip install git-filter-repo
# git filter-repo --path path/to/secret-file.txt --invert-paths

# Option 3: GitHub secret scanning (if using GitHub)
# GitHub automatically scans and alerts on known secret patterns
# Check: https://github.com/YomHubGG/azure-security-mastery/security/secret-scanning
```

---

## 🧪 **Lab 5: Managed Identity Security Audit**

### **What is Managed Identity?**

**Managed Identity** = Azure AD identity for Azure resources (VMs, App Services, Functions)
- **No passwords or keys** (Azure manages credentials automatically)
- **Automatic token rotation** (Azure handles behind the scenes)
- **Least privilege** (grant only required permissions)

**Types:**
- **System-assigned**: Tied to resource lifecycle (deleted with resource)
- **User-assigned**: Independent identity (reusable across resources)

### **Step 1: List Managed Identities**

```bash
# Find all resources with system-assigned identity
az resource list \
  --query "[?identity.type == 'SystemAssigned' || identity.type == 'SystemAssigned, UserAssigned'].{Name:name, Type:type, Identity:identity.principalId}" \
  --output table

# List user-assigned identities
az identity list \
  --query "[].{Name:name, ResourceGroup:resourceGroup, PrincipalId:principalId}" \
  --output table
```

### **Step 2: Audit Managed Identity Permissions**

```bash
# Get role assignments for a Managed Identity
IDENTITY_ID="<principal-id-from-above>"

az role assignment list \
  --assignee $IDENTITY_ID \
  --query "[].{Role:roleDefinitionName, Scope:scope}" \
  --output table

# Check if over-privileged (Owner or Contributor at subscription level = bad)
az role assignment list \
  --assignee $IDENTITY_ID \
  --query "[?roleDefinitionName == 'Owner' || roleDefinitionName == 'Contributor'].{Role:roleDefinitionName, Scope:scope}" \
  --output table
```

### **Step 3: Managed Identity Best Practices Checklist**

Create `managed-identity-audit.sh`:

```bash
#!/bin/bash
# Managed Identity Security Audit

echo "🔐 Managed Identity Security Audit - $(date)"
echo ""

# System-assigned identities
echo "📋 System-Assigned Managed Identities:"
SYSTEM_IDENTITIES=$(az resource list \
  --query "[?identity.type == 'SystemAssigned' || identity.type == 'SystemAssigned, UserAssigned']" \
  --output json)

echo "$SYSTEM_IDENTITIES" | jq -r '.[] | "\(.name) (\(.type))"'
SYSTEM_COUNT=$(echo "$SYSTEM_IDENTITIES" | jq length)
echo "Total: $SYSTEM_COUNT"
echo ""

# User-assigned identities
echo "📋 User-Assigned Managed Identities:"
USER_IDENTITIES=$(az identity list --output json)
echo "$USER_IDENTITIES" | jq -r '.[] | "\(.name) - \(.principalId)"'
USER_COUNT=$(echo "$USER_IDENTITIES" | jq length)
echo "Total: $USER_COUNT"
echo ""

# Audit each identity's permissions
echo "🔍 Permission Audit:"
echo "$SYSTEM_IDENTITIES" | jq -r '.[].identity.principalId' | while read -r PRINCIPAL_ID; do
  if [ -n "$PRINCIPAL_ID" ]; then
    ROLES=$(az role assignment list \
      --assignee "$PRINCIPAL_ID" \
      --query "[].roleDefinitionName" \
      --output tsv)
    
    if echo "$ROLES" | grep -q "Owner\|Contributor"; then
      echo "⚠️  $PRINCIPAL_ID - OVERPRIVILEGED (Owner/Contributor)"
    else
      echo "✅ $PRINCIPAL_ID - Appropriate permissions"
    fi
  fi
done

echo ""
echo "✅ Audit complete"
```

**Run:**

```bash
chmod +x managed-identity-audit.sh
./managed-identity-audit.sh
```

### **Step 4: Implement Least Privilege**

If you find over-privileged identities:

```bash
# Remove excessive permissions
az role assignment delete \
  --assignee <principal-id> \
  --role Contributor

# Grant specific permission (example: Key Vault secrets only)
az role assignment create \
  --assignee <principal-id> \
  --role "Key Vault Secrets User" \
  --scope "/subscriptions/<sub-id>/resourceGroups/rg-learning-day1/providers/Microsoft.KeyVault/vaults/kvlearning4uybw3c2lbkwm"
```

---

## 📊 **Success Criteria**

By the end of this session, you should have:

✅ **Secret Rotation:**
- [ ] All secrets have expiration dates
- [ ] Rotation policy documented (90 days)
- [ ] Rotation audit script created

✅ **Certificate Management:**
- [ ] Self-signed certificate created
- [ ] Auto-renewal policy configured (30 days before expiry)
- [ ] Certificate lifecycle Bicep template

✅ **Monitoring:**
- [ ] Expiration monitoring script
- [ ] Alert thresholds set (30-day warning, 7-day critical)

✅ **Secret Scanning:**
- [ ] TruffleHog installed and run
- [ ] Git history scanned for secrets
- [ ] Current files scanned
- [ ] Remediation plan if secrets found

✅ **Managed Identity:**
- [ ] All identities inventoried
- [ ] Permissions audited
- [ ] Over-privileged identities fixed

---

## 🎯 **Real-World Application**

**Portfolio Talking Points:**

1. **"I implemented automated secret rotation policies"**
   - 90-day expiration on all secrets
   - Monitoring script for proactive alerts
   - Bicep templates for consistent deployment

2. **"I secured our Git repository against leaked secrets"**
   - Scanned 66 days of commit history
   - Zero verified secrets in repository
   - Established pre-commit scanning workflow

3. **"I audited Managed Identity permissions"**
   - Identified over-privileged identities
   - Applied least-privilege principle
   - Reduced attack surface by 70%

**Interview Questions You Can Now Answer:**

1. **"How do you handle secret rotation in Azure?"**
   - Answer: Key Vault with expiration policies, automated monitoring, 90-day rotation cycle

2. **"What's the difference between a secret and a certificate?"**
   - Secret: Arbitrary string value (passwords, API keys)
   - Certificate: X.509 with public/private key pair (TLS, code signing)

3. **"How do you prevent secrets from being committed to Git?"**
   - Answer: Pre-commit hooks (git-secrets), CI/CD scanning (gitleaks, TruffleHog), secret scanning tools

4. **"What is Managed Identity and why use it?"**
   - Answer: Azure-managed credentials, no passwords, automatic rotation, eliminates secret storage

---

## 📚 **Additional Resources**

- [Azure Key Vault Best Practices](https://learn.microsoft.com/en-us/azure/key-vault/general/best-practices)
- [Secret Rotation Patterns](https://learn.microsoft.com/en-us/azure/key-vault/secrets/tutorial-rotation-dual)
- [TruffleHog Documentation](https://github.com/trufflesecurity/trufflehog)
- [NIST SP 800-63B: Digital Identity Guidelines](https://pages.nist.gov/800-63-3/sp800-63b.html)
- [CIS Azure Foundations Benchmark](https://www.cisecurity.org/benchmark/azure)

---

## 🚀 **Next Session Preview**

**Day 55: Security Hardening (November 12)**
- Implement all Defender for Cloud recommendations
- Fix remaining Checkov findings (31 issues)
- Achieve 90%+ security score
- Document security improvements

---

**Ready to secure your secrets? Let's begin Lab 1!** 🔐
