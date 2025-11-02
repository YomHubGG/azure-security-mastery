# Day 47: Infrastructure Security Scanning

**Date:** November 2, 2025  
**Focus:** Policy-as-Code and Infrastructure-as-Code (IaC) Security Validation  
**Tools:** Checkov, tfsec, Azure Policy, Open Policy Agent  
**Cost:** €0.00 (local scanning only)

---

## 🎯 Learning Objectives

By the end of this session, you will:
- ✅ Understand Infrastructure-as-Code (IaC) security scanning principles
- ✅ Use Checkov to scan Bicep/Terraform templates for security issues
- ✅ Implement automated IaC scanning in CI/CD pipelines
- ✅ Identify and remediate common IaC security misconfigurations
- ✅ Apply Policy-as-Code principles for compliance automation
- ✅ Integrate security scanning into development workflow

---

## 📊 Scan Results Summary

### **Your Infrastructure Security Posture**

**Scan Date:** November 2, 2025  
**Tool:** Checkov 3.2.489  
**Scope:** 9 Bicep templates across Azure Security Journey

```
✅ Passed Checks:  68 (59%)
❌ Failed Checks:  47 (41%)
⏭️ Skipped Checks: 0
```

**Security Score:** 59/100 - **Needs Improvement**

---

## 🔍 What is Infrastructure Security Scanning?

### **The Problem**

Traditional security focuses on **runtime** vulnerabilities (running applications). But what about the **infrastructure itself**?

**Example Scenario:**
```bicep
// ❌ Insecure: Storage account allows public access
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: 'mystorageaccount'
  properties: {
    // Missing: networkAcls to deny public access
    // Missing: encryption requirements
    supportsHttpsTrafficOnly: false  // Allows HTTP!
  }
}
```

**Security Issues:**
1. ❌ No network access restrictions (public by default)
2. ❌ HTTP traffic allowed (not HTTPS-only)
3. ❌ No geo-replication (single point of failure)
4. ❌ Missing encryption-at-rest configuration

**Cost of Mistakes:**
- 🔓 Data breach from publicly exposed storage
- 💸 Compliance fines (GDPR: up to €20M)
- 🚨 Security incidents in production
- ⏱️ Emergency hotfixes and incident response

### **The Solution: Shift-Left Security**

**Shift-Left:** Find security issues **before deployment**, not after.

```
Traditional:  Code → Deploy → BREACH! → Fix
Shift-Left:   Code → SCAN → Fix → Deploy ✅
```

**Benefits:**
- 🚀 Faster feedback (seconds vs days)
- 💰 Lower cost (fix in dev vs production)
- 🛡️ Proactive security (prevent vs react)
- 📊 Compliance validation (automated checks)

---

## 🛠️ Tools Overview

### **1. Checkov** ⭐ (Used Today)

**What:** Multi-cloud IaC security scanner (Terraform, Bicep, CloudFormation, Kubernetes)  
**By:** Bridgecrew (acquired by Palo Alto Networks)  
**Open Source:** Yes (Apache 2.0)  
**Languages:** Python

**Features:**
- ✅ 1,000+ built-in policies (CIS benchmarks, GDPR, HIPAA)
- ✅ Multi-cloud support (AWS, Azure, GCP)
- ✅ CI/CD integration (GitHub Actions, GitLab, Jenkins)
- ✅ Fix suggestions (auto-remediation)
- ✅ SARIF output (GitHub Security tab integration)

**Installation:**
```bash
# Virtual environment (recommended)
python3 -m venv ~/.venv/checkov
source ~/.venv/checkov/bin/activate
pip install checkov

# Verify
checkov --version  # 3.2.489
```

### **2. tfsec**

**What:** Terraform-specific static analysis security scanner  
**By:** Aqua Security  
**Open Source:** Yes  
**Language:** Go (faster than Checkov)

**Focus:** Terraform only (HCL syntax), but very fast and accurate.

### **3. Azure Policy**

**What:** Cloud-native governance and compliance service  
**By:** Microsoft  
**Cost:** Free for evaluation, paid for remediation

**Use Case:** Runtime compliance enforcement (prevents non-compliant deployments)

### **4. Open Policy Agent (OPA)**

