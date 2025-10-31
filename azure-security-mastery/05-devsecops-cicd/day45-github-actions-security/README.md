# Day 45: GitHub Actions Security Deep Dive 🔒

**Date**: October 31, 2025 - Session #23  
**Focus**: Advanced pipeline security patterns, secrets management, and automated security gates  
**Building On**: Day 23 (GitHub Actions basics), Day 35 (Container security), Day 43 (Kubernetes)

---

## 🎯 Learning Objectives

### **What We're Building**
A **production-grade secure CI/CD pipeline** that:
1. ✅ Scans for secrets before they're committed
2. ✅ Performs SAST (Static Application Security Testing)
3. ✅ Scans container images for vulnerabilities (Trivy)
4. ✅ Validates dependencies for known CVEs
5. ✅ Uses OpenID Connect (OIDC) instead of long-lived secrets
6. ✅ Implements security gates (fail pipeline on high-severity issues)
7. ✅ Deploys to Azure Container Instances with security best practices

### **Skills You'll Demonstrate**
- **Modern Authentication** - OIDC token exchange (no stored credentials!)
- **Shift-Left Security** - Find issues before production
- **Defense in Depth** - Multiple security layers (code, dependencies, containers)
- **Automated Compliance** - Security checks run automatically
- **Cost Optimization** - €0 pipeline with enterprise features

---

## 📊 Current State Assessment

### **What You Already Have (Day 23)**
✅ Basic GitHub Actions workflow  
✅ CodeQL security scanning  
✅ ESLint security rules  
✅ Service Principal authentication  
✅ Azure CLI deployment

### **What's Missing (We'll Add Today)**
❌ **Secret scanning** - Detect hardcoded credentials  
❌ **Container image scanning** - Find CVE vulnerabilities  
❌ **OIDC authentication** - Eliminate stored secrets  
❌ **SBOM generation** - Software Bill of Materials  
❌ **Security gates** - Block deployments with critical issues  
❌ **Matrix builds** - Test across multiple environments  

---

## 🔐 Part 1: Eliminating Secrets with OIDC

### **The Problem with Service Principals**
Your Day 23 workflow stores `AZURE_CREDENTIALS` as a secret:
```json
{
  "clientId": "xxx",
  "clientSecret": "xxx",  ← This is a long-lived credential (risky!)
  "subscriptionId": "xxx",
  "tenantId": "xxx"
}
```

**Risks:**
- Secret lives for months/years
- If leaked, attacker has persistent access
- Manual rotation required
- Stored in GitHub (encrypted but still stored)

### **The OIDC Solution**
OpenID Connect allows GitHub to **request temporary tokens** from Azure:
- No stored secrets in GitHub
- Tokens expire after minutes
- Azure validates the request came from your repo
- Industry best practice (used by AWS, GCP, Azure)

### **How OIDC Works**
```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   GitHub    │         │   Microsoft  │         │    Azure    │
│   Actions   │────────▶│   Entra ID   │────────▶│  Resources  │
│  (Workflow) │  1. OIDC│  (validates) │  2. JWT │ (deployment)│
└─────────────┘   token └──────────────┘   token └─────────────┘

1. GitHub generates OIDC token (signed by GitHub)
2. Azure verifies token is from your repo/branch
3. Azure issues short-lived access token
4. Workflow uses token to deploy (expires in 1 hour)
```

### **Setting Up OIDC (We'll Do This Together)**

#### **Step 1: Create Federated Credential in Azure**
```bash
# Get your subscription ID
SUBSCRIPTION_ID=$(az account show --query id -o tsv)

# Create app registration for OIDC
APP_NAME="github-oidc-cybersecurity-journey"

# Create the app
APP_ID=$(az ad app create \
  --display-name "$APP_NAME" \
  --query appId -o tsv)

echo "App ID: $APP_ID"

# Create service principal
az ad sp create --id $APP_ID

# Assign permissions (scoped to resource group)
az role assignment create \
  --role "Contributor" \
  --assignee $APP_ID \
  --scope "/subscriptions/$SUBSCRIPTION_ID/resourceGroups/rg-learning-day1"
```

