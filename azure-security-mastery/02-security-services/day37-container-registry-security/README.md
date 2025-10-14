# Day 37: Azure Container Registry (ACR) Security 🏗️

**Date**: October 14, 2025  
**Focus**: Enterprise Container Registry Security  
**Cost Strategy**: Theory + CLI exploration (€0 if no deployment)  
**Status**: In Progress 🚧

---

## 🎯 Learning Objectives

By the end of this session, you will:

1. ✅ Understand Azure Container Registry (ACR) architecture
2. ✅ Master ACR security features and best practices
3. ✅ Learn image signing and content trust
4. ✅ Understand vulnerability scanning integration
5. ✅ Know RBAC and access control patterns
6. ✅ Decide: Deploy for hands-on OR document for theory (cost-aware)

---

## 📋 Prerequisites

### **What You Already Have**:
- ✅ **Day 35 Complete**: Docker/Podman security fundamentals
- ✅ **Secure container built**: `secure-app:1.0.0` (ready to push!)
- ✅ **Multi-stage Dockerfile**: Production security patterns
- ✅ **Podman installed**: Rootless container management

### **What You'll Build On**:
From Day 35, you have a production-ready container. Today you'll learn:
- Where to store it securely (ACR)
- How to protect images (signing, scanning)
- Who can access it (RBAC, private endpoints)
- How to maintain it (retention, replication)

---

## 💰 Cost Warning & Decision Point

### **ACR Pricing (2025)**
```
Tier           Cost/Month    Storage    Webhooks    Geo-Replication
───────────────────────────────────────────────────────────────────
Basic          €4.60         10 GB      2           ❌
Standard       €18.40        100 GB     10          ❌
Premium        €92.00        500 GB     500         ✅
```

### **🚨 IMPORTANT DECISION**

**Option A: Theory-Only (€0)** ✅ RECOMMENDED
- Learn all concepts and CLI commands
- Explore ACR features via documentation
- Understand security architecture
- Ready to implement when budget allows
- **Cost**: €0 (documentation-based learning)

**Option B: Hands-On Deployment (€4.60/month minimum)**
- Deploy actual ACR Basic tier
- Push your Day 35 container
- Practice real commands
- Experience live security features
- **Cost**: €4.60/month (€0.15/day)

### **My Recommendation**: 
**Theory-only for now**. You've proven you can build containers (Day 35). ACR security concepts are more valuable than hands-on at this stage. Save the €55/year for certifications or AKS later!

---

## 📚 Azure Container Registry Architecture

### **What is ACR?**

Azure Container Registry is Microsoft's **private Docker registry** - like Docker Hub but:
- 🔒 **Private by default** (no public images)
- 🛡️ **Enterprise security** (RBAC, AAD integration, private endpoints)
- 🌍 **Geo-replication** (Premium tier - global distribution)
- 🔍 **Vulnerability scanning** (Microsoft Defender integration)
- 🔐 **Content trust** (Image signing with Notary)

### **ACR vs Alternatives**

| Feature | ACR | Docker Hub | GitHub Container Registry |
|---------|-----|-----------|---------------------------|
| **Privacy** | Private default | Public default | Private default |
| **Azure Integration** | Native | External | External |
| **RBAC** | Azure AD | Basic | GitHub teams |
| **Vulnerability Scan** | Built-in | Paid tier | Built-in |
| **Geo-Replication** | Yes (Premium) | No | Global CDN |
| **Cost (private)** | €4.60+/mo | Free (1 repo) | Free (unlimited) |
| **Best For** | Azure workloads | Public OSS | GitHub projects |

### **ACR Architecture Components**

