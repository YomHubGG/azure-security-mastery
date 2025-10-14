# Day 37: ACR Security Exercises

**Goal**: Master ACR security concepts through CLI exploration and documentation

---

## 🎯 Exercise 1: ACR Name Availability Check

Learn ACR naming rules and check availability:

```bash
# Check if your desired ACR name is available
az acr check-name --name yomsecureacr

# Expected output:
# {
#   "message": null,
#   "nameAvailable": true/false,
#   "reason": null
# }

# Try different names:
az acr check-name --name yom2025registry
az acr check-name --name azuresecuritylearning

# Test invalid names (learn the rules):
az acr check-name --name my-acr          # ❌ Hyphens not allowed
az acr check-name --name MyACR           # ❌ Must be lowercase
az acr check-name --name ab              # ❌ Too short (min 5 chars)
```

**Learning**: ACR names must be:
- 5-50 characters
- Alphanumeric only (no hyphens, underscores)
- Lowercase only
- Globally unique across all Azure

---

## 🎯 Exercise 2: Understand ACR Pricing

Calculate costs for different scenarios:

```bash
# Scenario 1: Your Current Learning
# - 1 app (354 MB from Day 35)
# - Development only
# - Single region (West Europe)

# ACR Basic:
# - €4.60/month base cost
# - €0.01/GB/month storage
# - Total: €4.60 + (0.354 GB × €0.01) = ~€4.61/month

# Scenario 2: Small Startup (5 apps)
# - 5 apps × 400 MB average = 2 GB total
# - Development + Production environments
# - CI/CD pipeline

# ACR Standard (need multiple webhooks):
# - €18.40/month base
# - €0.01/GB/month storage
# - Total: €18.40 + (2 GB × €0.01) = ~€18.42/month

# Scenario 3: Enterprise (100 apps)
# - 100 apps × 400 MB = 40 GB
# - 3 regions (geo-replication)
# - Private endpoints
# - Content trust

# ACR Premium × 3 regions:
# - €92/month per region × 3 = €276/month
# - Storage: 40 GB × €0.01 = €0.40/month
# - Total: ~€276.40/month
```

**Question**: For your current needs (learning), which tier makes sense?
**Answer**: None yet! Use local Podman or GitHub Container Registry (FREE)

---

## 🎯 Exercise 3: Compare Authentication Methods

Understand the security trade-offs:

```bash
# Method 1: Admin User (⚠️ Least Secure)
# What it is: Username + password shared across all users
# Security: ❌ No audit trail, shared credentials, hard to rotate
# Use case: Emergency breakglass access only

# Command (DON'T RUN - THEORY ONLY):
# az acr update --name myacr --admin-enabled true
# az acr credential show --name myacr
# Output: { "username": "myacr", "passwords": [...] }

# Why bad:
# - Anyone with password has full access
# - Can't tell who did what (no individual audit trail)
# - Difficult to rotate without breaking everything

# Method 2: Service Principal (✅ Good for Automation)
# What it is: Application identity with specific permissions
# Security: ✅ Scoped permissions, individual identity, rotatable
# Use case: CI/CD pipelines, automated deployments

# Command (THEORY):
# az ad sp create-for-rbac \
#   --name "github-actions-acr" \
#   --role acrpush \
#   --scopes /subscriptions/{sub-id}/resourceGroups/{rg}/providers/Microsoft.ContainerRegistry/registries/{acr}

# Why good:
# ✅ Scoped to specific role (acrpush, acrpull)
# ✅ Individual identity (audit trail shows "github-actions-acr")
# ✅ Rotatable credentials (update secret without changing SP)

# Method 3: Azure AD User (✅ Good for Developers)
# What it is: Your personal Azure AD account
# Security: ✅ MFA, conditional access, personal accountability
# Use case: Developer workflows, manual operations

# Command:
# az acr login --name myacr

# Why good:
# ✅ Your personal identity (audit shows your name)
# ✅ MFA enforcement possible
# ✅ Can be revoked individually

# Method 4: Managed Identity (✅✅ Best for Azure Services)
# What it is: Automatic identity for Azure resources
# Security: ✅✅ No credentials at all, automatic management
# Use case: AKS pulling images, ACI running containers

# Command (THEORY):
# az aks create --attach-acr myacr --enable-managed-identity

# Why best:
# ✅✅ NO CREDENTIALS (nothing to steal!)
# ✅ Automatic credential management
# ✅ Scoped permissions
# ✅ Individual audit trail
```

**Exercise**: Rank these methods from most to least secure for each scenario:
1. **CI/CD Pipeline**: Managed Identity (if on Azure) > Service Principal > Admin User
2. **Developer Access**: Azure AD User > Service Principal > Admin User
3. **AKS Cluster**: Managed Identity > Service Principal > Admin User
4. **Emergency Access**: Service Principal > Admin User (disable after use)

---

