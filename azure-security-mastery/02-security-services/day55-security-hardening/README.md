# Day 55: Security Hardening & Posture Management

**Session #28** | November 10, 2025 | Security Optimization & Remediation

## 🎯 Learning Objectives

By the end of this session, you will:

1. **Implement Microsoft Defender recommendations** (free tier optimization)
2. **Fix remaining Checkov IaC security findings** (target: 90%+ score)
3. **Harden Azure resources** using CIS benchmarks and best practices
4. **Create automated hardening scripts** for continuous compliance
5. **Document security improvements** with before/after metrics

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Part 1: Security Posture Assessment](#part-1-security-posture-assessment)
- [Part 2: Defender Recommendations](#part-2-defender-recommendations)
- [Part 3: Checkov Remediation](#part-3-checkov-remediation)
- [Part 4: OS-Level Hardening](#part-4-os-level-hardening)
- [Part 5: Network Hardening](#part-5-network-hardening)
- [Part 6: Automation & Continuous Compliance](#part-6-automation--continuous-compliance)
- [Labs](#labs)
- [Real-World Context](#real-world-context)
- [Interview Preparation](#interview-preparation)

---

## Prerequisites

- ✅ Completed Day 47 (IaC Security Scanning with Checkov)
- ✅ Azure subscription with Defender for Cloud free tier enabled
- ✅ Existing Bicep templates from previous days
- ✅ Azure CLI configured and authenticated

**Cost**: €0.00 (using free tier only)

---

## Part 1: Security Posture Assessment

### 1.1 Understanding Security Hardening

**Security Hardening** = Reducing attack surface by:
- Disabling unnecessary services/ports
- Enforcing strong authentication
- Applying least privilege principles
- Patching vulnerabilities
- Implementing defense-in-depth

**The Hardening Pyramid**:
```
        ┌─────────────────┐
        │  Application    │ ← SAST, DAST, dependency scanning
        ├─────────────────┤
        │  Configuration  │ ← IaC scanning, policy enforcement
        ├─────────────────┤
        │  Network        │ ← NSGs, firewalls, zero trust
        ├─────────────────┤
        │  Identity       │ ← MFA, RBAC, Conditional Access
        ├─────────────────┤
        │  Infrastructure │ ← Patching, encryption, compliance
        └─────────────────┘
```

### 1.2 Current Security Posture (Baseline)

From **Day 47** (Nov 2, 2025), your Checkov results:
```
✅ Passed checks: 43 (73% pass rate)
❌ Failed checks: 16 (27% failure rate)
⚠️  Total issues: 31 findings

Top categories:
- Encryption: 8 issues (HTTPS enforcement, TLS 1.2+)
- Logging: 6 issues (diagnostic settings missing)
- Access Control: 5 issues (public access, RBAC gaps)
- Networking: 4 issues (NSG rules too permissive)
- Resource Management: 8 issues (tags, lifecycle policies)
```

**Goal**: Improve from **73% → 90%+** pass rate (fix 10-12 critical issues).

### 1.3 CIS Azure Foundations Benchmark

**CIS (Center for Internet Security)** = Industry-standard security baselines.

**Key CIS Azure Recommendations**:
1. **Identity & Access Management** (1.x)
   - 1.1: MFA for all users
   - 1.2: No guest users with admin roles
   - 1.23: Service principals with certificates (not passwords)

2. **Microsoft Defender for Cloud** (2.x)
   - 2.1: Enable Defender for all resource types
   - 2.2: Auto-provisioning enabled for Log Analytics agent
   - 2.15: Email notifications for high-severity alerts

3. **Storage Accounts** (3.x)
   - 3.1: Require secure transfer (HTTPS only)
   - 3.7: Public network access disabled
   - 3.8: Trusted Microsoft services allowed

4. **Database Services** (4.x)
   - 4.1.1: Auditing enabled on SQL servers
   - 4.3.1: Enforce SSL connections

5. **Logging & Monitoring** (5.x)
   - 5.1.1: Diagnostic settings configured for all resources
   - 5.1.5: Activity log retention ≥ 365 days

6. **Networking** (6.x)
   - 6.1: RDP/SSH access from internet blocked
   - 6.5: Network security groups associated with subnets

7. **Virtual Machines** (7.x)
   - 7.1: OS and data disks encrypted
   - 7.4: Latest OS patches installed

**Tool**: `az security assessment list` (Azure Security Center API)

---

## Part 2: Defender Recommendations

### 2.1 Accessing Defender Recommendations (Free Tier)

**Microsoft Defender for Cloud Free Tier** provides:
- ✅ Security posture assessment (Secure Score)
- ✅ Recommendations for Azure resources
- ✅ Regulatory compliance dashboard (limited)
- ❌ Threat protection (paid: €13/server/month)
- ❌ Just-in-time VM access (paid)

**Access via Azure CLI**:
```bash
# List all security recommendations
az security assessment list --output table

# Get specific recommendation details
az security assessment show \
  --name <assessment-id> \
  --output json

# Get Secure Score
az security secure-score-controls list \
  --output table
```

### 2.2 Common Free-Tier Recommendations

**1. Storage Account Secure Transfer**
```
Recommendation: Enable secure transfer required
Severity: High
Impact: Prevents unencrypted HTTP connections

Fix (Bicep):
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    supportsHttpsTrafficOnly: true  // ← Add this
  }
}
```

**2. Key Vault Soft Delete**
```
Recommendation: Enable soft delete on Key Vault
Severity: Medium
Impact: Protects against accidental deletion

Fix (Bicep):
resource keyVault 'Microsoft.KeyVault/vaults@2023-07-01' = {
  properties: {
    enableSoftDelete: true          // ← Add this
    softDeleteRetentionInDays: 90   // ← Add this
  }
}
```

**3. Diagnostic Settings Missing**
```
Recommendation: Enable diagnostic logs
Severity: Low
Impact: Required for incident investigation

Fix (Bicep):
resource diagnosticSettings 'Microsoft.Insights/diagnosticSettings@2021-05-01-preview' = {
  name: 'storage-diagnostics'
  scope: storageAccount
  properties: {
    logs: [
      {
        category: 'StorageRead'
        enabled: true
        retentionPolicy: {
          enabled: true
          days: 90
        }
      }
    ]
  }
}
```

**4. Public Network Access**
```
Recommendation: Disable public access to storage accounts
Severity: High
Impact: Prevents internet-facing data exposure

Fix (Bicep):
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    publicNetworkAccess: 'Disabled'  // ← Add this
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'
    }
  }
}
```

### 2.3 Prioritization Matrix

| Priority | Severity | Effort | Impact | Fix Order |
|----------|----------|--------|--------|-----------|
| **P0** | High | Low | High | Fix immediately (HTTPS, encryption) |
| **P1** | High | Medium | High | Fix this sprint (soft delete, logging) |
| **P2** | Medium | Low | Medium | Fix next sprint (tags, retention) |
| **P3** | Low | High | Low | Backlog (advanced configs) |

**Day 55 Goal**: Focus on **P0** (quick wins, high impact).

---

## Part 3: Checkov Remediation

### 3.1 Your Day 47 Checkov Findings (Prioritized)

**Category 1: Encryption (8 issues) - P0**

1. **CKV_AZURE_3**: Storage account encryption with customer-managed keys
```bicep
// BEFORE (failed check)
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    encryption: {
      services: {
        blob: { enabled: true }
      }
    }
  }
}

// AFTER (passes check)
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    encryption: {
      services: {
        blob: { enabled: true }
        file: { enabled: true }    // ← Add file encryption
        queue: { enabled: true }   // ← Add queue encryption
        table: { enabled: true }   // ← Add table encryption
      }
      keySource: 'Microsoft.Storage'  // or 'Microsoft.Keyvault' for CMK
    }
  }
}
```

2. **CKV_AZURE_33**: HTTPS only for storage accounts
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    supportsHttpsTrafficOnly: true  // ← Must be true
    minimumTlsVersion: 'TLS1_2'     // ← Add TLS 1.2 minimum
  }
}
```

**Category 2: Logging (6 issues) - P1**

3. **CKV_AZURE_35**: Diagnostic settings for Key Vault
```bicep
resource keyVaultDiagnostics 'Microsoft.Insights/diagnosticSettings@2021-05-01-preview' = {
  name: 'keyvault-diagnostics'
  scope: keyVault
  properties: {
    logs: [
      {
        category: 'AuditEvent'
        enabled: true
        retentionPolicy: {
          enabled: true
          days: 90  // ← 90 days minimum for compliance
        }
      }
    ]
    metrics: [
      {
        category: 'AllMetrics'
        enabled: true
      }
    ]
  }
}
```

**Category 3: Access Control (5 issues) - P0**

4. **CKV_AZURE_36**: Storage account public access disabled
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  properties: {
    allowBlobPublicAccess: false    // ← Disable public blob access
    publicNetworkAccess: 'Disabled' // ← Disable public network access
    networkAcls: {
      defaultAction: 'Deny'
      bypass: 'AzureServices'        // ← Allow trusted Azure services
    }
  }
}
```

**Category 4: Networking (4 issues) - P1**

5. **CKV_AZURE_12**: NSG rules too permissive
```bicep
// BEFORE (failed - allows 0.0.0.0/0)
resource nsgRule 'Microsoft.Network/networkSecurityGroups/securityRules@2023-05-01' = {
  properties: {
    sourceAddressPrefix: '*'  // ❌ Too broad
    destinationPortRange: '22'
    access: 'Allow'
  }
}

// AFTER (passes - specific IP ranges)
resource nsgRule 'Microsoft.Network/networkSecurityGroups/securityRules@2023-05-01' = {
  properties: {
    sourceAddressPrefix: '10.0.0.0/24'  // ✅ Specific subnet
    destinationPortRange: '22'
    access: 'Allow'
  }
}
```

**Category 5: Resource Management (8 issues) - P2**

6. **CKV_AZURE_11**: Missing resource tags
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  tags: {
    Environment: 'Production'
    Owner: 'SecurityTeam'
    CostCenter: 'IT-Security'
    Compliance: 'PCI-DSS'
    DataClassification: 'Confidential'
  }
}
```

### 3.2 Automated Remediation Script

Create `checkov-fix.sh` to apply fixes across all templates:

```bash
#!/usr/bin/env bash
set -euo pipefail

# Find all Bicep files
BICEP_FILES=$(find . -name "*.bicep" -type f)

echo "🔍 Found $(echo "$BICEP_FILES" | wc -l) Bicep files"

# Run Checkov before fixes
echo "📊 Running baseline scan..."
checkov -d . --framework bicep --compact --output json > before.json

# Apply automated fixes (example: HTTPS enforcement)
for file in $BICEP_FILES; do
  # Fix HTTPS enforcement
  sed -i '/supportsHttpsTrafficOnly/!s/properties: {/properties: {\n    supportsHttpsTrafficOnly: true/' "$file"
  
  # Fix TLS version
  sed -i '/minimumTlsVersion/!s/supportsHttpsTrafficOnly: true/supportsHttpsTrafficOnly: true\n    minimumTlsVersion: '\''TLS1_2'\''/' "$file"
done

# Run Checkov after fixes
echo "📊 Running post-fix scan..."
checkov -d . --framework bicep --compact --output json > after.json

# Compare results
BEFORE_PASSED=$(jq '.summary.passed' before.json)
AFTER_PASSED=$(jq '.summary.passed' after.json)
IMPROVEMENT=$((AFTER_PASSED - BEFORE_PASSED))

echo "✅ Improvements: +$IMPROVEMENT passed checks"
```

---

## Part 4: OS-Level Hardening

### 4.1 Linux Hardening (ParrotOS)

**1. Unattended Security Updates**
```bash
# Install automatic security updates
sudo apt-get install unattended-upgrades -y

# Configure to auto-install security updates only
sudo nano /etc/apt/apt.conf.d/50unattended-upgrades

# Add:
Unattended-Upgrade::Allowed-Origins {
    "${distro_id}:${distro_codename}-security";
};
Unattended-Upgrade::Automatic-Reboot "false";
```

**2. Firewall Configuration (UFW)**
```bash
# Enable UFW firewall
sudo ufw enable

# Default deny incoming, allow outgoing
sudo ufw default deny incoming
sudo ufw default allow outgoing

# Allow specific services only
sudo ufw allow 22/tcp   # SSH
sudo ufw allow 443/tcp  # HTTPS
sudo ufw status verbose
```

**3. Fail2Ban (Brute Force Protection)**
```bash
# Install Fail2Ban
sudo apt-get install fail2ban -y

# Configure SSH protection
sudo cp /etc/fail2ban/jail.conf /etc/fail2ban/jail.local
sudo nano /etc/fail2ban/jail.local

# Add:
[sshd]
enabled = true
port = 22
maxretry = 3
bantime = 3600  # 1 hour ban after 3 failed attempts
```

**4. SSH Hardening**
```bash
# Edit SSH config
sudo nano /etc/ssh/sshd_config

# Recommended settings:
PermitRootLogin no               # Disable root login
PasswordAuthentication no         # SSH keys only
PubkeyAuthentication yes
X11Forwarding no                  # Disable X11
MaxAuthTries 3
ClientAliveInterval 300           # 5-minute timeout
ClientAliveCountMax 2

# Restart SSH
sudo systemctl restart sshd
```

**5. AppArmor (Mandatory Access Control)**
```bash
# Check AppArmor status
sudo aa-status

# Enforce AppArmor profiles
sudo aa-enforce /etc/apparmor.d/*

# Common profiles:
# - /usr/sbin/sshd (SSH daemon protection)
# - /usr/bin/docker (Container runtime isolation)
```

### 4.2 Security Audit Tools

**1. Lynis (System Hardening Scanner)**
```bash
# Install Lynis
sudo apt-get install lynis -y

# Run system audit
sudo lynis audit system

# Output:
# - Hardening index: 68/100 (baseline)
# - Suggestions: 47 items
# - Warnings: 12 critical findings

# Example output:
# [WARNING] SSH configuration: PermitRootLogin=yes
# [SUGGESTION] Enable automatic security updates
# [HARDENING] Install fail2ban for brute-force protection
```

**2. OpenSCAP (Compliance Scanning)**
```bash
# Install OpenSCAP
sudo apt-get install libopenscap8 -y

# Download CIS benchmark
wget https://downloads.cisecurity.org/ubuntu/cis-ubuntu-benchmark.xml

# Run compliance scan
oscap xccdf eval --profile xccdf_org.cisecurity_profile_Level_1_-_Server cis-ubuntu-benchmark.xml

# Generate HTML report
oscap xccdf eval --profile ... --results results.xml --report report.html cis-ubuntu-benchmark.xml
```

---

## Part 5: Network Hardening

### 5.1 Azure Network Security Groups (NSGs)

**Best Practice NSG Rules**:

```bicep
resource nsg 'Microsoft.Network/networkSecurityGroups@2023-05-01' = {
  name: 'nsg-hardened'
  location: location
  properties: {
    securityRules: [
      {
        name: 'DenyAllInbound'
        properties: {
          priority: 4096
          direction: 'Inbound'
          access: 'Deny'
          protocol: '*'
          sourceAddressPrefix: '*'
          sourcePortRange: '*'
          destinationAddressPrefix: '*'
          destinationPortRange: '*'
        }
      }
      {
        name: 'AllowHTTPSInbound'
        properties: {
          priority: 100
          direction: 'Inbound'
          access: 'Allow'
          protocol: 'Tcp'
          sourceAddressPrefix: 'Internet'
          sourcePortRange: '*'
          destinationAddressPrefix: 'VirtualNetwork'
          destinationPortRange: '443'
        }
      }
      {
        name: 'DenyRDPFromInternet'
        properties: {
          priority: 200
          direction: 'Inbound'
          access: 'Deny'
          protocol: 'Tcp'
          sourceAddressPrefix: 'Internet'
          sourcePortRange: '*'
          destinationAddressPrefix: '*'
          destinationPortRange: '3389'
        }
      }
      {
        name: 'DenySSHFromInternet'
        properties: {
          priority: 201
          direction: 'Inbound'
          access: 'Deny'
          protocol: 'Tcp'
          sourceAddressPrefix: 'Internet'
          sourcePortRange: '*'
          destinationAddressPrefix: '*'
          destinationPortRange: '22'
        }
      }
    ]
  }
}
```

**NSG Flow Logs** (for auditing):
```bicep
resource nsgFlowLog 'Microsoft.Network/networkWatchers/flowLogs@2023-05-01' = {
  name: 'nsg-flow-logs'
  location: location
  properties: {
    targetResourceId: nsg.id
    storageId: storageAccount.id
    enabled: true
    retentionPolicy: {
      days: 90
      enabled: true
    }
    format: {
      type: 'JSON'
      version: 2
    }
  }
}
```

### 5.2 Azure Firewall (Comparison)

| Feature | NSG | Azure Firewall | Cost |
|---------|-----|----------------|------|
| **Layer** | Layer 4 (IP/port) | Layer 7 (application) | NSG: €0, Firewall: €1.25/hour |
| **Filtering** | IP, port, protocol | FQDN, URL, TLS inspection | |
| **Stateful** | Yes | Yes | |
| **Threat Intel** | No | Yes (Microsoft feed) | |
| **Logging** | Flow logs | Detailed logs + alerts | |
| **Use Case** | Basic subnet protection | Enterprise perimeter | |

**For Day 55**: Use NSGs (€0 cost, sufficient for learning).

---

## Part 6: Automation & Continuous Compliance

### 6.1 Automated Hardening Script

Create `azure-hardening-audit.sh`:

```bash
#!/usr/bin/env bash
set -euo pipefail

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "🔒 Azure Security Hardening Audit"
echo "=================================="
echo ""

# 1. Check Storage Accounts for HTTPS enforcement
echo "📦 Checking Storage Accounts..."
STORAGE_ACCOUNTS=$(az storage account list --query "[].{Name:name, HTTPS:supportsHttpsTrafficOnly, TLS:minimumTlsVersion}" -o json)

STORAGE_COUNT=$(echo "$STORAGE_ACCOUNTS" | jq 'length')
HTTPS_ENABLED=$(echo "$STORAGE_ACCOUNTS" | jq '[.[] | select(.HTTPS==true)] | length')
TLS12_ENABLED=$(echo "$STORAGE_ACCOUNTS" | jq '[.[] | select(.TLS=="TLS1_2")] | length')

if [ "$HTTPS_ENABLED" -eq "$STORAGE_COUNT" ]; then
  echo -e "  ${GREEN}✅ HTTPS enforcement: $HTTPS_ENABLED/$STORAGE_COUNT${NC}"
else
  echo -e "  ${RED}❌ HTTPS enforcement: $HTTPS_ENABLED/$STORAGE_COUNT${NC}"
fi

if [ "$TLS12_ENABLED" -eq "$STORAGE_COUNT" ]; then
  echo -e "  ${GREEN}✅ TLS 1.2 minimum: $TLS12_ENABLED/$STORAGE_COUNT${NC}"
else
  echo -e "  ${YELLOW}⚠️  TLS 1.2 minimum: $TLS12_ENABLED/$STORAGE_COUNT${NC}"
fi

# 2. Check Key Vaults for soft delete
echo ""
echo "🔑 Checking Key Vaults..."
KEY_VAULTS=$(az keyvault list --query "[].{Name:name, SoftDelete:properties.enableSoftDelete, PurgeProtection:properties.enablePurgeProtection}" -o json)

KV_COUNT=$(echo "$KEY_VAULTS" | jq 'length')
SOFT_DELETE=$(echo "$KEY_VAULTS" | jq '[.[] | select(.SoftDelete==true)] | length')
PURGE_PROTECT=$(echo "$KEY_VAULTS" | jq '[.[] | select(.PurgeProtection==true)] | length')

if [ "$SOFT_DELETE" -eq "$KV_COUNT" ]; then
  echo -e "  ${GREEN}✅ Soft delete: $SOFT_DELETE/$KV_COUNT${NC}"
else
  echo -e "  ${RED}❌ Soft delete: $SOFT_DELETE/$KV_COUNT${NC}"
fi

if [ "$PURGE_PROTECT" -eq "$KV_COUNT" ]; then
  echo -e "  ${GREEN}✅ Purge protection: $PURGE_PROTECT/$KV_COUNT${NC}"
else
  echo -e "  ${YELLOW}⚠️  Purge protection: $PURGE_PROTECT/$KV_COUNT${NC}"
fi

# 3. Check public network access
echo ""
echo "🌐 Checking Public Network Access..."
PUBLIC_STORAGE=$(az storage account list --query "[?publicNetworkAccess=='Enabled'].name" -o tsv | wc -l)

if [ "$PUBLIC_STORAGE" -eq 0 ]; then
  echo -e "  ${GREEN}✅ No storage accounts with public access${NC}"
else
  echo -e "  ${YELLOW}⚠️  $PUBLIC_STORAGE storage accounts have public access${NC}"
fi

# 4. Secure Score
echo ""
echo "📊 Microsoft Defender Secure Score..."
SECURE_SCORE=$(az security secure-score list --query "[0].properties.score.percentage" -o tsv 2>/dev/null || echo "N/A")

if [ "$SECURE_SCORE" != "N/A" ]; then
  SCORE=$(echo "$SECURE_SCORE" | awk '{printf "%.0f", $1}')
  
  if [ "$SCORE" -ge 90 ]; then
    echo -e "  ${GREEN}✅ Secure Score: ${SCORE}%${NC}"
  elif [ "$SCORE" -ge 70 ]; then
    echo -e "  ${YELLOW}⚠️  Secure Score: ${SCORE}%${NC}"
  else
    echo -e "  ${RED}❌ Secure Score: ${SCORE}%${NC}"
  fi
else
  echo -e "  ${YELLOW}⚠️  Secure Score: Not available (enable Defender)${NC}"
fi

# 5. Summary
echo ""
echo "=================================="
TOTAL_CHECKS=5
PASSED_CHECKS=0

[ "$HTTPS_ENABLED" -eq "$STORAGE_COUNT" ] && ((PASSED_CHECKS++))
[ "$TLS12_ENABLED" -eq "$STORAGE_COUNT" ] && ((PASSED_CHECKS++))
[ "$SOFT_DELETE" -eq "$KV_COUNT" ] && ((PASSED_CHECKS++))
[ "$PUBLIC_STORAGE" -eq 0 ] && ((PASSED_CHECKS++))
[ "$SECURE_SCORE" != "N/A" ] && [ "$SCORE" -ge 90 ] && ((PASSED_CHECKS++))

PASS_RATE=$((PASSED_CHECKS * 100 / TOTAL_CHECKS))

echo "✅ Passed: $PASSED_CHECKS/$TOTAL_CHECKS ($PASS_RATE%)"

if [ "$PASS_RATE" -ge 90 ]; then
  echo -e "${GREEN}🎉 Excellent security posture!${NC}"
  exit 0
elif [ "$PASS_RATE" -ge 70 ]; then
  echo -e "${YELLOW}⚠️  Good, but improvements needed${NC}"
  exit 1
else
  echo -e "${RED}❌ Critical hardening required${NC}"
  exit 2
fi
```

### 6.2 GitHub Actions Hardening Workflow

Create `.github/workflows/security-hardening.yml`:

```yaml
name: Security Hardening Check

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * 1'  # Weekly on Monday 2 AM

jobs:
  checkov:
    name: IaC Security Scan
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Run Checkov
        uses: bridgecrewio/checkov-action@master
        with:
          directory: .
          framework: bicep
          output_format: cli
          soft_fail: false  # Fail build on security issues
          
  lynis:
    name: System Hardening Audit
    runs-on: ubuntu-latest
    steps:
      - name: Install Lynis
        run: |
          sudo apt-get update
          sudo apt-get install -y lynis
          
      - name: Run Lynis Audit
        run: |
          sudo lynis audit system --quick --quiet
          
      - name: Check Hardening Index
        run: |
          HARDENING=$(sudo lynis show details | grep "Hardening index" | awk '{print $4}')
          echo "Hardening index: $HARDENING"
          [ "$HARDENING" -ge 70 ] && echo "✅ Passed" || exit 1
```

---

## Labs

### Lab 1: Run Security Posture Assessment

**Objective**: Baseline your current security posture.

**Steps**:
1. Run Defender assessment:
```bash
az security assessment list --output table
```

2. Run Checkov scan:
```bash
checkov -d azure-security-mastery/ --framework bicep --compact
```

3. Document results in `LAB-RESULTS.md`:
   - Current pass rate: X%
   - Critical findings: Y
   - Target: 90%+

### Lab 2: Fix Top 5 Checkov Issues

**Objective**: Apply automated fixes to improve security score.

**Steps**:
1. Identify top 5 issues from Checkov output
2. Update Bicep templates with fixes
3. Re-run Checkov to verify improvements
4. Calculate improvement percentage

**Expected Outcome**: +10-15% improvement in pass rate.

### Lab 3: Implement Storage Account Hardening

**Objective**: Apply CIS benchmarks to storage accounts.

**Steps**:
1. Create hardened storage account template:
```bash
cd azure-security-mastery/02-security-services/day55-security-hardening/
nano storage-hardened.bicep
```

2. Deploy template:
```bash
az deployment group create \
  --resource-group rg-security-hardening \
  --template-file storage-hardened.bicep
```

3. Verify hardening with audit script:
```bash
./azure-hardening-audit.sh
```

### Lab 4: OS Hardening (Local)

**Objective**: Harden ParrotOS with security tools.

**Steps**:
1. Run Lynis audit:
```bash
sudo lynis audit system
```

2. Apply top 5 recommendations:
   - Enable automatic updates
   - Configure UFW firewall
   - Install Fail2Ban
   - Harden SSH config
   - Enable AppArmor profiles

3. Re-run Lynis and document improvement.

### Lab 5: Create Continuous Compliance Script

**Objective**: Automate security posture monitoring.

**Steps**:
1. Create `azure-hardening-audit.sh` (see Part 6.1)
2. Make executable: `chmod +x azure-hardening-audit.sh`
3. Run audit: `./azure-hardening-audit.sh`
4. Schedule with cron: `crontab -e`
   ```
   0 2 * * * /path/to/azure-hardening-audit.sh >> /var/log/security-audit.log 2>&1
   ```

---

## Real-World Context

### Case Study 1: Equifax Breach (2017)

**What Happened**:
- Unpatched Apache Struts vulnerability (CVE-2017-5638)
- 147 million records compromised
- Cost: $1.4 billion in fines + settlements

**Hardening Failure**:
- ❌ Patch management process failed (patch available 2 months before breach)
- ❌ Network segmentation insufficient (database accessible from web tier)
- ❌ Encryption not enforced for sensitive data in transit

**How Day 55 Prevents This**:
1. **Automated patching**: Unattended-upgrades ensures OS patches applied
2. **Network hardening**: NSG rules enforce segmentation
3. **Encryption enforcement**: Checkov flags missing HTTPS/TLS configs

### Case Study 2: Capital One Breach (2019)

**What Happened**:
- Misconfigured AWS firewall (security group too permissive)
- 100 million customer records stolen
- Cost: $80 million fine + $190 million settlement

**Hardening Failure**:
- ❌ Security group allowed 0.0.0.0/0 access to metadata service
- ❌ IAM role too permissive (allowed access to 700+ S3 buckets)
- ❌ No IaC scanning to catch misconfiguration

**How Day 55 Prevents This**:
1. **Checkov CKV_AZURE_12**: Detects permissive NSG rules
2. **Least privilege**: Managed Identity audit script finds overprivileged roles
3. **IaC scanning**: Catches misconfigurations before deployment

### Industry Statistics

**Verizon 2024 Data Breach Investigations Report**:
- **61%** of breaches involve credentials
- **74%** include a human element
- **43%** of breaches target small businesses
- **Average time to identify breach**: 207 days
- **Average cost per breach**: $4.45 million

**Top Attack Vectors**:
1. Unpatched vulnerabilities (58%)
2. Misconfigurations (42%)
3. Weak credentials (37%)
4. Insider threats (19%)

**Day 55 Coverage**:
- ✅ Patching automation (unattended-upgrades)
- ✅ Configuration validation (Checkov)
- ✅ Credential management (Day 53 secret rotation)
- ✅ Access control (RBAC audit)

---

## Interview Preparation

### Q1: "Walk me through your hardening process for Azure resources"

**Answer**:
"I use a defense-in-depth approach with 5 layers:

1. **Assessment**: Run Checkov on all Bicep templates + Azure Defender recommendations to establish baseline. From my Day 55 project, I improved from 73% to 90%+ pass rate.

2. **Prioritization**: P0 fixes (high severity, low effort) first - HTTPS enforcement, TLS 1.2, public access disabled. These are quick wins with immediate impact.

3. **Implementation**: Update Bicep templates with hardening configs:
   - Storage: `supportsHttpsTrafficOnly: true`, `minimumTlsVersion: TLS1_2`
   - Key Vault: `enableSoftDelete: true`, `enablePurgeProtection: true`
   - NSGs: Deny 0.0.0.0/0, allow specific subnets only

4. **Validation**: Re-run Checkov and my custom `azure-hardening-audit.sh` script to verify improvements. The script checks HTTPS, TLS, soft delete, public access, and Secure Score.

5. **Automation**: GitHub Actions workflow runs Checkov on every PR to prevent regressions. Scheduled weekly scans ensure continuous compliance."

**Follow-up Evidence**:
- Show `azure-hardening-audit.sh` (5 automated checks, color-coded output)
- Demonstrate before/after Checkov results (73% → 90%+)
- Explain CIS Azure Foundations benchmarks implemented

---

### Q2: "How do you balance security with usability?"

**Answer**:
"Security should be transparent - users shouldn't fight the system. My approach:

1. **Least Privilege by Default**: Start with deny-all NSG rules, open only necessary ports. Example: HTTPS (443) allowed, SSH (22) denied from internet.

2. **Progressive Hardening**: Don't break production. I use:
   - **Soft fail** mode initially (Checkov warnings, not errors)
   - **Phased rollout** (dev → staging → prod)
   - **Rollback plan** (keep previous template version)

3. **Developer Experience**: 
   - Document why each control exists (PCI-DSS requirement, not arbitrary)
   - Provide compliant templates (copy-paste ready)
   - Automate scanning (developers get instant feedback in PR)

4. **Exception Process**:
   - Business justification required
   - Compensating controls (if public access needed, add WAF + logging)
   - Time-limited exceptions (review every 90 days)

Real example from Day 55: Storage account public access disabled by default, but I allow `bypass: 'AzureServices'` for legitimate Azure integrations."

---

### Q3: "Explain a time you found a critical security misconfiguration"

**Answer**:
"During Day 47's Checkov scan, I found 16 failed checks in my own templates. The most critical:

**Issue**: Storage account allowed HTTP (not HTTPS only) + public blob access enabled.

**Risk**: 
- Unencrypted data in transit (violates PCI-DSS, GDPR)
- Public exposure of potentially sensitive data
- MITM attacks possible

**Impact**: If deployed, this would fail compliance audits and risk data breach.

**Fix**: 
```bicep
supportsHttpsTrafficOnly: true
minimumTlsVersion: 'TLS1_2'
allowBlobPublicAccess: false
publicNetworkAccess: 'Disabled'
```

**Prevention**: Added pre-commit hook with Checkov to catch before Git push. This shifted security left - finding issues in IDE, not production.

**Learning**: IaC scanning is non-negotiable. One misconfigured template can deploy 100 vulnerable resources. Automated scanning prevents human error at scale."

---

### Q4: "How do you stay current with security best practices?"

**Answer**:
"I use a multi-source approach:

1. **Frameworks**: CIS benchmarks (updated quarterly), NIST 800-53, Azure Security Baseline
2. **Tools**: Checkov rules updated weekly, Microsoft Defender recommendations
3. **Threat Intel**: Microsoft Security Response Center (MSRC), CVE databases
4. **Community**: Azure Security subreddit, Cloud Security Podcast
5. **Hands-on**: My 365-day Azure Security journey (Day 55 today) - learning by doing

Example: Day 53 I learned about secret rotation (PCI-DSS 4.0 now requires 90 days, changed from 1 year). Updated my templates same day.

**Process**:
- Weekly: Review Defender recommendations for new patterns
- Monthly: Update Checkov, review failed checks for new rules
- Quarterly: Re-scan all infrastructure with latest tools
- Yearly: Re-certification (planning Azure Security Engineer Associate)"

---

### Q5: "What metrics do you use to measure security posture?"

**Answer**:
"I track 4 key metrics:

1. **Secure Score** (0-100%):
   - Azure Defender provides this automatically
   - Target: 90%+ (my current: 85% after Day 55 hardening)
   - Tracks 50+ controls across identity, data, apps, network

2. **Checkov Pass Rate**:
   - Before hardening: 73% (43 passed, 16 failed)
   - After hardening: 90%+ (goal for Day 55)
   - Measured per deployment, per resource type

3. **Mean Time to Remediate (MTTR)**:
   - P0 issues: < 24 hours
   - P1 issues: < 7 days
   - Track with GitHub Issues (label: security)

4. **Configuration Drift**:
   - Weekly Checkov scans detect changes
   - Target: 0 manual changes (IaC only)
   - My script `azure-hardening-audit.sh` automates this

**Dashboard**: I created a simple script that outputs:
```
✅ HTTPS enforcement: 5/5 storage accounts
✅ TLS 1.2 minimum: 5/5 storage accounts
✅ Soft delete: 3/3 key vaults
✅ Secure Score: 85%
```

This runs in GitHub Actions weekly, alerts if score drops below threshold."

---

## Summary

**Day 55 Objectives**:
1. ✅ Assess current security posture (baseline)
2. ✅ Fix Checkov findings (73% → 90%+)
3. ✅ Implement CIS Azure benchmarks
4. ✅ Create automated hardening scripts
5. ✅ Document improvements with metrics

**Skills Acquired**:
- Security posture assessment
- IaC security remediation
- CIS benchmark implementation
- OS-level hardening (Linux)
- Network security (NSGs, flow logs)
- Continuous compliance automation

**Cost**: €0.00 (free tier only)

**Next Session**: Day 57 - Q2 Capstone Planning (November 12)

---

## Resources

**Official Documentation**:
- [CIS Azure Foundations Benchmark](https://www.cisecurity.org/benchmark/azure)
- [Microsoft Cloud Security Benchmark](https://docs.microsoft.com/en-us/security/benchmark/azure/)
- [Azure Security Baseline](https://docs.microsoft.com/en-us/security/benchmark/azure/security-baselines-overview)

**Tools**:
- [Checkov](https://www.checkov.io/)
- [Lynis](https://cisofy.com/lynis/)
- [OpenSCAP](https://www.open-scap.org/)

**Standards**:
- PCI-DSS 4.0
- NIST 800-53
- ISO 27001:2022
- SOC 2 Type II

Good luck with Day 55! 🔒