```
Azure Container Registry
├── Registry (yourname.azurecr.io)
│   ├── Repositories
│   │   ├── app-name (repository)
│   │   │   ├── latest (tag)
│   │   │   ├── v1.0.0 (tag)
│   │   │   └── sha256:abc123 (manifest)
│   │   └── another-app
│   │       └── v2.0.0
│   │
│   ├── Security
│   │   ├── RBAC (Azure AD)
│   │   ├── Service Principal
│   │   ├── Managed Identity
│   │   └── Admin User (⚠️ not recommended)
│   │
│   ├── Networking
│   │   ├── Public Endpoint (default)
│   │   ├── Private Endpoint (Premium)
│   │   └── Firewall Rules
│   │
│   ├── Content
│   │   ├── Content Trust (signing)
│   │   ├── Quarantine Policy
│   │   └── Retention Policy
│   │
│   └── Scanning
│       ├── Microsoft Defender
│       ├── Vulnerability Reports
│       └── Compliance Scanning
```

---

## 🔐 ACR Security Features Deep Dive

### **1. Authentication & Authorization**

#### **Authentication Methods (from most to least secure)**

```bash
# 1. MANAGED IDENTITY (Best - no credentials!)
# Used by AKS, ACI, Azure Functions
az aks update --attach-acr <acr-name>

# 2. SERVICE PRINCIPAL (Good - scoped credentials)
# For CI/CD pipelines
az ad sp create-for-rbac \
  --name acr-service-principal \
  --role acrpull \
  --scopes /subscriptions/{subscription-id}/resourceGroups/{rg}/providers/Microsoft.ContainerRegistry/registries/{acr-name}

# 3. AZURE AD USER (Good - personal access)
# For developers
az acr login --name <acr-name>

# 4. ADMIN USER (⚠️ Not recommended - shared password)
# Emergency access only
az acr update --name <acr-name> --admin-enabled true
az acr credential show --name <acr-name>
```

#### **RBAC Roles**

| Role | Permissions | Use Case |
|------|-------------|----------|
| **AcrPull** | Pull images only | Production workloads (AKS, ACI) |
| **AcrPush** | Pull + Push images | CI/CD pipelines |
| **AcrDelete** | Pull + Push + Delete | DevOps engineers |
| **Owner** | Full control + RBAC | Security admins |
| **Reader** | View metadata only | Auditors |

#### **Security Best Practices**

```bash
# ✅ GOOD: Managed Identity (AKS cluster pulling images)
az aks create \
  --name my-aks \
  --attach-acr <acr-name> \
  --enable-managed-identity

# ✅ GOOD: Service Principal with minimum scope
az ad sp create-for-rbac \
  --name "github-actions-acr" \
  --role acrpush \
  --scopes /subscriptions/.../registries/myacr

# ❌ BAD: Admin user (shared password, audit nightmare)
az acr update --admin-enabled true  # Avoid this!

# ❌ BAD: Owner role for CI/CD (excessive permissions)
az role assignment create \
  --assignee <sp-id> \
  --role Owner  # Too much!
```

---

### **2. Network Security**

#### **Public vs Private Endpoints**

```bash
# Public Endpoint (Basic/Standard/Premium)
# - Accessible from internet
# - Can restrict by IP firewall
# - Good for: Development, CI/CD

# Private Endpoint (Premium only)
# - Accessible only from VNet
# - No public internet exposure
# - Good for: Production, compliance
```

#### **Firewall Rules (IP Allowlist)**

```bash
# Add your IP for management
az acr network-rule add \
  --name <acr-name> \
  --ip-address <your-ip>/32

# Add CI/CD server IP
az acr network-rule add \
  --name <acr-name> \
  --ip-address <github-runner-ip>/32

# List current rules
az acr network-rule list \
  --name <acr-name> \
  --output table
```

#### **Virtual Network Integration (Premium)**

```bash
# Create private endpoint
az network private-endpoint create \
  --name acr-private-endpoint \
  --resource-group <rg> \
  --vnet-name <vnet> \
  --subnet <subnet> \
  --private-connection-resource-id <acr-resource-id> \
  --group-id registry \
  --connection-name acr-connection

# Result: ACR accessible via private IP (10.0.1.5)
# Public endpoint disabled
```

