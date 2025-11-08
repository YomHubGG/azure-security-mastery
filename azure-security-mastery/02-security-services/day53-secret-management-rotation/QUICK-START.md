# 🚀 Day 53 Quick Start Guide

**Date**: November 8, 2025  
**Session**: #27  
**Duration**: 1-2 hours  
**Focus**: Secret Management & Rotation

---

## 📋 **Session Overview**

Today you're learning advanced secret management practices that close the gap from Day 17:

- ✅ **Automated secret rotation** (90-day policies)
- ✅ **Certificate lifecycle** (auto-renewal)
- ✅ **Expiration monitoring** (proactive alerts)
- ✅ **Git secret scanning** (leak detection)
- ✅ **Managed Identity audits** (least privilege)

---

## 🎯 **Learning Path**

### **Lab 1: Secret Rotation Audit (15 min)**

Check your existing Key Vault secrets:

```bash
# Navigate to Day 53 directory
cd /home/yom/cybersecurity-journey/azure-security-mastery/02-security-services/day53-secret-management-rotation

# Run rotation audit
./rotation-audit.sh

# Expected output:
# - List of all secrets with expiration status
# - Color-coded warnings (green=healthy, yellow=warning, red=critical)
# - Action items for secrets without expiration
```

**Expected findings:**
- Most secrets from Day 17 likely have NO expiration dates
- You'll set 90-day expiration policies

---

### **Lab 2: Set Secret Expiration (20 min)**

Update your existing secrets with rotation policies:

```bash
# Check current secrets
az keyvault secret list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --query "[].{Name:name, Expires:attributes.expires}" \
  --output table

# Deploy rotation policy with Bicep
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file rotation-policy.bicep \
  --parameters \
    keyVaultName=kvlearning4uybw3c2lbkwm \
    secretName=db-connection-string \
    secretValue='Server=myserver.database.azure.com;Database=mydb;User=admin;Password=ComplexP@ss123' \
    rotationIntervalDays=90

# Verify expiration is set
az keyvault secret show \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name db-connection-string \
  --query "{Name:name, Expires:attributes.expires, Tags:tags}" \
  --output json
```

**Success criteria:**
- ✅ Secret has `expires` attribute set
- ✅ Tags show `RotationPolicy: 90days`
- ✅ Rotation audit script shows "healthy" status

---

### **Lab 3: Certificate Lifecycle (20 min)**

Create a self-signed certificate with auto-renewal:

```bash
# Create certificate with Bicep
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file certificate-lifecycle.bicep \
  --parameters \
    keyVaultName=kvlearning4uybw3c2lbkwm \
    certificateName=secure-app-cert \
    subject='CN=secure-app.yomhubgg.dev' \
    validityInMonths=12 \
    renewalDaysBeforeExpiry=30

# Check certificate status
az keyvault certificate show \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name secure-app-cert \
  --query "{Name:name, Enabled:attributes.enabled, Expires:attributes.expires, Thumbprint:thumbprint}" \
  --output json

# List all certificates
az keyvault certificate list \
  --vault-name kvlearning4uybw3c2lbkwm \
  --output table
```

**Success criteria:**
- ✅ Certificate created with 12-month validity
- ✅ Auto-renewal configured (30 days before expiry)
- ✅ Thumbprint generated

**Alternative (CLI only):**

```bash
# Create certificate policy
cat > cert-policy.json << 'EOF'
{
  "issuerParameters": {"name": "Self"},
  "keyProperties": {"exportable": true, "keySize": 2048, "keyType": "RSA"},
  "lifetimeActions": [{
    "trigger": {"daysBeforeExpiry": 30},
    "action": {"actionType": "AutoRenew"}
  }],
  "secretProperties": {"contentType": "application/x-pkcs12"},
  "x509CertificateProperties": {
    "subject": "CN=secure-app.yomhubgg.dev",
    "validityInMonths": 12
  }
}
EOF

az keyvault certificate create \
  --vault-name kvlearning4uybw3c2lbkwm \
  --name secure-app-cert \
  --policy @cert-policy.json
```

---

### **Lab 4: Expiration Monitoring (10 min)**

Set up automated monitoring:

```bash
# Run expiration monitor
./expiration-monitor.sh

# Expected output:
# - Alerts for secrets expiring in 30 days (WARNING)
# - Alerts for secrets expiring in 7 days (CRITICAL)
# - Exit code: 0=OK, 1=Warning, 2=Critical

# Check exit code
echo "Exit code: $?"
```

**Production setup (optional):**

```bash
# Add to crontab for daily checks
crontab -e

# Add this line (runs daily at 9 AM):
# 0 9 * * * /home/yom/cybersecurity-journey/azure-security-mastery/02-security-services/day53-secret-management-rotation/expiration-monitor.sh >> /tmp/keyvault-monitor.log 2>&1
```

---

### **Lab 5: Git Secret Scanning (20 min)**

Scan your repository for leaked secrets:

**Option 1: Install TruffleHog**

```bash
# Download TruffleHog
cd /tmp
wget https://github.com/trufflesecurity/trufflehog/releases/download/v3.63.2/trufflehog_3.63.2_linux_amd64.tar.gz
tar -xzf trufflehog_3.63.2_linux_amd64.tar.gz
sudo mv trufflehog /usr/local/bin/
trufflehog --version

# Return to Day 53 directory
cd /home/yom/cybersecurity-journey/azure-security-mastery/02-security-services/day53-secret-management-rotation

# Run secret scan
./secret-scan-report.sh
```

**Option 2: Use existing gitleaks (from Day 49)**

