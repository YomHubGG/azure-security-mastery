# GitHub Container Registry (ghcr.io) Quickstart

**Recommended Alternative to ACR for Learning & Portfolio**  
**Cost**: €0 (FREE unlimited private images)  
**Perfect For**: Students, portfolio projects, GitHub-centric workflows

---

## 🎯 Why GitHub Container Registry?

### Comparison with ACR

| Feature | GitHub CR | ACR Basic |
|---------|-----------|-----------|
| **Cost** | €0 (FREE) | €4.60/month |
| **Private Images** | Unlimited | 10 GB |
| **Public Images** | Unlimited | Not recommended |
| **Portfolio Visibility** | ✅ Shows in GitHub profile | ❌ Not visible |
| **CI/CD Integration** | Native GitHub Actions | Requires service principal |
| **Vulnerability Scanning** | ✅ Dependabot | ✅ Defender (extra cost) |
| **RBAC** | GitHub teams | Azure AD |
| **Best For** | Learning, OSS, portfolio | Azure production workloads |

### For Your Cybersecurity Journey
✅ **FREE** - Save €55/year for certifications  
✅ **Portfolio** - Employers can see your images  
✅ **GitHub Actions** - You learned this on Day 23!  
✅ **Transferable** - All ACR concepts still apply  
✅ **Migration** - Can move to ACR anytime  

---

## 🚀 Setup (5 Minutes)

### Step 1: Create Personal Access Token

1. Go to GitHub: https://github.com/settings/tokens
2. Click **"Generate new token (classic)"**
3. Name: `ghcr-podman-access`
4. Expiration: **90 days** (good security practice)
5. Select scopes:
   - ✅ `write:packages` (push images)
   - ✅ `read:packages` (pull images)
   - ✅ `delete:packages` (manage images)