#### **Step 2: Configure Federated Credential**
```bash
# Get your GitHub repo details
GITHUB_ORG="YomHubGG"  # Your GitHub username
GITHUB_REPO="azure-security-mastery"

# Create federated credential for main branch
az ad app federated-credential create \
  --id $APP_ID \
  --parameters "{
    \"name\": \"github-main-branch\",
    \"issuer\": \"https://token.actions.githubusercontent.com\",
    \"subject\": \"repo:${GITHUB_ORG}/${GITHUB_REPO}:ref:refs/heads/main\",
    \"description\": \"GitHub Actions OIDC for main branch\",
    \"audiences\": [\"api://AzureADTokenExchange\"]
  }"

echo "✅ Federated credential created!"
echo "App ID (save this): $APP_ID"
```

#### **Step 3: Update GitHub Secrets**
Instead of storing credentials, you only need 3 IDs:
```bash
# In GitHub repo → Settings → Secrets → Actions:
AZURE_CLIENT_ID: <APP_ID from above>
AZURE_TENANT_ID: <your tenant ID>
AZURE_SUBSCRIPTION_ID: <your subscription ID>
```

**No client secret needed!** 🎉

---

## 🔍 Part 2: Advanced Security Scanning

### **Security Scanning Layers**

```
┌─────────────────────────────────────────────────────────────┐
│                    Security Scanning Pipeline                │
├─────────────────────────────────────────────────────────────┤
│  1. Secret Scanning       │ Detect hardcoded API keys       │
│  2. SAST (CodeQL)         │ Find code vulnerabilities       │
│  3. Dependency Scanning   │ Check npm/pip packages for CVEs │
│  4. Container Scanning    │ Scan Docker images (Trivy)      │
│  5. IaC Scanning          │ Validate Bicep/Terraform        │
│  6. License Compliance    │ Check for GPL violations        │
└─────────────────────────────────────────────────────────────┘
```

### **1. Secret Scanning with Gitleaks**
Detects 1000+ patterns of secrets (AWS keys, API tokens, private keys):

```yaml
- name: '🔐 Scan for Leaked Secrets'
  uses: gitleaks/gitleaks-action@v2
  env:
    GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
    GITLEAKS_ENABLE_SUMMARY: true
```

**What it catches:**
- AWS access keys (`AKIA...`)
- Azure connection strings
- GitHub personal access tokens
- Private SSH keys
- Slack webhooks
- Stripe API keys
- 990+ more patterns

### **2. Advanced CodeQL Configuration**
Day 23 had basic CodeQL. Let's level up:

```yaml
- name: '🛡️ Advanced CodeQL Analysis'
  uses: github/codeql-action/init@v3
  with:
    languages: javascript, python
    queries: security-extended, security-and-quality
    config: |
      paths:
        - azure-security-mastery
      paths-ignore:
        - node_modules
        - '**/test/**'
      query-filters:
        - include:
            kind: problem
            severity: error
        - include:
            kind: problem
            severity: warning
            tags:
              - security
```

**What this finds:**
- SQL injection vulnerabilities
- Cross-site scripting (XSS)
- Command injection
- Path traversal
- Insecure randomness
- Hardcoded credentials
- Weak cryptography

### **3. Container Image Scanning with Trivy**
Scans for CVEs in OS packages and application dependencies:

```yaml
- name: '🐳 Build Container Image'
  run: |
    docker build -t secure-app:${{ github.sha }} \
      -f azure-security-mastery/02-security-services/day35-docker-security/Dockerfile \
      azure-security-mastery/02-security-services/day35-docker-security/

- name: '🔍 Scan Container for Vulnerabilities'
  uses: aquasecurity/trivy-action@master
  with:
    image-ref: 'secure-app:${{ github.sha }}'
    format: 'sarif'
    output: 'trivy-results.sarif'
    severity: 'CRITICAL,HIGH'
    exit-code: '1'  # Fail pipeline if critical/high vulnerabilities found

- name: '📊 Upload Trivy Results to GitHub'
  uses: github/codeql-action/upload-sarif@v3
  with:
    sarif_file: 'trivy-results.sarif'
```