---

### **3. Content Trust (Image Signing)**

**What is Content Trust?**
- Ensures images are from trusted publishers
- Cryptographic signatures verify authenticity
- Prevents tampering and supply chain attacks
- Uses **Notary** (CNCF project)

#### **How It Works**

```
Developer Signs Image               Registry Stores Signature
     ↓                                     ↓
[docker trust sign]              [ACR stores manifest]
     ↓                                     ↓
Private key signs                  Public key validates
SHA256 hash                        on pull
     ↓                                     ↓
Signature stored              ✅ Pull only if valid
in registry                   ❌ Reject if tampered
```

#### **Enabling Content Trust**

```bash
# Enable content trust in ACR (Premium only)
az acr config content-trust update \
  --name <acr-name> \
  --status enabled

# Sign image during push (Docker)
export DOCKER_CONTENT_TRUST=1
docker push myacr.azurecr.io/app:v1.0.0

# With Podman (requires skopeo for signing)
skopeo copy \
  --sign-by <gpg-key-id> \
  containers-storage:app:v1.0.0 \
  docker://myacr.azurecr.io/app:v1.0.0

# Verify signature on pull
export DOCKER_CONTENT_TRUST=1
docker pull myacr.azurecr.io/app:v1.0.0
# ✅ Pulls only if signature valid
# ❌ Fails if signature missing/invalid
```

#### **Content Trust Keys**

```bash
# Root key (offline, secured)
# - Signs delegation keys
# - Stored in hardware security module (HSM) in production

# Targets key (per-repository)
# - Signs image manifests
# - Rotated regularly

# Snapshots key
# - Signs metadata
# - Short-lived
```

---

### **4. Vulnerability Scanning**

#### **Microsoft Defender for Containers**

```bash
# Enable Defender for ACR (costs extra)
az security pricing create \
  --name ContainerRegistry \
  --tier Standard

# Automatic scanning:
# - Every push triggers scan
# - Daily re-scan of existing images
# - CVE database updated continuously

# View scan results
az security assessment list \
  --resource-group <rg> \
  --resource <acr-name> \
  --resource-type Microsoft.ContainerRegistry/registries
```

#### **Scan Results Interpretation**

```
Vulnerability Severity Levels:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
CRITICAL   🔴  Immediate action required (RCE, auth bypass)
HIGH       🟠  Urgent fix needed (privilege escalation)
MEDIUM     🟡  Fix in next sprint (information disclosure)
LOW        🟢  Fix when convenient (minor issues)
```

#### **Quarantine Policy**

```bash
# Automatically quarantine vulnerable images
az acr config content-trust update \
  --name <acr-name> \
  --policy-enabled true

# Images with CRITICAL vulnerabilities:
# - Blocked from being pulled
# - Marked as quarantined
# - Admin override required

# View quarantined images
az acr repository show-tags \
  --name <acr-name> \
  --repository <repo> \
  --query "[?quarantine=='true']"
```

---

### **5. Retention & Lifecycle Management**

#### **Retention Policies (Premium)**

```bash
# Delete untagged manifests after 7 days
az acr config retention update \
  --name <acr-name> \
  --status enabled \
  --days 7 \
  --type UntaggedManifests

# Why this matters:
# - Each push creates new manifest
# - Old manifests without tags accumulate
# - Storage costs increase
# - Retention policies auto-cleanup

# Example scenario:
# Day 1: Push app:v1.0.0 (manifest sha256:abc)
# Day 2: Push app:v1.0.0 (manifest sha256:def) ← new manifest
# Old manifest sha256:abc is now "untagged"
# Day 9: Old manifest auto-deleted (7-day retention)
```

#### **Tag Locking (Prevent Deletion)**