**What:** General-purpose policy engine (uses Rego language)  
**By:** CNCF (Cloud Native Computing Foundation)  
**Use Case:** Custom policy creation (beyond built-in checks)

---

## 📋 Your Infrastructure Scan Analysis

### **Top 10 Security Issues Found**

#### **1. Missing Secret Expiration Dates** (12 occurrences)
**Check:** `CKV_AZURE_41`  
**Severity:** MEDIUM  
**Files:** `day17-key-vault-secrets/secrets-only.bicep`, `secrets-setup.bicep`

**Issue:**
```bicep
// ❌ FAILED: No expiration date
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  parent: keyVault
  name: 'database-connection-string'
  properties: {
    value: databaseConnectionString
    // Missing: attributes.exp = <timestamp>
  }
}
```

**Why It Matters:**
- 🔑 Secrets should rotate regularly
- ⏰ Expiration forces credential updates
- 🚨 Prevents stale secrets from being exploited

**Fix:**
```bicep
// ✅ PASSED: Expiration date set
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  parent: keyVault
  name: 'database-connection-string'
  properties: {
    value: databaseConnectionString
    attributes: {
      exp: dateTimeToEpoch(dateTimeAdd(utcNow(), 'P90D'))  // 90 days
    }
  }
}
```

**Impact:** 12 secrets without expiration = 12 potential long-term vulnerabilities

---

#### **2. Storage Account Public Network Access** (6 occurrences)
**Check:** `CKV_AZURE_35`  
**Severity:** HIGH  
**Files:** `day01`, `day09`, `day33`

**Issue:**
```bicep
// ❌ FAILED: Default network access = allow (public internet)
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: uniqueString(resourceGroup().id)
  properties: {
    // Missing: networkAcls configuration
  }
}
```

**Attack Scenario:**
1. Attacker scans Azure public IP ranges
2. Finds your storage account endpoint
3. Downloads/uploads files if container permissions are lax
4. Data breach or malware distribution

**Fix:**
```bicep
// ✅ PASSED: Default deny, explicit allow
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: uniqueString(resourceGroup().id)
  properties: {
    networkAcls: {
      defaultAction: 'Deny'        // Block all by default
      bypass: 'AzureServices'      // Allow trusted Azure services
      ipRules: [                   // Whitelist specific IPs
        { value: '203.0.113.0/24' }
      ]
      virtualNetworkRules: []      // Or use VNet integration
    }
  }
}
```

**Impact:** 6 storage accounts potentially exposed to internet

---

#### **3. App Service Not HTTPS-Only** (Multiple checks)
**Checks:** `CKV_AZURE_14`, `CKV_AZURE_70`, `CKV_AZURE_153`  
**Severity:** HIGH  
**Files:** `day17-key-vault-secrets/secrets-setup.bicep`, `day33 capstone`

**Issue:**
```bicep
// ❌ FAILED: HTTP traffic allowed (man-in-the-middle attacks)
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  name: appName
  properties: {
    httpsOnly: false  // Or missing (defaults to false)
  }
}
```

**Attack:** Man-in-the-middle (MITM) intercepts unencrypted traffic

**Fix:**
```bicep
// ✅ PASSED: HTTPS only
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  name: appName
  properties: {
    httpsOnly: true
    siteConfig: {
      minTlsVersion: '1.2'  // Bonus: enforce TLS 1.2+
      http20Enabled: true   // HTTP/2 support
    }
  }
}
```

---

#### **4. No Storage Replication** (6 occurrences)
**Check:** `CKV_AZURE_206`  
**Severity:** MEDIUM  
**Files:** All storage accounts

**Issue:**
```bicep
// ❌ FAILED: No replication = single datacenter failure risk
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  sku: {
    name: 'Standard_LRS'  // Locally Redundant Storage (3 copies, 1 datacenter)
  }
}
```

**Risk:** Single datacenter outage = data loss

**Fix:**
```bicep
// ✅ PASSED: Geo-redundant replication
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  sku: {
    name: 'Standard_GRS'   // Geo-Redundant Storage (6 copies, 2 regions)
    // Or: 'Standard_GZRS'  // Geo-Zone-Redundant (best resilience)
  }
}
```

**Tradeoff:** Cost increases (LRS = cheapest, GZRS = most expensive)