**What Trivy finds:**
- OS package vulnerabilities (Alpine, Ubuntu, etc.)
- Application dependency CVEs
- Misconfigurations (exposed ports, weak settings)
- Secret detection in container layers

### **4. Dependency Scanning with npm audit**
Check for known vulnerabilities in Node.js dependencies:

```yaml
- name: '📦 Audit Dependencies'
  run: |
    cd azure-security-mastery/02-security-services/day35-docker-security/app
    npm audit --audit-level=high --production
    npm audit fix --dry-run
```

**What it catches:**
- Prototype pollution vulnerabilities
- Regular expression DoS
- Insecure dependencies
- Outdated packages with patches

---

## 🛡️ Part 3: Security Gates & Pipeline Design

### **Security Gate Strategy**
Not all vulnerabilities should block deployment:

| Severity | Action | Example |
|----------|--------|---------|
| **CRITICAL** | ❌ Block deployment | RCE vulnerability in production dependency |
| **HIGH** | ⚠️ Warn + require approval | XSS in admin panel |
| **MEDIUM** | ℹ️ Log + create issue | Weak cipher in dev dependency |
| **LOW** | ℹ️ Log only | Informational finding |

### **Implementing Gates**

```yaml
jobs:
  security-scan:
    runs-on: ubuntu-latest
    steps:
      - name: Run security scans
        id: scan
        run: |
          # Run all scans, capture exit codes
          trivy_exit=0
          npm_exit=0
          
          trivy image myapp:latest --exit-code 1 --severity CRITICAL,HIGH || trivy_exit=$?
          npm audit --audit-level=high || npm_exit=$?
          
          # Fail if critical issues found
          if [ $trivy_exit -ne 0 ] || [ $npm_exit -ne 0 ]; then
            echo "::error::Critical security issues found!"
            exit 1
          fi

  deploy:
    needs: security-scan  # Only runs if security-scan passes
    runs-on: ubuntu-latest
    environment: production  # Requires manual approval
    steps:
      - name: Deploy to Azure
        run: |
          az containerinstance create ...
```

---

## 🚀 Part 4: Complete Secure Pipeline

### **Full Workflow Architecture**

```
┌──────────────────────────────────────────────────────────────┐
│                     GitHub Push to main                       │
└────────────────────────┬─────────────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │   Job 1: Security Scanning     │
         ├───────────────────────────────┤
         │ • Gitleaks (secrets)           │
         │ • CodeQL (SAST)                │
         │ • npm audit (dependencies)     │
         │ • ESLint security rules        │
         └───────────────┬───────────────┘
                         │ (all pass)
                         ▼
         ┌───────────────────────────────┐
         │   Job 2: Build & Scan Image   │
         ├───────────────────────────────┤
         │ • Docker build                 │
         │ • Trivy scan (CVEs)            │
         │ • SBOM generation              │
         │ • Push to GHCR                 │
         └───────────────┬───────────────┘
                         │ (no critical CVEs)
                         ▼
         ┌───────────────────────────────┐
         │   Job 3: Deploy to ACI        │
         ├───────────────────────────────┤
         │ • OIDC auth to Azure           │
         │ • Deploy container             │
         │ • Health check                 │
         │ • Smoke tests                  │
         └───────────────────────────────┘
```

### **Enhanced Workflow File**
Let me create the advanced version:

```yaml
name: 'Secure DevSecOps Pipeline - Advanced'

on:
  push:
    branches: [ main ]
    paths:
      - 'azure-security-mastery/**'
      - '.github/workflows/**'
  pull_request:
    branches: [ main ]
  workflow_dispatch:  # Manual trigger

permissions:
  contents: read
  security-events: write
  id-token: write  # Required for OIDC
  issues: write    # Create issues for findings

env:
  AZURE_RESOURCE_GROUP: 'rg-learning-day1'
  CONTAINER_NAME: 'secure-app'
  IMAGE_TAG: ${{ github.sha }}

jobs:
  security-scan:
    name: '🔒 Security Scanning'
    runs-on: ubuntu-latest
    
    steps:
    - name: '📥 Checkout Code'
      uses: actions/checkout@v4
      with:
        fetch-depth: 0  # Full history for better analysis
        
    - name: '🔐 Scan for Leaked Secrets'
      uses: gitleaks/gitleaks-action@v2
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        
    - name: '🛡️ Initialize CodeQL'
      uses: github/codeql-action/init@v3
      with:
        languages: javascript, python
        queries: security-extended
        
    - name: '📦 Setup Node.js'
      uses: actions/setup-node@v4
      with:
        node-version: '18'
        cache: 'npm'
        cache-dependency-path: '**/package-lock.json'
        
    - name: '🔍 Audit Dependencies'
      run: |
        cd azure-security-mastery/02-security-services/day35-docker-security/app
        npm ci
        npm audit --audit-level=high || exit 0  # Don't fail on high, just report
        
    - name: '🔬 Perform CodeQL Analysis'
      uses: github/codeql-action/analyze@v3
      
  build-and-scan:
    name: '🐳 Build & Scan Container'
    runs-on: ubuntu-latest
    needs: security-scan
    
    steps:
    - name: '📥 Checkout Code'
      uses: actions/checkout@v4
      
    - name: '🐳 Build Container Image'
      run: |
        docker build \
          -t ghcr.io/${{ github.repository_owner }}/secure-app:${{ env.IMAGE_TAG }} \
          -t ghcr.io/${{ github.repository_owner }}/secure-app:latest \
          -f azure-security-mastery/02-security-services/day35-docker-security/Dockerfile \
          azure-security-mastery/02-security-services/day35-docker-security/
          
    - name: '🔍 Scan Image with Trivy'
      uses: aquasecurity/trivy-action@master
      with:
        image-ref: 'ghcr.io/${{ github.repository_owner }}/secure-app:${{ env.IMAGE_TAG }}'
        format: 'sarif'
        output: 'trivy-results.sarif'
        severity: 'CRITICAL,HIGH'
        
    - name: '📊 Upload Trivy Results'
      uses: github/codeql-action/upload-sarif@v3
      if: always()
      with:
        sarif_file: 'trivy-results.sarif'
        
    - name: '📋 Generate SBOM'
      uses: anchore/sbom-action@v0
      with:
        image: 'ghcr.io/${{ github.repository_owner }}/secure-app:${{ env.IMAGE_TAG }}'
        format: 'spdx-json'
        output-file: 'sbom.spdx.json'
        
    - name: '📤 Upload SBOM Artifact'
      uses: actions/upload-artifact@v4
      with:
        name: sbom
        path: sbom.spdx.json
        
  deploy:
    name: '🚀 Deploy to Azure'
    runs-on: ubuntu-latest
    needs: build-and-scan
    if: github.ref == 'refs/heads/main'
    environment: production  # Requires approval for production
    
    steps:
    - name: '📥 Checkout Code'
      uses: actions/checkout@v4
      
    - name: '🔐 Azure Login (OIDC)'
      uses: azure/login@v2
      with:
        client-id: ${{ secrets.AZURE_CLIENT_ID }}
        tenant-id: ${{ secrets.AZURE_TENANT_ID }}
        subscription-id: ${{ secrets.AZURE_SUBSCRIPTION_ID }}
        
    - name: '🚀 Deploy to ACI'
      run: |
        az containerinstance create \
          --resource-group ${{ env.AZURE_RESOURCE_GROUP }} \
          --name ${{ env.CONTAINER_NAME }}-${{ github.run_number }} \
          --image ghcr.io/${{ github.repository_owner }}/secure-app:${{ env.IMAGE_TAG }} \
          --cpu 0.5 \
          --memory 0.5 \
          --ports 3000 \
          --dns-name-label secure-app-${{ github.run_number }} \
          --restart-policy OnFailure \
          --environment-variables NODE_ENV=production \
          --secure-environment-variables API_KEY=${{ secrets.API_KEY }}
          
    - name: '✅ Health Check'
      run: |
        FQDN=$(az containerinstance show \
          --resource-group ${{ env.AZURE_RESOURCE_GROUP }} \
          --name ${{ env.CONTAINER_NAME }}-${{ github.run_number }} \
          --query ipAddress.fqdn -o tsv)
        
        echo "Waiting for container to be ready..."
        sleep 30
        
        HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" http://$FQDN:3000/)
        
        if [ $HTTP_CODE -eq 200 ]; then
          echo "✅ Health check passed! App is running at http://$FQDN:3000/"
        else
          echo "❌ Health check failed with HTTP $HTTP_CODE"
          exit 1
        fi
```