## 🎯 Exercise 4: Network Security Levels

Design network security for different environments:

```bash
# Level 0: Public + No Restrictions (❌ NEVER DO THIS)
# - Anyone on internet can access
# - No authentication check at network level
# - Only ACR authentication required

# Risk: Brute force attacks, DoS, data exfiltration attempts

# Level 1: Public + IP Allowlist (✅ Acceptable for Dev)
# Add your current IP:
# az acr network-rule add --name myacr --ip-address $(curl -s ifconfig.me)/32

# Add CI/CD server:
# az acr network-rule add --name myacr --ip-address 20.40.50.60/32

# Result: Only specific IPs can access (network-level protection)

# Level 2: Public + Azure Services Only (✅ Good for Most)
# az acr update --name myacr --public-network-enabled true --default-action Deny
# az acr network-rule add --name myacr --action Allow --service-tag AzureCloud

# Result: Only Azure services can access (blocks external attackers)

# Level 3: Private Endpoint Only (✅✅ Best for Production)
# az network private-endpoint create --name acr-pe --resource-group rg \
#   --vnet-name myvnet --subnet mysubnet \
#   --private-connection-resource-id /subscriptions/.../myacr \
#   --connection-name acr-connection

# Result: ACR has NO public IP, only accessible from VNet

# az acr update --name myacr --public-network-enabled false
```

**Design Exercise**: For each environment, choose the right level:

| Environment | Network Security | Justification |
|-------------|------------------|---------------|
| Development | Level 1 (IP allowlist) | Developer IPs + CI/CD |
| Staging | Level 2 (Azure services) | No external access needed |
| Production | Level 3 (Private endpoint) | Maximum isolation |
| CI/CD | Level 1 (IP allowlist) | GitHub Actions IPs |

---

## 🎯 Exercise 5: Vulnerability Management Workflow

Understand the scanning process:

```bash
# Step 1: Enable Microsoft Defender for Containers (costs extra)
# az security pricing create --name ContainerRegistry --tier Standard

# Step 2: Push image → Automatic scan triggers
# podman push myacr.azurecr.io/app:v1.0.0
# (Defender scans within minutes)

# Step 3: View scan results
# az security assessment list \
#   --resource-group rg \
#   --resource myacr \
#   --resource-type Microsoft.ContainerRegistry/registries

# Example output:
# {
#   "image": "app:v1.0.0",
#   "vulnerabilities": [
#     {
#       "id": "CVE-2024-1234",
#       "severity": "CRITICAL",
#       "package": "openssl",
#       "version": "1.1.1g",
#       "fixedVersion": "1.1.1w"
#     },
#     {
#       "id": "CVE-2024-5678",
#       "severity": "HIGH",
#       "package": "curl",
#       "version": "7.68.0",
#       "fixedVersion": "7.88.1"
#     }
#   ]
# }

# Step 4: Decide action based on severity
# CRITICAL → Block deployment, fix immediately
# HIGH → Fix within 7 days, document exception if can't fix
# MEDIUM → Fix within 30 days
# LOW → Fix when convenient
```

**Scenario**: Your image scan shows:
- 1 CRITICAL: OpenSSL RCE vulnerability
- 3 HIGH: Kernel privilege escalation
- 10 MEDIUM: Various packages
- 25 LOW: Minor issues

**What do you do?**
1. ❌ Block production deployment (critical vulnerability)
2. ✅ Update base image to latest Alpine (fixes OpenSSL)
3. ✅ Rebuild image with new base
4. ✅ Re-scan (verify critical is fixed)
5. ✅ Document HIGH vulnerabilities (fix in next sprint)
6. ✅ Deploy to dev first, then staging, then production

---

## 🎯 Exercise 6: Content Trust (Image Signing)

Understand why image signing matters:

```bash
# Threat Model: Supply Chain Attack
# Attacker compromises CI/CD pipeline
# Pushes malicious image: myacr.azurecr.io/app:v1.0.0
# Image contains backdoor

# Without Content Trust:
# ❌ AKS pulls and runs malicious image
# ❌ No way to verify authenticity
# ❌ Backdoor deployed to production

# With Content Trust:
# ✅ Only signed images can be pulled
# ✅ Attacker can't sign (no private key)
# ✅ AKS rejects unsigned/invalid image
# ✅ Production protected

# How signing works:
# 1. Developer has private key (kept secret)
# 2. Developer signs image with private key
# 3. Signature stored in ACR
# 4. Public key stored in ACR
# 5. AKS pulls image + signature
# 6. AKS verifies signature with public key
# 7. If valid → Run, if invalid → Reject
```

**Commands (THEORY - Content Trust is Premium tier)**:

```bash
# Enable content trust
az acr config content-trust update --name myacr --status enabled

# Sign image on push (Docker)
export DOCKER_CONTENT_TRUST=1
docker push myacr.azurecr.io/app:v1.0.0

# What happens:
# - Docker generates root key (asks for passphrase)
# - Creates repository key for "app"
# - Signs image manifest
# - Pushes image + signature to ACR

# Pull with verification
export DOCKER_CONTENT_TRUST=1
docker pull myacr.azurecr.io/app:v1.0.0

# What happens:
# - Docker downloads image + signature
# - Verifies signature with public key
# - If valid: Pulls and runs
# - If invalid: Rejects with error
```

**Exercise**: Explain to a non-technical person:
*"Image signing is like a tamper-proof seal on medicine bottles. The manufacturer (developer) seals the bottle with a unique code. The pharmacy (Azure) can verify the seal is authentic and unbroken. If someone tampered with it, the seal breaks and you know not to use it."*

---

## 🎯 Exercise 7: Cost-Benefit Analysis

Make the deployment decision:

**Your Situation**:
- Learning Azure Security (not production)
- 1 container built (Day 35)
- Budget-conscious (€0 spent so far)
- GitHub account available

**Option A: No Registry (Current)**
- Cost: €0
- Store images: Local Podman only
- Can share: No
- Portfolio value: Low
- Production-ready: No

**Option B: ACR Basic**
- Cost: €4.60/month (€55/year)
- Store images: Private Azure registry
- Can share: Yes (with RBAC)
- Portfolio value: Medium
- Production-ready: Yes (single region)

**Option C: GitHub Container Registry**
- Cost: €0 (FREE unlimited private)
- Store images: GitHub packages
- Can share: Yes (with GitHub auth)
- Portfolio value: High (visible in portfolio)
- Production-ready: Yes (GitHub SLA)

**Decision Framework**:

| Need | Best Option |
|------|-------------|
| Learning only | Option A (local) |
| Portfolio showcase | Option C (GitHub) |
| Azure-native practice | Option B (ACR Basic) |
| Production deployment | Option B or C |
| Multi-region | ACR Premium (€276/mo) |

**Recommendation for You**: **Option C (GitHub Container Registry)**
- ✅ FREE (saves €55/year)
- ✅ Portfolio-visible (employers can see)
- ✅ GitHub Actions integration (you learned Day 23!)
- ✅ Can migrate to ACR anytime
- ✅ All concepts learned still apply

---

## 🎯 Exercise 8: Create Your ACR Security Playbook

Fill out this decision tree for future reference:

```markdown
# When to Use ACR vs Alternatives

## Use ACR Basic (€4.60/mo) IF:
- [ ] Running production workloads on Azure
- [ ] Need Azure AD integration
- [ ] AKS or ACI deployment planned
- [ ] Compliance requires Azure-native registry
- [ ] Budget allows €55/year

## Use ACR Standard (€18.40/mo) IF:
- [ ] All Basic reasons +
- [ ] Need >2 webhooks for automation
- [ ] >10 GB storage required
- [ ] Multiple environments (dev/staging/prod)

## Use ACR Premium (€92/mo) IF:
- [ ] All Standard reasons +
- [ ] Need geo-replication
- [ ] Private endpoints required (compliance)
- [ ] Content trust (image signing) required
- [ ] >100 GB storage needed

## Use GitHub Container Registry (FREE) IF:
- [ ] Learning/portfolio projects
- [ ] GitHub-centric workflow
- [ ] Budget-conscious
- [ ] Public or private GitHub repos
- [ ] Don't need Azure-native features

## Use Local Podman Only IF:
- [ ] Learning Docker/container basics
- [ ] Not sharing with anyone
- [ ] No deployment planned
- [ ] Experimenting only
```

---

## ✅ Exercise Completion Checklist

Mark each as complete:

- [ ] Checked ACR name availability with `az acr check-name`
- [ ] Calculated cost for your scenario (should be €0 using GitHub)
- [ ] Ranked authentication methods by security
- [ ] Designed network security for dev/staging/prod
- [ ] Understood vulnerability scanning workflow
- [ ] Explained content trust (image signing) concept
- [ ] Completed cost-benefit analysis
- [ ] Created ACR decision playbook
- [ ] Decided on registry strategy (recommend: GitHub for now)

---

## 🚀 Next Actions

### **Recommended: Use GitHub Container Registry**

1. Create GitHub Personal Access Token
2. Login with Podman: `podman login ghcr.io`
3. Tag your Day 35 image for GitHub
4. Push to ghcr.io
5. View in your GitHub packages
6. **Cost: €0** ✅

### **Alternative: Wait for ACR**

1. Continue learning (Day 39 coming up!)
2. Deploy ACR when budget allows
3. All knowledge learned today still applies
4. Can migrate from GitHub to ACR easily

### **Not Recommended: Deploy ACR Basic Now**

- Costs €55/year
- GitHub offers same features for free
- Better to save money for AKS later (Day 41)

---

**Exercises completed = Day 37 mastered!** 🎯