---

#### **5. SSH Access from Internet** (2 occurrences)
**Check:** `CKV_AZURE_10`  
**Severity:** CRITICAL  
**Files:** `day09-network-security`, `day17 VM`

**Issue:**
```bicep
// ❌ FAILED: SSH port 22 open to 0.0.0.0/0 (entire internet)
resource nsg 'Microsoft.Network/networkSecurityGroups@2021-05-01' = {
  properties: {
    securityRules: [
      {
        name: 'allow-ssh'
        properties: {
          priority: 100
          direction: 'Inbound'
          access: 'Allow'
          protocol: 'Tcp'
          sourceAddressPrefix: '*'  // ❌ Entire internet!
          destinationPortRange: '22'
        }
      }
    ]
  }
}
```

**Attack:** Brute force SSH credentials from anywhere in the world

**Fix:**
```bicep
// ✅ PASSED: SSH restricted to specific IP
resource nsg 'Microsoft.Network/networkSecurityGroups@2021-05-01' = {
  properties: {
    securityRules: [
      {
        name: 'allow-ssh-admin-only'
        properties: {
          priority: 100
          direction: 'Inbound'
          access: 'Allow'
          protocol: 'Tcp'
          sourceAddressPrefix: '203.0.113.10'  // Your admin IP
          destinationPortRange: '22'
        }
      }
    ]
  }
}
```

**Best Practice:** Use Azure Bastion (no public SSH ports at all)

---

#### **6. Missing Client Certificate Authentication** (3 occurrences)
**Check:** `CKV_AZURE_17`  
**Severity:** MEDIUM  
**Files:** App Service resources

**Issue:** App Service doesn't require client certificates (mutual TLS)

**Fix:**
```bicep
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  properties: {
    clientCertEnabled: true
    clientCertMode: 'Required'  // Enforce client certificates
  }
}
```

**Use Case:** B2B APIs, internal services requiring mutual authentication

---

#### **7. FTP Deployments Enabled** (3 occurrences)
**Check:** `CKV_AZURE_78`  
**Severity:** MEDIUM  
**Files:** App Service resources

**Issue:** FTP is unencrypted and insecure

**Fix:**
```bicep
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  properties: {
    siteConfig: {
      ftpsState: 'Disabled'  // Or 'FtpsOnly' for FTPS
    }
  }
}
```

---

#### **8. HTTP/2 Not Enabled** (3 occurrences)
**Check:** `CKV_AZURE_18`  
**Severity:** LOW  
**Files:** App Service resources

**Issue:** Using HTTP/1.1 (slower, less efficient)

**Fix:**
```bicep
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  properties: {
    siteConfig: {
      http20Enabled: true  // HTTP/2 support
    }
  }
}
```

---

#### **9. Key Vault Public Network Access** (1 occurrence)
**Check:** `CKV_AZURE_189`  
**Severity:** HIGH  
**File:** `day33 capstone`

**Issue:** Key Vault accessible from internet

**Fix:**
```bicep
resource keyVault 'Microsoft.KeyVault/vaults@2023-02-01' = {
  properties: {
    publicNetworkAccess: 'Disabled'  // Private endpoints only
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
    }
  }
}
```

---

#### **10. Missing Health Checks** (2 occurrences)
**Check:** `CKV_AZURE_213`  
**Severity:** MEDIUM  
**Files:** App Service resources

**Issue:** No health endpoint configured

**Fix:**
```bicep
resource appService 'Microsoft.Web/sites@2022-03-01' = {
  properties: {
    siteConfig: {
      healthCheckPath: '/health'  // Application health endpoint
    }
  }
}
```

---

## 🎓 Practical Hands-On: Scanning Your Infrastructure

### **Step 1: Install Checkov**

```bash
# Create virtual environment
python3 -m venv ~/.venv/checkov
source ~/.venv/checkov/bin/activate

# Install Checkov
pip install checkov

# Verify installation
checkov --version
```

### **Step 2: Scan Single File**

```bash
# Scan Day 1 storage account
checkov -f 01-foundations/day01-azure-account-setup/storage-account.bicep \
  --framework bicep \
  --compact

# Output:
# Passed checks: 4, Failed checks: 3, Skipped checks: 0
```

