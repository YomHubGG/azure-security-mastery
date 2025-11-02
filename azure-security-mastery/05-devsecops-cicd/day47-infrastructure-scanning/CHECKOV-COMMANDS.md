# Checkov Command Reference - Day 47

Quick reference for Infrastructure-as-Code security scanning with Checkov.

---

## 🚀 Basic Commands

### **Installation**
```bash
# Virtual environment (recommended)
python3 -m venv ~/.venv/checkov
source ~/.venv/checkov/bin/activate
pip install checkov

# Verify installation
checkov --version
```

### **Simple Scans**
```bash
# Scan single file
checkov -f path/to/file.bicep

# Scan directory
checkov -d path/to/directory

# Scan current directory
checkov -d .

# Scan specific framework
checkov -d . --framework bicep
checkov -d . --framework terraform
checkov -d . --framework kubernetes
```

---

## 📋 Output Formats

### **Console Output (Human-Readable)**
```bash
# Compact output (less verbose)
checkov -d . --compact

# Quiet mode (errors only)
checkov -d . --quiet

# Detailed output (default)
checkov -d .
```

### **JSON Output (Machine-Readable)**
```bash
# JSON output
checkov -d . --output json

# Save to file
checkov -d . --output json > results.json

# Pretty-printed JSON
checkov -d . --output json | jq '.'
```

### **SARIF Output (GitHub Security Tab)**
```bash
# SARIF format for CI/CD
checkov -d . --output sarif > results.sarif

# Upload to GitHub (in Actions workflow)
- uses: github/codeql-action/upload-sarif@v3
  with:
    sarif_file: results.sarif
```

### **Multiple Outputs**
```bash
# CLI + JSON + SARIF
checkov -d . \
  --output cli \
  --output json \
  --output sarif \
  --output-file-path results/
```

---

## 🎯 Framework-Specific Scans

### **Bicep (Azure)**
```bash
# Scan all Bicep files
checkov -d . --framework bicep

# Scan specific Bicep file
checkov -f storage-account.bicep --framework bicep

# Scan with Azure-specific checks only
checkov -d . --framework bicep --check CKV_AZURE_*
```

### **Terraform**
```bash
# Scan all Terraform files
checkov -d . --framework terraform

# Scan Terraform modules
checkov -d modules/ --framework terraform

# Scan with AWS-specific checks
checkov -d . --framework terraform --check CKV_AWS_*
```

### **Kubernetes**
```bash
# Scan Kubernetes YAML manifests
checkov -d . --framework kubernetes

# Scan Helm charts
checkov -d helm-chart/ --framework helm

# Scan Kustomize
checkov -d kustomize/ --framework kustomize
```

### **Dockerfile**
```bash
# Scan Dockerfile
checkov -f Dockerfile --framework dockerfile

# Scan all Dockerfiles
checkov -d . --framework dockerfile
```

---

## 🔍 Filtering Results

### **By Severity**
```bash
# Show only critical/high severity
checkov -d . --compact | grep -E "(CRITICAL|HIGH)"

# Count failures by severity
checkov -d . --output json | jq '.results.failed_checks | group_by(.severity) | .[] | {severity: .[0].severity, count: length}'
```

### **By Check ID**
```bash
# Run specific checks only
checkov -d . --check CKV_AZURE_35,CKV_AZURE_41

# Skip specific checks
checkov -d . --skip-check CKV_AZURE_206

# Run all except specific checks
checkov -d . --skip-check CKV_AZURE_*_206
```

### **By Resource Type**
```bash
# Scan only storage accounts
checkov -d . --framework bicep | grep "Microsoft.Storage"

# Scan only Key Vaults
checkov -d . --framework bicep | grep "Microsoft.KeyVault"
```

---

## 📊 Analysis & Reporting

### **Summary Statistics**
```bash
# Show summary only
checkov -d . --compact --quiet

# JSON summary
checkov -d . --output json | jq '.summary'

# Example output:
# {
#   "passed": 68,
#   "failed": 47,
#   "skipped": 0,
#   "parsing_errors": 0,
#   "resource_count": 115
# }
```

### **Failed Checks Details**
```bash
# List all failed checks
checkov -d . --output json | jq '.results.failed_checks[] | {check_id, check_name, file_path}'

# Group failed checks by type
checkov -d . --output json | jq '.results.failed_checks | group_by(.check_id) | .[] | {check_id: .[0].check_id, count: length}'

# Top 10 most common failures
checkov -d . --output json | jq '.results.failed_checks | group_by(.check_id) | sort_by(-length) | .[:10] | .[] | {check: .[0].check_id, count: length}'
```

### **Passed Checks Details**
```bash
# List all passed checks
checkov -d . --output json | jq '.results.passed_checks[] | {check_id, check_name, resource}'

# Count passed checks by file
checkov -d . --output json | jq '.results.passed_checks | group_by(.file_path) | .[] | {file: .[0].file_path, count: length}'
```

---

## 🔧 Suppressing Checks

### **Inline Suppression (Bicep)**
```bicep
// Suppress specific check with reason
#checkov:skip=CKV_AZURE_35:Dev environment - public access acceptable for testing
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: 'devstoragetest'
  properties: {
    // ... configuration
  }
}
```

### **Inline Suppression (Terraform)**
```hcl
# Suppress check with justification
resource "azurerm_storage_account" "example" {
  # checkov:skip=CKV_AZURE_35:Development environment only
  name                     = "devstoragetest"
  resource_group_name      = azurerm_resource_group.example.name
  location                 = azurerm_resource_group.example.location
}
```

### **Configuration File Suppression**
```yaml
# .checkov.yml
skip-check:
  - CKV_AZURE_35  # Storage account public access (dev environment)
  - CKV_AZURE_206 # Storage replication (cost optimization)
```