```bash
# Lock production images
az acr repository update \
  --name <acr-name> \
  --image app:v1.0.0 \
  --write-enabled false \
  --delete-enabled false

# ✅ Can pull
# ❌ Cannot push new image with same tag
# ❌ Cannot delete

# Unlock when needed
az acr repository update \
  --name <acr-name> \
  --image app:v1.0.0 \
  --write-enabled true \
  --delete-enabled true
```

---

### **6. Geo-Replication (Premium Only)**

#### **Why Geo-Replication?**

```
Problem: Pulling images from US to Europe = slow + expensive
Solution: Replicate ACR to multiple regions

Azure Container Registry (Primary: West Europe)
├── Replica: North Europe (automatic sync)
├── Replica: UK South (automatic sync)
└── Replica: France Central (automatic sync)

ACI/AKS pulls from nearest replica automatically
```

#### **Setting Up Replication**

```bash
# Replicate to another region
az acr replication create \
  --registry <acr-name> \
  --location northeurope

# List replicas
az acr replication list \
  --registry <acr-name> \
  --output table

# Benefits:
# ✅ Faster pull times (local region)
# ✅ Lower egress costs (no cross-region transfer)
# ✅ High availability (region failure tolerance)
# ❌ Cost: ~€92/month per region
```

---

## 🛠️ Hands-On: ACR Security Assessment

Since we're doing **theory-focused** learning today, let's explore ACR with CLI (no deployment):

### **Task 1: Check If You Have Existing ACRs**

```bash
# List all ACRs in your subscription
az acr list --output table

# Expected output: Possibly empty or show existing registries
# If you have one from past sessions, we'll audit it!
```

### **Task 2: Understand ACR Naming**

```bash
# ACR names are globally unique
# Format: <name>.azurecr.io
# Rules:
# - 5-50 alphanumeric characters
# - No hyphens, underscores
# - Lowercase only

# Check if a name is available
az acr check-name --name yourdesiredname

# Examples:
# ✅ mysecureacr
# ✅ yom2025registry
# ❌ my-acr (hyphens not allowed)
# ❌ MyACR (uppercase not allowed)
```

### **Task 3: Estimate Costs**

```bash
# Calculate your theoretical costs:

# Scenario 1: Basic tier for learning
# - €4.60/month
# - 10 GB storage (enough for ~50 small images)
# - 2 webhooks
# - No geo-replication

# Scenario 2: Standard tier for dev/test
# - €18.40/month
# - 100 GB storage
# - 10 webhooks
# - No geo-replication

# Scenario 3: Premium for production
# - €92/month
# - 500 GB storage
# - Geo-replication
# - Private endpoints
# - Content trust

# Your Day 35 container (354 MB):
# Storage cost: Negligible (€0.01/month in Basic tier)
# Real cost: The tier itself (€4.60/month minimum)
```

---

## 📖 ACR Security Best Practices (Theory)

### **1. Authentication Strategy**

```bash
# Production Pattern:
AKS Cluster → Managed Identity → ACR (AcrPull)
ACI → Managed Identity → ACR (AcrPull)
Azure Functions → Managed Identity → ACR (AcrPull)

# CI/CD Pattern:
GitHub Actions → Service Principal → ACR (AcrPush)
Azure DevOps → Service Principal → ACR (AcrPush)

# Developer Pattern:
Developer → Azure AD User → ACR (AcrPush/AcrDelete)

# Emergency Pattern:
Admin User → Disabled by default → Enable only for breakglass
```

### **2. Network Isolation Levels**

```
Level 0: Public + No firewall (❌ Not recommended)
  └─ Anyone can access
  
Level 1: Public + IP allowlist (✅ Acceptable for dev)
  └─ Only specific IPs can access
  
Level 2: Public + Azure trusted services (✅ Good for most)
  └─ Only Azure services can access
  
Level 3: Private endpoint only (✅✅ Best for production)
  └─ Only VNet resources can access
```

### **3. Image Lifecycle**