### **Step 3: Scan Entire Directory**

```bash
# Scan all Bicep files in project
checkov -d . \
  --framework bicep \
  --compact \
  --quiet

# Output:
# bicep scan results:
# Passed checks: 68, Failed checks: 47, Skipped checks: 0
```

### **Step 4: Generate JSON Report**

```bash
# Save results for CI/CD integration
checkov -d . \
  --framework bicep \
  --output json \
  --quiet > checkov-results.json
```

### **Step 5: Generate SARIF for GitHub**

```bash
# SARIF format for GitHub Security tab
checkov -d . \
  --framework bicep \
  --output sarif \
  --quiet > checkov-results.sarif

# Upload to GitHub (in Actions workflow)
# See: .github/workflows/day47-iac-scanning.yml
```

---

## 🔧 Remediation Examples

### **Example 1: Fix Storage Account Security**

**Before (3 failures):**
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: uniqueString(resourceGroup().id)
  location: location
  sku: {
    name: 'Standard_LRS'  // ❌ No replication
  }
  kind: 'StorageV2'
  properties: {
    supportsHttpsTrafficOnly: true
    // ❌ Missing networkAcls
  }
}
```

**After (all passed):**
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: 'st${uniqueString(resourceGroup().id)}'  // ✅ Naming convention
  location: location
  sku: {
    name: 'Standard_GRS'  // ✅ Geo-redundant replication
  }
  kind: 'StorageV2'
  properties: {
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    networkAcls: {
      defaultAction: 'Deny'      // ✅ Deny by default
      bypass: 'AzureServices'
      ipRules: []
      virtualNetworkRules: []
    }
    encryption: {
      services: {
        blob: { enabled: true }
        file: { enabled: true }
      }
      keySource: 'Microsoft.Storage'
    }
  }
}
```

**Fixes Applied:**
1. ✅ Added `st` prefix for naming convention
2. ✅ Changed SKU to `Standard_GRS` (geo-replication)
3. ✅ Added `networkAcls` with `defaultAction: 'Deny'`
4. ✅ Enforced TLS 1.2 minimum version

---

### **Example 2: Fix Key Vault Secrets**

**Before (2 failures per secret):**
```bicep
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  parent: keyVault
  name: 'database-connection-string'
  properties: {
    value: databaseConnectionString
    // ❌ Missing content_type
    // ❌ Missing expiration date
  }
}
```

**After (all passed):**
```bicep
resource databaseSecret 'Microsoft.KeyVault/vaults/secrets@2023-02-01' = {
  parent: keyVault
  name: 'database-connection-string'
  properties: {
    value: databaseConnectionString
    contentType: 'text/plain'  // ✅ Content type specified
    attributes: {
      enabled: true
      exp: dateTimeToEpoch(dateTimeAdd(utcNow(), 'P90D'))  // ✅ 90-day expiry
      nbf: dateTimeToEpoch(utcNow())
    }
  }
}
```

**Fixes Applied:**
1. ✅ Added `contentType` metadata
2. ✅ Set expiration date (90 days from deployment)
3. ✅ Added `nbf` (not-before) timestamp

---

## 🚀 CI/CD Integration

### **GitHub Actions Workflow**

Create `.github/workflows/day47-iac-scanning.yml`:

```yaml
name: Infrastructure Security Scanning

on:
  push:
    paths:
      - '**/*.bicep'
      - '**/*.tf'
  pull_request:
    paths:
      - '**/*.bicep'
      - '**/*.tf'

jobs:
  checkov-scan:
    runs-on: ubuntu-latest
    name: Checkov IaC Security Scan
    
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
      
      - name: Set up Python
        uses: actions/setup-python@v5
        with:
          python-version: '3.11'
      
      - name: Install Checkov
        run: pip install checkov
      
      - name: Run Checkov scan
        id: checkov
        run: |
          checkov -d . \
            --framework bicep \
            --output cli \
            --output sarif \
            --output-file-path results/ \
            --quiet || true
      
      - name: Upload SARIF results
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: results/results_sarif.sarif
          category: infrastructure-security
      
      - name: Security Summary
        run: |
          echo "## 🔍 Infrastructure Security Scan Results" >> $GITHUB_STEP_SUMMARY
          echo "" >> $GITHUB_STEP_SUMMARY
          cat results/results_cli.txt >> $GITHUB_STEP_SUMMARY
```