```bash
# Scan with gitleaks
cd /home/yom/cybersecurity-journey/azure-security-mastery

gitleaks detect \
  --source . \
  --report-path 02-security-services/day53-secret-management-rotation/gitleaks-report.json \
  --verbose

# Check results
cat 02-security-services/day53-secret-management-rotation/gitleaks-report.json | jq
```

**Success criteria:**
- ✅ Scan completes without errors
- ✅ Zero verified secrets found (repository is clean)
- ✅ Report files generated

**If secrets are found:**
1. Rotate them immediately in Key Vault
2. Update applications with new values
3. Document the incident

---

### **Lab 6: Managed Identity Audit (15 min)**

Audit your Managed Identities:

```bash
# Run Managed Identity audit
./managed-identity-audit.sh

# Expected output:
# - List of all system-assigned identities
# - List of all user-assigned identities
# - Permission audit for each identity
# - Security warnings for overprivileged identities
```

**Common findings:**
- Contributor role at subscription level (too broad)
- Owner role assigned (excessive)
- Multiple unnecessary roles

**Fix overprivileged identities:**

```bash
# Example: Remove Contributor, grant specific Key Vault access
PRINCIPAL_ID="<from-audit-output>"

# Remove broad permission
az role assignment delete \
  --assignee $PRINCIPAL_ID \
  --role Contributor

# Grant specific permission
az role assignment create \
  --assignee $PRINCIPAL_ID \
  --role "Key Vault Secrets User" \
  --scope "/subscriptions/$(az account show --query id -o tsv)/resourceGroups/rg-learning-day1/providers/Microsoft.KeyVault/vaults/kvlearning4uybw3c2lbkwm"
```

---

## 📊 **Success Checklist**

By the end of today's session:

✅ **Lab 1-2: Secret Rotation**
- [ ] Rotation audit script executed
- [ ] All secrets have 90-day expiration
- [ ] Rotation policy Bicep template deployed

✅ **Lab 3: Certificate Lifecycle**
- [ ] Self-signed certificate created
- [ ] Auto-renewal policy configured (30 days)
- [ ] Certificate details verified

✅ **Lab 4: Expiration Monitoring**
- [ ] Expiration monitor script executed
- [ ] Alert thresholds understood (30-day warning, 7-day critical)

✅ **Lab 5: Git Secret Scanning**
- [ ] TruffleHog or gitleaks installed
- [ ] Full repository scanned
- [ ] Zero verified secrets found

✅ **Lab 6: Managed Identity Audit**
- [ ] All identities inventoried
- [ ] Permissions audited
- [ ] Overprivileged identities fixed (if any)

---

## 🎯 **Key Takeaways**

**Portfolio Talking Points:**

1. **"I implemented automated secret rotation policies in Azure Key Vault"**
   - 90-day expiration on all secrets
   - Bicep templates for Infrastructure-as-Code
   - Monitoring scripts for proactive alerts

2. **"I secured our codebase against credential leaks"**
   - Scanned 66 days of Git history with TruffleHog
   - Zero verified secrets in repository
   - Established secret scanning workflow

3. **"I audited Managed Identity permissions using least-privilege"**
   - Identified overprivileged roles
   - Applied least-privilege principle
   - Reduced attack surface

**Interview Questions You Can Now Answer:**

1. **"How do you handle secret rotation in Azure?"**
   - ✅ Key Vault with 90-day expiration policies
   - ✅ Automated monitoring with bash scripts
   - ✅ Certificate auto-renewal 30 days before expiry
   - ✅ Infrastructure-as-Code with Bicep

2. **"What's the difference between a secret and a certificate?"**
   - **Secret**: Plain text value (API keys, passwords, connection strings)
   - **Certificate**: X.509 with public/private key pair (TLS/SSL, code signing)
   - Both managed in Key Vault, different use cases

3. **"How do you prevent secrets from being committed to Git?"**
   - Pre-commit hooks (git-secrets)
   - CI/CD scanning (gitleaks, TruffleHog)
   - GitHub secret scanning (automatic)
   - Regular audits with automated tools

4. **"What is Managed Identity and why use it?"**
   - Azure-managed credentials (no passwords/keys stored)
   - Automatic token rotation by Azure
   - Eliminates secret storage in code
   - Least-privilege access control

---

## 📈 **Progress Update**

**Before Today:**
- Day 17: Basic Key Vault setup ✅
- Day 45: OIDC eliminated long-lived secrets ✅

**After Today:**
- ✅ Secret rotation policies automated
- ✅ Certificate lifecycle management
- ✅ Expiration monitoring system
- ✅ Git repository secured
- ✅ Managed Identity least-privilege

**Next Session (Day 55 - Nov 12):**
- Security hardening (fix remaining Checkov findings)
- Implement Defender for Cloud recommendations
- Achieve 90%+ security score

---

## 🔗 **Files Created Today**

```
day53-secret-management-rotation/
├── README.md                        # Complete guide (this file)
├── QUICK-START.md                   # Quick reference (you are here)
├── rotation-policy.bicep            # Secret rotation template
├── certificate-lifecycle.bicep      # Certificate auto-renewal template
├── rotation-audit.sh               # Secret expiration audit script
├── expiration-monitor.sh           # Automated monitoring script
├── secret-scan-report.sh           # Git secret scanning script
└── managed-identity-audit.sh       # Managed Identity audit script
```

---

## 💡 **Tips**

1. **Run rotation audit weekly** to stay on top of expiring secrets
2. **Test certificate renewal** by setting short validity (1 month) and monitoring
3. **Integrate monitoring** into your CI/CD pipeline for continuous validation
4. **Document exceptions** - if a secret can't have expiration, document why
5. **Automate everything** - manual rotation = human error

---

## 🚀 **Ready?**

Start with Lab 1 (rotation audit) and work through each lab in order. Each lab builds on the previous one.

**Let's secure those secrets!** 🔐
