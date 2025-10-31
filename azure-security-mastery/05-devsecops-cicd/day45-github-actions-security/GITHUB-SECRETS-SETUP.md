# Day 45: GitHub Secrets Setup Guide

**Task**: Add 3 secrets to GitHub for OIDC authentication

---

## 🔐 Step 1: Navigate to Secrets Page

Go to: https://github.com/YomHubGG/azure-security-mastery/settings/secrets/actions

Or manually:
1. Open your repository: https://github.com/YomHubGG/azure-security-mastery
2. Click **Settings** tab
3. Click **Secrets and variables** → **Actions** (left sidebar)
4. Click **New repository secret** button

---

## ✅ Step 2: Add These 3 Secrets

### Secret 1: AZURE_CLIENT_ID
```
Name: AZURE_CLIENT_ID
Value: 1eba6d9f-ceb6-4101-adc6-d86d2142fd43
```

### Secret 2: AZURE_TENANT_ID
```
Name: AZURE_TENANT_ID
Value: 286ee762-df63-4515-be88-d6a2032dfe6f
```

### Secret 3: AZURE_SUBSCRIPTION_ID
```
Name: AZURE_SUBSCRIPTION_ID
Value: a174b81f-93c2-4089-afa0-6f82a5165c86
```

---

## 🎉 What You Just Accomplished

### **Security Improvement:**
- ❌ **Old way (Day 23)**: Stored client secret (long-lived credential)
- ✅ **New way (Day 45)**: OIDC tokens (expire in 1 hour, auto-rotated)

### **How It Works:**
```
GitHub Actions Workflow
        ↓
Request OIDC token from GitHub
        ↓
Send token to Azure (with CLIENT_ID, TENANT_ID, SUBSCRIPTION_ID)
        ↓
Azure validates:
  - Token signed by GitHub ✓
  - Repo matches federated credential ✓
  - Branch matches (main) ✓
        ↓
Azure issues short-lived access token (1 hour)
        ↓
Workflow uses token to deploy
```

**No client secret stored anywhere!** 🎉

---

## 🧪 Step 3: Test the Pipeline

After adding secrets + completing role assignment in Azure Portal:

### Option A: Push to Main (Auto-trigger)
```bash
cd /home/yom/cybersecurity-journey
git add .github/workflows/day45-secure-pipeline.yml azure-security-mastery/05-devsecops-cicd/day45-github-actions-security/
git commit -m "Day 45: Advanced GitHub Actions security pipeline with OIDC"
git push origin main
```

### Option B: Manual Trigger
1. Go to: https://github.com/YomHubGG/azure-security-mastery/actions
2. Click **"Secure DevSecOps Pipeline - Day 45 Advanced"**
3. Click **"Run workflow"** → **"Run workflow"**

---

## 📊 What to Expect

### Pipeline Stages:
1. **🔒 Security Scanning** (~2 minutes)
   - Gitleaks: Scan for leaked secrets
   - CodeQL: Static code analysis
   - npm audit: Dependency vulnerabilities
   
2. **🐳 Build & Scan Container** (~3 minutes)
   - Docker build with caching
   - Trivy: Container vulnerability scan
   - SBOM generation (Software Bill of Materials)
   
3. **🚀 Deploy to Azure** (~2 minutes)
   - OIDC authentication (no secrets!)
   - Deploy to Azure Container Instances
   - Health check
   
4. **📊 Security Summary** (~10 seconds)
   - Consolidated security report

**Total time**: ~5-7 minutes

---

## ✅ Success Indicators

### Check these after pipeline runs:

1. **GitHub Actions Tab**
   - All jobs show green checkmarks ✅
   - No authentication errors

2. **Security Tab**
   - Go to: https://github.com/YomHubGG/azure-security-mastery/security
   - Should see CodeQL and Trivy results

3. **Artifacts**
   - SBOM generated (`sbom-<sha>.json`)
   - npm audit results
   - Deployment info

4. **Azure Container Instance**
   - Check: https://portal.azure.com → rg-learning-day1
   - Should see new container: `secure-app-<run-number>`
   - Status: Running
   - Can visit: `http://secure-app-day45-<run-number>.westeurope.azurecontainer.io:3000/`

---

## 🧹 Cleanup After Testing

Don't forget to delete the test container to avoid costs:

```bash
# List containers
az containerinstance list --resource-group rg-learning-day1 --output table

# Delete specific container (replace <run-number>)
az containerinstance delete \
  --resource-group rg-learning-day1 \
  --name secure-app-<run-number> \
  --yes
```

**Or via Portal:**
1. Go to rg-learning-day1
2. Find `secure-app-<run-number>`
3. Click → Delete → Confirm

---

## 🎯 Interview Talking Points

After completing this, you can say:

> "I implemented a production-grade CI/CD pipeline with 5 security layers:
> 
> 1. **Secret scanning** with Gitleaks to catch hardcoded credentials
> 2. **SAST** with CodeQL for code vulnerabilities  
> 3. **Dependency scanning** with npm audit for known CVEs
> 4. **Container scanning** with Trivy before deployment
> 5. **SBOM generation** for supply chain security
> 
> For authentication, I eliminated stored secrets by implementing OpenID Connect (OIDC), where GitHub requests temporary tokens from Azure that expire after 1 hour. This follows the principle of least privilege and is the industry best practice used by AWS, GCP, and Azure.
> 
> The entire pipeline runs for €0 using GitHub Actions free tier, with deployments to Azure Container Instances only lasting minutes for testing."

---

## 📚 Next Steps After Day 45

- **Day 47**: Infrastructure security scanning (Bicep/Terraform with Checkov)
- **Day 49**: Supply chain security deep dive (signed commits, provenance)
- **Day 51**: Security monitoring and alerting

---

**Status**: Ready to add secrets and run pipeline! 🚀