**What This Does:**
1. ✅ Triggers on Bicep/Terraform file changes
2. ✅ Scans infrastructure code with Checkov
3. ✅ Uploads results to GitHub Security tab (SARIF)
4. ✅ Shows summary in PR comments

---

## 📊 Policy-as-Code Concepts

### **What is Policy-as-Code?**

**Definition:** Security and compliance rules written as code (automated enforcement)

**Traditional Approach:**
```
Developer → Submit IaC → Security Review (manual) → Feedback → Fix → Repeat
Timeline: Days to weeks
```

**Policy-as-Code Approach:**
```
Developer → Commit IaC → Automated Scan → Pass/Fail → Self-service Fix
Timeline: Seconds to minutes
```

### **Benefits:**
- ⚡ Instant feedback (no waiting for security team)
- 🔄 Consistent enforcement (no human error)
- 📊 Audit trail (every scan logged)
- 🚀 Developer productivity (self-service fixes)

### **Policy Examples:**

**1. Azure CIS Benchmark**
```
- All storage accounts must use HTTPS only
- All Key Vault secrets must have expiration dates
- All NSGs must deny SSH from internet
```

**2. GDPR Compliance**
```
- All data at rest must be encrypted
- All network traffic must be encrypted (TLS 1.2+)
- All access must be logged and auditable
```

**3. Cost Optimization**
```
- Storage accounts must use LRS (not GRS) in dev/test
- VMs must be tagged with cost center
- App Service Plans must be Basic tier or lower in dev/test
```

---

## 🎯 Interview Questions & Answers

### **Q1: What is Infrastructure-as-Code security scanning?**

**Answer:**
"IaC security scanning is the practice of analyzing infrastructure templates (Bicep, Terraform, CloudFormation) for security misconfigurations **before deployment**. Tools like Checkov scan your code against thousands of security policies (CIS benchmarks, GDPR, HIPAA) and identify issues like:
- Public network access when it should be private
- Missing encryption configurations
- Weak network security rules
- Expired or missing secrets rotation

The key benefit is **shift-left security** - finding issues in development (seconds) rather than production (days/weeks). In my journey, I scanned 9 Bicep templates with Checkov and found 47 security issues, including storage accounts exposed to the internet and Key Vault secrets without expiration dates."

---

### **Q2: Why use Checkov instead of manual security reviews?**

**Answer:**
"Manual reviews are:
1. **Slow:** Takes days for security team to review IaC
2. **Inconsistent:** Human error, missed checks
3. **Not scalable:** Can't review every commit

Checkov provides:
1. **Speed:** Scans entire codebase in seconds
2. **Consistency:** Same 1,000+ policies every time
3. **Scalability:** Runs on every commit automatically
4. **Developer empowerment:** Self-service fixes with actionable guidance

Real example: My Day 1 storage account had 3 security failures. Checkov identified them instantly with fix recommendations. Manual review would have taken hours or days."

---

### **Q3: How do you integrate IaC scanning into CI/CD?**

**Answer:**
"I integrate Checkov into GitHub Actions with this workflow:
1. **Trigger:** On push or PR with Bicep/Terraform changes
2. **Scan:** Run `checkov -d . --framework bicep`
3. **Report:** Generate SARIF output for GitHub Security tab
4. **Enforce:** Optionally fail the pipeline if critical issues found

Key features:
- **SARIF format:** Results appear in GitHub Security tab alongside CodeQL
- **PR comments:** Automated feedback on security issues
- **Baseline:** Track security debt and improvements over time

This is exactly what I implemented in Day 45 with OIDC authentication - multi-layer security scanning including infrastructure validation."

---

### **Q4: What's the difference between Checkov and Azure Policy?**

**Answer:**
| Feature | Checkov | Azure Policy |
|---------|---------|--------------|
| **When** | Before deployment (dev) | During deployment (runtime) |
| **Scope** | IaC templates (code) | Deployed resources (cloud) |
| **Feedback** | Instant (seconds) | Deployment-time (minutes) |
| **Cost** | Free (open source) | Free evaluation, paid remediation |
| **Use Case** | Developer shift-left | Governance enforcement |