```
Development:
1. Build locally → Test
2. Push to ACR with dev tag (app:dev-20251014)
3. Scan automatically
4. Deploy to dev environment

Staging:
1. Promote image (retag: app:staging-1.2.3)
2. Re-scan (CVE database may have updated)
3. Deploy to staging
4. Run tests

Production:
1. Lock image (write-enabled: false)
2. Sign with content trust
3. Deploy to production
4. Keep for compliance period (e.g., 1 year retention)
```

### **4. Vulnerability Management**

```
High-Level Process:
1. Image pushed → Defender scans automatically
2. Vulnerabilities detected → Alert sent
3. Critical severity → Quarantine image
4. Team reviews → Decides fix or accept risk
5. Fix applied → Rebuild image → Push → Re-scan
6. Clean scan → Promote to production
```

---

## 🎓 Real-World ACR Security Scenarios

### **Scenario 1: Startup (Your Current Stage)**

**Needs:**
- Private registry for 2-3 apps
- Basic security
- Minimal cost

**Solution:**
- ACR Basic tier (€4.60/mo)
- Managed identity for ACI/AKS
- Service principal for GitHub Actions
- Public endpoint with IP allowlist

**Alternative:**
- GitHub Container Registry (FREE!)
- Good for learning, GitHub-integrated
- Consider ACR when you need Azure-native features

---

### **Scenario 2: Scale-Up (50-100 apps)**

**Needs:**
- Multiple environments (dev/staging/prod)
- Vulnerability scanning
- CI/CD integration

**Solution:**
- ACR Standard tier (€18.40/mo)
- Separate registries per environment
- Microsoft Defender enabled
- Webhooks for CI/CD notifications

---

### **Scenario 3: Enterprise (1000+ images)**

**Needs:**
- Global distribution
- Compliance requirements
- High availability
- Air-gapped production

**Solution:**
- ACR Premium tier (€92/mo) x regions
- Geo-replication (3 regions: West EU, North EU, UK)
- Private endpoints only
- Content trust enforced
- 1-year retention policy
- HSM-backed signing keys

**Total Cost:** €276/month (3 regions) + Defender

---

## 📊 Decision Framework: Do You Need ACR?

### **Use ACR If:**
- ✅ Running production workloads on Azure (AKS, ACI, Functions)
- ✅ Need private registry with Azure AD integration
- ✅ Require compliance features (signing, scanning)
- ✅ Multiple regions (geo-replication)
- ✅ Enterprise RBAC requirements

### **Skip ACR If:**
- ❌ Learning only (use local Podman for now)
- ❌ Budget-constrained (€55/year adds up)
- ❌ Using GitHub Actions (GitHub Container Registry is free)
- ❌ Simple single-app deployment
- ❌ Not yet in production phase

### **Alternative: GitHub Container Registry**

```bash
# Completely FREE alternative for your learning!

# 1. Create personal access token on GitHub
# Settings → Developer settings → Personal access tokens
# Scopes: write:packages, delete:packages

# 2. Login with Podman
echo $GITHUB_TOKEN | podman login ghcr.io -u YOUR_USERNAME --password-stdin

# 3. Tag your image
podman tag secure-app:1.0.0 ghcr.io/yomhubgg/secure-app:1.0.0

# 4. Push to GitHub
podman push ghcr.io/yomhubgg/secure-app:1.0.0

# 5. View in GitHub
# https://github.com/YomHubGG?tab=packages

# Benefits:
# ✅ FREE unlimited private images
# ✅ Integrated with GitHub Actions
# ✅ Good vulnerability scanning
# ✅ Perfect for portfolio projects
```

---

## 🎯 Today's Practical Exercise: ACR Security Audit

Let's create an ACR security checklist you can use:

### **Checklist: ACR Security Assessment**