---

## 🚀 CI/CD Integration

### **GitHub Actions**
```yaml
- name: Run Checkov
  run: |
    pip install checkov
    checkov -d . \
      --framework bicep \
      --output sarif \
      --output-file-path results/

- name: Upload SARIF
  uses: github/codeql-action/upload-sarif@v3
  with:
    sarif_file: results/results_sarif.sarif
```

### **Azure DevOps**
```yaml
- script: |
    pip install checkov
    checkov -d $(Build.SourcesDirectory) \
      --framework bicep \
      --output json > checkov-results.json
  displayName: 'Run Checkov IaC Scan'

- task: PublishBuildArtifacts@1
  inputs:
    pathToPublish: 'checkov-results.json'
    artifactName: 'checkov-results'
```

### **GitLab CI**
```yaml
checkov_scan:
  image: bridgecrew/checkov:latest
  script:
    - checkov -d . --framework bicep --output json > checkov-results.json
  artifacts:
    reports:
      sast: checkov-results.json
```

---

## 🎯 Advanced Usage

### **Baseline (Ignore Existing Issues)**
```bash
# Create baseline (first scan)
checkov -d . --output json > baseline.json

# Scan against baseline (only new issues)
checkov -d . --baseline baseline.json
```

### **Custom Policies**
```python
# custom_checks/check_storage_naming.py
from checkov.common.models.enums import CheckResult, CheckCategories
from checkov.arm.checks.resource.base_resource_check import BaseResourceCheck

class StorageAccountNaming(BaseResourceCheck):
    def __init__(self):
        name = "Ensure storage account follows naming convention"
        id = "CKV_AZURE_CUSTOM_1"
        supported_resources = ['Microsoft.Storage/storageAccounts']
        categories = [CheckCategories.CONVENTION]
        super().__init__(name=name, id=id, categories=categories, 
                         supported_resources=supported_resources)

    def scan_resource_conf(self, conf):
        name = conf.get('name')
        if name and name.startswith('st'):
            return CheckResult.PASSED
        return CheckResult.FAILED

check = StorageAccountNaming()
```

```bash
# Run with custom checks
checkov -d . --external-checks-dir ./custom_checks/
```

### **Policy-as-Code Repository**
```bash
# Clone shared policy repository
git clone https://github.com/your-org/checkov-policies.git

# Scan with external policies
checkov -d . --external-checks-git https://github.com/your-org/checkov-policies.git
```

---

## 📊 Real-World Examples

### **Example 1: Azure Security Journey Scan**
```bash
# Scan all Bicep templates
cd /home/yom/cybersecurity-journey/azure-security-mastery
source ~/.venv/checkov/bin/activate

checkov -d . \
  --framework bicep \
  --compact \
  --quiet

# Output:
# bicep scan results:
# Passed checks: 68, Failed checks: 47, Skipped checks: 0
```

### **Example 2: Scan Single File with Details**
```bash
checkov -f 01-foundations/day01-azure-account-setup/storage-account.bicep \
  --framework bicep

# Output:
# Check: CKV_AZURE_35: "Ensure default network access rule for Storage Accounts is set to deny"
#   FAILED for resource: Microsoft.Storage/storageAccounts.storageAccount
#   File: storage-account.bicep:12-38
#   Guide: https://docs.prismacloud.io/.../set-default-network-access-rule-for-storage-accounts-to-deny
```

### **Example 3: Generate Report for Team**
```bash
# Comprehensive scan with all outputs
checkov -d . \
  --framework bicep \
  --output cli \
  --output json \
  --output sarif \
  --output-file-path scan-results/ \
  --compact

# Results in:
# scan-results/results_cli.txt
# scan-results/results_json.json
# scan-results/results_sarif.sarif
```

### **Example 4: CI/CD Pre-commit Hook**
```bash
# .git/hooks/pre-commit
#!/bin/bash
echo "Running Checkov scan..."
checkov -d . --framework bicep --quiet --compact

if [ $? -ne 0 ]; then
  echo "❌ Checkov scan failed. Please fix security issues before committing."
  exit 1
fi

echo "✅ Checkov scan passed"
```

---

## 🎓 Tips & Best Practices

### **1. Start with Warnings, Not Blocking**
```bash
# Allow failures initially (gather baseline)
checkov -d . --soft-fail

# After baseline established, enforce
checkov -d . --compact || exit 1
```

### **2. Scan Only Changed Files**
```bash
# In CI/CD, scan only PR changes
git diff --name-only origin/main...HEAD | grep '.bicep$' | while read file; do
  checkov -f "$file" --framework bicep
done
```

### **3. Parallel Scanning for Speed**
```bash
# Scan multiple frameworks in parallel
checkov -d . --framework bicep --compact &
checkov -d . --framework terraform --compact &
checkov -d . --framework kubernetes --compact &
wait
```

### **4. Use Checkov in Development**
```bash
# Add to package.json
{
  "scripts": {
    "security:scan": "checkov -d infrastructure/ --framework bicep --compact",
    "security:fix": "checkov -d infrastructure/ --framework bicep --compact --output json > issues.json"
  }
}

# Run during development
npm run security:scan
```

---

## 📚 Resources

- **Checkov Docs:** https://www.checkov.io/
- **Azure Policies:** https://docs.prismacloud.io/en/enterprise-edition/policy-reference/azure-policies
- **GitHub Integration:** https://www.checkov.io/2.Basics/Integrate%20with%20GitHub.html
- **Custom Checks:** https://www.checkov.io/3.Custom%20Policies/Python%20Custom%20Policies.html

---

**Last Updated:** November 2, 2025  
**Day 47:** Infrastructure Security Scanning