6. Click **"Generate token"**
7. **COPY THE TOKEN IMMEDIATELY** (you won't see it again!)

### Step 2: Save Token Securely

```bash
# DO NOT save in plain text file!
# Use environment variable for this session:
export CR_PAT=YOUR_TOKEN_HERE

# Or use pass/gpg (more secure):
echo "YOUR_TOKEN_HERE" | pass insert github/cr-token

# Or save in dedicated file with strict permissions:
echo "YOUR_TOKEN_HERE" > ~/.github-cr-token
chmod 600 ~/.github-cr-token
```

### Step 3: Login with Podman

```bash
# Login to GitHub Container Registry
echo $CR_PAT | podman login ghcr.io -u YomHubGG --password-stdin

# Expected output:
# Login Succeeded!

# Verify login
cat ~/.config/containers/auth.json | jq '.auths."ghcr.io"'
```

---

## 📦 Push Your Day 35 Container

### Step 1: Tag Your Image

```bash
# Your existing image from Day 35
podman images | grep secure-app

# Tag for GitHub Container Registry
podman tag secure-app:1.0.0 ghcr.io/yomhubgg/secure-app:1.0.0

# Also tag as latest
podman tag secure-app:1.0.0 ghcr.io/yomhubgg/secure-app:latest

# Verify tags
podman images | grep ghcr.io
```

### Step 2: Push to GitHub

```bash
# Push versioned tag
podman push ghcr.io/yomhubgg/secure-app:1.0.0

# Push latest tag
podman push ghcr.io/yomhubgg/secure-app:latest

# Expected output:
# Getting image source signatures
# Copying blob...
# Copying config...
# Writing manifest to image destination
# Storing signatures
```

### Step 3: Verify on GitHub

1. Go to: https://github.com/YomHubGG?tab=packages
2. You should see: `secure-app` package
3. Click it to view details

---

## 🔒 Make Image Public or Private

### Set Visibility

```bash
# Option 1: Via GitHub Web UI
# 1. Go to package: https://github.com/users/YomHubGG/packages/container/secure-app
# 2. Click "Package settings"
# 3. Scroll to "Danger Zone"
# 4. Click "Change visibility"
# 5. Choose "Public" or "Private"

# Option 2: Via GitHub CLI (if installed)
gh api \
  --method PATCH \
  -H "Accept: application/vnd.github+json" \
  /user/packages/container/secure-app/visibility \
  -f visibility='public'
```

### Recommendation
- **Private**: For learning, personal projects
- **Public**: For portfolio showcase (employers can see!)

---

## 🎓 Portfolio Value: Make It Public!

### Why Public Images Are Great for Your Portfolio

```markdown
Your GitHub Profile (what employers see):
└── Packages (Containers)
    └── secure-app
        ├── Latest: ghcr.io/yomhubgg/secure-app:latest
        ├── Version: ghcr.io/yomhubgg/secure-app:1.0.0
        ├── Created: October 14, 2025
        ├── Size: 354 MB
        └── Vulnerabilities: 0 critical, 2 high, 5 medium
```

**Employers see**:
- ✅ You build production-grade containers
- ✅ You follow security best practices
- ✅ You use modern DevOps workflows
- ✅ Your images are scanned for vulnerabilities
- ✅ You version your releases properly

### Making It More Impressive

```bash
# Add description and README to your package
# 1. Go to package settings
# 2. Add description:
#    "Secure Node.js application with Express, Helmet, and multi-stage Docker build"
# 3. Link to repository:
#    https://github.com/YomHubGG/cybersecurity-journey
# 4. Add README with:
#    - What the app does
#    - Security features implemented
#    - How to run it
```

---

## 🔍 Vulnerability Scanning

### Automatic Scanning with Dependabot

GitHub automatically scans your container images!

```bash
# View vulnerabilities:
# 1. Go to your package
# 2. Click on a version (e.g., 1.0.0)
# 3. Scroll to "Vulnerabilities"
# 4. See CVE list with severity

# Example output you'll see:
# CVE-2024-1234 - CRITICAL - OpenSSL RCE
# CVE-2024-5678 - HIGH - Curl buffer overflow
# CVE-2024-9012 - MEDIUM - Node.js prototype pollution
```

### Fix Vulnerabilities

```bash
# Update your Dockerfile base image
# FROM node:18.17.1-alpine3.18  # Old
FROM node:18.19.0-alpine3.19    # New (patched)

# Rebuild and push
cd /path/to/day35-docker-security
podman build -f Dockerfile.secure -t secure-app:1.0.1 .
podman tag secure-app:1.0.1 ghcr.io/yomhubgg/secure-app:1.0.1
podman push ghcr.io/yomhubgg/secure-app:1.0.1

# GitHub re-scans automatically
# Check vulnerabilities again in 5-10 minutes
```

---

## 📊 Managing Your Images

### List All Your Images

```bash
# Via Podman (local)
podman images | grep ghcr.io

# Via GitHub CLI (remote)
gh api \
  -H "Accept: application/vnd.github+json" \
  /user/packages?package_type=container

# Via curl (remote)
curl -H "Authorization: Bearer $CR_PAT" \
     https://ghcr.io/v2/yomhubgg/secure-app/tags/list | jq '.'
```

### Delete Old Images

```bash
# List versions
# Go to: https://github.com/users/YomHubGG/packages/container/secure-app/versions

# Delete via web UI:
# 1. Click version to delete
# 2. Click "Delete version"
# 3. Confirm

# Or via API:
gh api \
  --method DELETE \
  /user/packages/container/secure-app/versions/VERSION_ID
```

### Pull Your Image (Test)

```bash
# Public images (anyone can pull)
podman pull ghcr.io/yomhubgg/secure-app:1.0.0

# Private images (need authentication)
echo $CR_PAT | podman login ghcr.io -u YomHubGG --password-stdin
podman pull ghcr.io/yomhubgg/secure-app:1.0.0

# Run it
podman run -d -p 3000:3000 --name test ghcr.io/yomhubgg/secure-app:1.0.0
curl http://localhost:3000
```

---

## 🔐 Security Best Practices

### 1. Token Management

```bash
# ❌ BAD: Store token in .bashrc
# export CR_PAT="ghp_xxxxxxxxxxxx"

# ✅ GOOD: Use temporary environment variable
# (expires when you close terminal)
export CR_PAT="ghp_xxxxxxxxxxxx"

# ✅ BETTER: Use password manager
pass insert github/cr-token
export CR_PAT=$(pass show github/cr-token)

# ✅ BEST: Rotate every 90 days
# Set expiration when creating token
# Calendar reminder to regenerate
```

### 2. Scope Limitation

```bash
# Only give necessary permissions:
# - write:packages (for pushing)
# - read:packages (for pulling)
# - delete:packages (for cleanup)

# DON'T give:
# - repo (full repository access)
# - admin:org (organization admin)
# - workflow (GitHub Actions modification)
```

### 3. Image Tagging Strategy

```bash
# ✅ GOOD: Semantic versioning + date
ghcr.io/yomhubgg/secure-app:1.0.0
ghcr.io/yomhubgg/secure-app:1.0.0-20251014
ghcr.io/yomhubgg/secure-app:latest

# ❌ BAD: Only "latest"
ghcr.io/yomhubgg/secure-app:latest
# (What if you need to rollback?)

# ❌ BAD: Random tags
ghcr.io/yomhubgg/secure-app:test
ghcr.io/yomhubgg/secure-app:final
ghcr.io/yomhubgg/secure-app:really-final
```

---

## 🎯 Integration with Your Day 23 Pipeline

Remember your GitHub Actions pipeline? Integrate container push!

### Update Your `.github/workflows/secure-devsecops-pipeline.yml`

```yaml
# Add this job after security scanning
deploy-container:
  name: 'Build and Push Container'
  runs-on: ubuntu-latest
  needs: [security-scan, infrastructure-security]
  if: github.ref == 'refs/heads/main'
  
  steps:
  - name: '🔍 Checkout Code'
    uses: actions/checkout@v4
    
  - name: '🐳 Login to GitHub Container Registry'
    uses: docker/login-action@v3
    with:
      registry: ghcr.io
      username: ${{ github.actor }}
      password: ${{ secrets.GITHUB_TOKEN }}
      
  - name: '🏗️ Build Container Image'
    working-directory: ./azure-security-mastery/02-security-services/day35-docker-security
    run: |
      docker build -f Dockerfile.secure -t ghcr.io/${{ github.repository_owner }}/secure-app:${{ github.sha }} .
      docker tag ghcr.io/${{ github.repository_owner }}/secure-app:${{ github.sha }} ghcr.io/${{ github.repository_owner }}/secure-app:latest
      
  - name: '📦 Push to Registry'
    run: |
      docker push ghcr.io/${{ github.repository_owner }}/secure-app:${{ github.sha }}
      docker push ghcr.io/${{ github.repository_owner }}/secure-app:latest
      
  - name: '📊 Report'
    run: |
      echo "✅ Container pushed successfully!" >> $GITHUB_STEP_SUMMARY
      echo "Image: ghcr.io/${{ github.repository_owner }}/secure-app:${{ github.sha }}" >> $GITHUB_STEP_SUMMARY
```

**Result**: Every push to main automatically builds and publishes your container!

---

## 📈 Portfolio Enhancement Tips

### 1. Add README.md to Package

Create a README that appears on your package page:

```markdown
# Secure Node.js Application

Production-ready container demonstrating enterprise security practices.

## Features
- ✅ Multi-stage Docker build (Alpine Linux, 354 MB)
- ✅ Non-root user (nodejs UID 1001)
- ✅ Security headers (Helmet.js)
- ✅ Rate limiting (brute-force protection)
- ✅ CORS configuration
- ✅ Compression middleware
- ✅ Health check endpoint
- ✅ Azure Key Vault integration ready

## Security Hardening
- Read-only filesystem (chmod 555)
- No shell in container
- Pinned base image versions
- Regular vulnerability scanning
- Zero CRITICAL vulnerabilities

## Usage
```bash
docker run -p 3000:3000 ghcr.io/yomhubgg/secure-app:latest
curl http://localhost:3000/health
```

## Part of Azure Security Journey
This container is part of a comprehensive Azure Security learning project.
View full journey: https://github.com/YomHubGG/cybersecurity-journey
```

### 2. Multiple Images (Show Range)

```bash
# Push multiple projects:
# - secure-app (from Day 35)
# - minishell (from Day 39 - coming soon!)
# - other 42 projects

# Example:
podman tag minishell:latest ghcr.io/yomhubgg/minishell:latest
podman push ghcr.io/yomhubgg/minishell:latest
```

### 3. Keep Images Updated

```bash
# Monthly maintenance:
# 1. Update base images in Dockerfiles
# 2. Rebuild images
# 3. Push new versions
# 4. Verify vulnerabilities resolved

# Script it:
#!/bin/bash
cd ~/cybersecurity-journey/azure-security-mastery/02-security-services/day35-docker-security
podman build -f Dockerfile.secure -t secure-app:$(date +%Y%m%d) .
podman tag secure-app:$(date +%Y%m%d) ghcr.io/yomhubgg/secure-app:$(date +%Y%m%d)
podman tag secure-app:$(date +%Y%m%d) ghcr.io/yomhubgg/secure-app:latest
podman push --all-tags ghcr.io/yomhubgg/secure-app
```

---

## 🆚 When to Migrate to ACR

You'll know it's time when:

### Migrate to ACR If:
1. **Production deployment on Azure** (AKS, ACI, Functions)
2. **Need Azure AD integration** (corporate requirement)
3. **Private endpoints required** (compliance, PCI-DSS)
4. **Geo-replication needed** (global users)
5. **Budget allows** (€4.60/month minimum)

### Stay on GitHub CR If:
1. **Learning/portfolio focus** (current stage!)
2. **GitHub-centric workflow**
3. **Budget-constrained** (€0 is unbeatable)
4. **Not deploying to Azure yet**
5. **Public portfolio showcase** (GitHub visibility)

### Easy Migration Process:
```bash
# Pull from GitHub
podman pull ghcr.io/yomhubgg/secure-app:1.0.0

# Login to ACR
az acr login --name yourACR

# Tag for ACR
podman tag ghcr.io/yomhubgg/secure-app:1.0.0 youracr.azurecr.io/secure-app:1.0.0

# Push to ACR
podman push youracr.azurecr.io/secure-app:1.0.0

# Done! All ACR security concepts learned still apply
```

---

## ✅ Quickstart Completion Checklist

- [ ] Created GitHub Personal Access Token
- [ ] Logged in to ghcr.io with Podman
- [ ] Tagged Day 35 image for GitHub
- [ ] Pushed image to ghcr.io
- [ ] Verified image on GitHub packages page
- [ ] Set visibility (public for portfolio)
- [ ] Added package description
- [ ] Checked vulnerability scan results
- [ ] (Optional) Integrated with Day 23 pipeline
- [ ] (Optional) Added README to package

---

## 🎉 Success!

You now have:
- ✅ Private container registry (FREE!)
- ✅ Portfolio-visible container images
- ✅ Automatic vulnerability scanning
- ✅ GitHub Actions integration ready
- ✅ All ACR concepts learned (transferable knowledge)
- ✅ €55/year saved for certifications!

**Next**: Day 39 - Deploy containers to Azure Container Instances! 🚀