```markdown
# ACR Security Audit Checklist

## Authentication & Authorization
- [ ] Admin user DISABLED
- [ ] Managed identities used for Azure services (AKS, ACI)
- [ ] Service principals have minimum required role (AcrPull/AcrPush)
- [ ] No hardcoded credentials in code
- [ ] Service principal credentials rotated every 90 days

## Network Security
- [ ] Public access restricted (IP allowlist or disabled)
- [ ] Private endpoint configured (if Premium tier)
- [ ] Virtual network rules implemented
- [ ] Firewall rules documented and reviewed quarterly
- [ ] Only trusted Azure services allowed bypass

## Image Security
- [ ] Vulnerability scanning enabled (Defender for Containers)
- [ ] Quarantine policy enabled for critical vulnerabilities
- [ ] Content trust enabled (image signing)
- [ ] Images scanned before production deployment
- [ ] Base images updated monthly

## Lifecycle Management
- [ ] Retention policy configured (untagged manifests)
- [ ] Production images locked (write/delete disabled)
- [ ] Image tags follow semantic versioning
- [ ] Old images archived or deleted per policy
- [ ] Compliance retention met (e.g., 1-year audit logs)

## Monitoring & Compliance
- [ ] Activity logs enabled and sent to Log Analytics
- [ ] Alerts configured for suspicious activities
- [ ] Regular security reviews conducted
- [ ] Compliance requirements documented
- [ ] Disaster recovery plan documented

## Cost Optimization
- [ ] Appropriate tier selected (Basic/Standard/Premium)
- [ ] Geo-replication justified and documented
- [ ] Storage usage monitored
- [ ] Unused images cleaned up
- [ ] Cost alerts configured

## Score: ___/26 checks passed
```

---

## 🚀 Next Steps

### **Option A: Continue Theory-Only (Recommended)**
- ✅ Completed Day 37 understanding
- ✅ Ready for Day 39 (ACI deployment with existing container)
- ✅ Total cost: €0
- ✅ Can implement ACR later when needed

### **Option B: Deploy ACR Basic (Hands-On)**
- Deploy ACR Basic tier (€4.60/month)
- Push your Day 35 container
- Practice real security configurations
- Cost: €4.60/month (€0.15/day)

### **Option C: Use GitHub Container Registry (FREE)**
- Push to ghcr.io instead
- Practice all concepts for €0
- Good for portfolio (public packages)
- Easy migration to ACR later

---

## 📚 Additional Resources

### **Microsoft Learn Modules**
- [Introduction to Azure Container Registry](https://learn.microsoft.com/azure/container-registry/)
- [ACR Security Best Practices](https://learn.microsoft.com/azure/container-registry/container-registry-best-practices)
- [Content Trust in ACR](https://learn.microsoft.com/azure/container-registry/container-registry-content-trust)

### **Documentation**
- [ACR Authentication](https://learn.microsoft.com/azure/container-registry/container-registry-authentication)
- [ACR Firewall Rules](https://learn.microsoft.com/azure/container-registry/container-registry-access-selected-networks)
- [ACR Webhooks](https://learn.microsoft.com/azure/container-registry/container-registry-webhook)

---

## ✅ Session Completion Criteria

You've completed Day 37 when you can:
- [ ] Explain ACR architecture and components
- [ ] Describe 4 authentication methods (ranked by security)
- [ ] List ACR security features (RBAC, scanning, signing, private endpoints)
- [ ] Explain content trust and how image signing works
- [ ] Understand vulnerability scanning and quarantine policies
- [ ] Compare ACR tiers and make cost-benefit decisions
- [ ] Identify when to use ACR vs alternatives (ghcr.io, Docker Hub)
- [ ] Complete the ACR security audit checklist

---

**Next Session**: Day 39 - Azure Container Instances (ACI)  
**Focus**: Deploy your Day 35 container to the cloud with full security!  
**Special**: Integrate your 42 minishell project 🎯

---

*Learning ACR security concepts = Job-ready knowledge*  
*No deployment needed to understand enterprise patterns!*  
*€0 spent today = €55 saved this year* 💰
