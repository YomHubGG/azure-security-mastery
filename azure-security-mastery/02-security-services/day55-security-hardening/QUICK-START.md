# Day 55 Quick Start - Security Hardening

**Goal**: Improve security posture from 73% → 90%+ in one session.

## 🚀 Fast Track (30 minutes)

### Step 1: Baseline Assessment (5 min)

```bash
cd ~/cybersecurity-journey/azure-security-mastery/02-security-services/day55-security-hardening/

# Run Checkov baseline
checkov -d ../../ --framework bicep --compact | tee baseline-scan.txt

# Count results
grep "Passed checks:" baseline-scan.txt
# Expected: Passed checks: 43, Failed checks: 16
```

### Step 2: Create Hardened Storage Template (10 min)

```bash
# Create template
nano storage-hardened.bicep
```

Paste this:
```bicep
@description('Storage account name (must be globally unique)')
param storageAccountName string = 'sthardened${uniqueString(resourceGroup().id)}'

@description('Location for all resources')
param location string = resourceGroup().location

resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  kind: 'StorageV2'
  sku: {
    name: 'Standard_LRS'
  }
  properties: {
    // ✅ CKV_AZURE_33: HTTPS only
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    
    // ✅ CKV_AZURE_36: Disable public access
    allowBlobPublicAccess: false
    publicNetworkAccess: 'Disabled'
    
    // ✅ CKV_AZURE_3: Encryption for all services
    encryption: {
      services: {
        blob: { enabled: true }
        file: { enabled: true }
        queue: { enabled: true }
        table: { enabled: true }
      }
      keySource: 'Microsoft.Storage'
    }
    
    // ✅ CKV_AZURE_35: Network ACLs
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
    }
  }
  tags: {
    Environment: 'Learning'
    SecurityLevel: 'Hardened'
    Compliance: 'CIS-Azure-3.x'
  }
}

output storageAccountId string = storageAccount.id
output storageAccountName string = storageAccount.name
```

### Step 3: Deploy Hardened Storage (3 min)

```bash
# Create resource group
az group create --name rg-security-hardening --location eastus

# Deploy template
az deployment group create \
  --resource-group rg-security-hardening \
  --template-file storage-hardened.bicep \
  --query 'properties.outputs'

# Verify deployment
az storage account show \
  --name $(az deployment group show -g rg-security-hardening -n storage-hardened --query 'properties.outputs.storageAccountName.value' -o tsv) \
  --query '{HTTPS:supportsHttpsTrafficOnly, TLS:minimumTlsVersion, PublicAccess:publicNetworkAccess}' \
  -o table
```

### Step 4: Create Audit Script (5 min)

```bash
nano azure-hardening-audit.sh
chmod +x azure-hardening-audit.sh
```

Paste script from README.md Part 6.1, then run:

```bash
./azure-hardening-audit.sh
```

### Step 5: Re-scan and Compare (5 min)

```bash
# Re-run Checkov
checkov -d ../../ --framework bicep --compact | tee after-scan.txt

# Compare results
echo "BEFORE:"
grep "Passed checks:" baseline-scan.txt

echo "AFTER:"
grep "Passed checks:" after-scan.txt

# Calculate improvement
python3 -c "
before = 43  # Update from your baseline
after = $(grep 'Passed checks:' after-scan.txt | awk '{print $3}' | tr -d ',')
print(f'Improvement: +{after - before} checks ({(after - before) / before * 100:.1f}%)')
"
```

### Step 6: Document Results (2 min)

```bash
nano LAB-RESULTS.md
```

Record:
- Baseline: X% pass rate
- After hardening: Y% pass rate
- Improvement: +Z%
- Time taken: ~30 minutes
- Cost: €0.00

## ✅ Success Criteria

- [ ] Checkov pass rate ≥ 90%
- [ ] Storage account deployed with 5+ hardening measures
- [ ] Audit script runs successfully
- [ ] LAB-RESULTS.md documents improvements

## 🚨 Troubleshooting

**Issue**: Checkov not installed
```bash
pip3 install checkov
```

**Issue**: Azure CLI not authenticated
```bash
az login
az account set --subscription <subscription-id>
```

**Issue**: Resource group already exists
```bash
az group delete --name rg-security-hardening --yes --no-wait
# Wait 2 minutes, then retry deployment
```

## 📊 Expected Outcome

Before:
```
Passed checks: 43
Failed checks: 16
Pass rate: 73%
```

After:
```
Passed checks: 52+
Failed checks: 7-
Pass rate: 90%+
```

**Time**: 30 minutes  
**Cost**: €0.00  
**Skills**: IaC security, compliance automation

Ready? Let's harden! 🔒