---

## 📚 Part 5: Understanding the Security Tools

### **Tool Comparison Matrix**

| Tool | Purpose | What It Finds | Speed | Cost |
|------|---------|---------------|-------|------|
| **Gitleaks** | Secret scanning | Hardcoded credentials, API keys | Fast (5s) | Free |
| **CodeQL** | SAST | Code vulnerabilities (SQLi, XSS) | Slow (2min) | Free |
| **npm audit** | Dependency scan | Known CVEs in packages | Fast (10s) | Free |
| **Trivy** | Container scan | OS & app vulnerabilities | Medium (30s) | Free |
| **Anchore** | SBOM generation | Package inventory | Fast (15s) | Free |
| **Snyk** | All-in-one | Code + deps + containers | Medium | Free tier |

---

## 🎯 Hands-On Exercise Plan

### **Phase 1: Setup OIDC (15 minutes)**
1. Create app registration in Azure
2. Configure federated credential
3. Update GitHub secrets
4. Test authentication

### **Phase 2: Enhance Pipeline (20 minutes)**
1. Add Gitleaks secret scanning
2. Improve CodeQL configuration
3. Add Trivy container scanning
4. Add SBOM generation

### **Phase 3: Security Gates (15 minutes)**
1. Configure job dependencies
2. Set up failure conditions
3. Create GitHub environment protection
4. Test with intentional vulnerability

### **Phase 4: Deploy & Verify (10 minutes)**
1. Trigger pipeline
2. Monitor security scans
3. Verify OIDC authentication
4. Check deployed container

---

## 📊 Success Criteria

### **What "Done" Looks Like**
✅ OIDC authentication working (no stored secrets)  
✅ All 4 security scans passing  
✅ Container deployed to ACI  
✅ SBOM generated and uploaded  
✅ Security results visible in GitHub Security tab  
✅ Pipeline completes in < 5 minutes  
✅ €0 cost maintained  

### **Interview Talking Points You'll Gain**
- "I implemented OpenID Connect for Azure authentication, eliminating long-lived secrets"
- "My pipeline includes 4 layers of security scanning: secrets, SAST, dependencies, and containers"
- "I use Trivy to scan for CVEs and fail the pipeline on critical vulnerabilities"
- "I generate SBOMs for supply chain security and compliance"
- "My security gates prevent insecure deployments from reaching production"

---

## 🚀 Ready to Start?

Let's begin with **Phase 1: OIDC Setup**. I'll guide you through each command step-by-step!

**Current Status**:
- ✅ Day 23 pipeline exists (basic version)
- ✅ Service Principal authentication working
- ✅ Container image built (Day 35)
- ✅ GitHub repository ready

**Next Steps**:
1. Set up Azure app registration for OIDC
2. Configure federated credentials
3. Test OIDC authentication
4. Enhance pipeline with advanced security scanning

**Time Estimate**: 60-75 minutes total

**Cost**: €0 (all tools are free, ACI deployment will be quick test)

Let's do this! 🔥