**Best Practice:** Use both!
- Checkov: Catch issues early in development
- Azure Policy: Enforce compliance in production

Example: Checkov finds storage account without network restrictions in code. Azure Policy prevents deployment if it reaches production."

---

### **Q5: How do you prioritize Checkov findings?**

**Answer:**
"I use a risk-based approach:

**1. CRITICAL (fix immediately):**
- Public internet access to sensitive resources
- SSH/RDP from 0.0.0.0/0
- Missing encryption for data at rest

**2. HIGH (fix before deployment):**
- HTTPS not enforced
- Missing network restrictions
- Key Vault public access

**3. MEDIUM (fix in sprint):**
- Missing secret expiration dates
- No geo-replication
- Missing health checks

**4. LOW (technical debt):**
- HTTP/2 not enabled
- Naming convention violations

In my scan: 47 failures found. I would prioritize:
1. SSH from internet (CKV_AZURE_10) - immediate
2. Storage public access (CKV_AZURE_35) - immediate
3. Secret expiration (CKV_AZURE_41) - same sprint
4. HTTP/2 support (CKV_AZURE_18) - backlog"

---

### **Q6: Can you give an example of a false positive from Checkov?**

**Answer:**
"Yes, common false positives include:

**Example:** `CKV_AZURE_206` (storage replication)

Checkov flags `Standard_LRS` (Locally Redundant Storage) as a security issue because it only replicates within one datacenter.

**False Positive Scenarios:**
1. **Dev/test environments:** LRS is cost-effective and acceptable risk
2. **Ephemeral data:** Temporary storage doesn't need geo-redundancy
3. **Budget constraints:** GRS costs 2-3x more than LRS

**How to Handle:**
```bicep
# Suppress specific check with comment
#checkov:skip=CKV_AZURE_206:Dev environment - LRS acceptable for cost savings
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  sku: { name: 'Standard_LRS' }
}
```

**Best Practice:** Document WHY you're suppressing a check (compliance audit trail)."

---

## 🎓 Key Takeaways

1. **Shift-Left Security:** Find issues in development, not production
2. **Automated Scanning:** Checkov scans 1,000+ policies in seconds
3. **CI/CD Integration:** GitHub Actions + Checkov = continuous security
4. **Fix Prioritization:** CRITICAL → HIGH → MEDIUM → LOW
5. **Policy-as-Code:** Compliance automation with consistent enforcement
6. **Cost Awareness:** Balance security vs cost (LRS vs GRS, free tier limits)

---

## 📚 Resources

### **Documentation**
- [Checkov Documentation](https://www.checkov.io/1.Welcome/What%20is%20Checkov.html)
- [Azure Policy Definitions](https://docs.microsoft.com/azure/governance/policy/samples/)
- [CIS Azure Benchmark](https://www.cisecurity.org/benchmark/azure)
- [Bicep Security Best Practices](https://learn.microsoft.com/azure/azure-resource-manager/bicep/best-practices)

### **Tools**
- [Checkov GitHub](https://github.com/bridgecrewio/checkov)
- [tfsec](https://github.com/aquasecurity/tfsec)
- [Terrascan](https://github.com/tenable/terrascan)
- [Open Policy Agent](https://www.openpolicyagent.org/)

### **Policies**
- [Azure Security Baseline](https://docs.microsoft.com/security/benchmark/azure/)
- [NIST Cybersecurity Framework](https://www.nist.gov/cyberframework)
- [OWASP Top 10](https://owasp.org/www-project-top-ten/)

---

## 🎯 Next Steps

**Day 49: Supply Chain Security**
- Software Bill of Materials (SBOM) generation
- Signed commits and verified builds (GPG)
- Dependency vulnerability tracking
- Container provenance and attestation

**Continue DevSecOps Journey:**
- Day 51: GitHub Actions Advanced Patterns
- Day 53: SAST Integration (SonarCloud)
- Day 55: DAST & Dynamic Scanning

---

**Session Complete!** ✅  
**Cost:** €0.00 maintained  
**Security Issues Found:** 47  
**Knowledge Gained:** Infrastructure security scanning mastery
