# Day 45: OIDC Setup - Configuration Values

**Created**: October 31, 2025  
**Purpose**: GitHub Actions OIDC authentication to Azure (no stored secrets!)

---

## ✅ Azure App Registration Created

**App Name**: `github-oidc-cybersecurity-journey`  
**App ID (Client ID)**: `1eba6d9f-ceb6-4101-adc6-d86d2142fd43`  
**Service Principal Object ID**: `5f37f027-63a7-49f2-958f-50932f4aef38`  
**Tenant ID**: `286ee762-df63-4515-be88-d6a2032dfe6f`  
**Subscription ID**: `a174b81f-93c2-4089-afa0-6f82a5165c86`

---

## 🔐 Federated Credential Configured

**Credential Name**: `github-main-branch`  
**Issuer**: `https://token.actions.githubusercontent.com`  
**Subject**: `repo:YomHubGG/azure-security-mastery:ref:refs/heads/main`  
**Audiences**: `["api://AzureADTokenExchange"]`

**What this means:**
- GitHub Actions can request Azure tokens WITHOUT storing secrets
- Only workflows from your main branch in YomHubGG/azure-security-mastery can authenticate
- Tokens expire after 1 hour (vs service principal secrets that last months/years)
- If token is leaked, it's only valid for 1 hour from that specific workflow run

---

## 📝 GitHub Secrets to Configure

Go to: `https://github.com/YomHubGG/cybersecurity-journey/settings/secrets/actions`

Add these 3 secrets (no client secret needed!):

```
Name: AZURE_CLIENT_ID
Value: 1eba6d9f-ceb6-4101-adc6-d86d2142fd43

Name: AZURE_TENANT_ID  
Value: 286ee762-df63-4515-be88-d6a2032dfe6f

Name: AZURE_SUBSCRIPTION_ID
Value: a174b81f-93c2-4089-afa0-6f82a5165c86
```

---

## ⚠️ Role Assignment Note

**Status**: Needs to be completed via Azure Portal due to CLI bug

**Steps to complete**:
1. Go to Azure Portal → Resource Groups → rg-learning-day1
2. Click "Access control (IAM)" → "Add role assignment"
3. Select "Contributor" role
4. Click "Members" → "Select members"
5. Search for "github-oidc-cybersecurity-journey"
6. Select the app → "Review + assign"

**Alternative via CLI (if you update Azure CLI)**:
```bash
az upgrade  # Update to latest Azure CLI

# Then retry role assignment
az role assignment create \
  --role "Contributor" \
  --assignee 1eba6d9f-ceb6-4101-adc6-d86d2142fd43 \
  --scope "/subscriptions/a174b81f-93c2-4089-afa0-6f82a5165c86/resourceGroups/rg-learning-day1"
```

---

## 🧪 Testing OIDC Authentication

After adding GitHub secrets and completing role assignment, test with this workflow:

```yaml
name: Test OIDC Auth

on: workflow_dispatch

permissions:
  id-token: write  # Required for OIDC token
  contents: read

jobs:
  test-auth:
    runs-on: ubuntu-latest
    steps:
      - name: 'Azure Login via OIDC'
        uses: azure/login@v2
        with:
          client-id: ${{ secrets.AZURE_CLIENT_ID }}
          tenant-id: ${{ secrets.AZURE_TENANT_ID }}
          subscription-id: ${{ secrets.AZURE_SUBSCRIPTION_ID }}
          
      - name: 'Test Azure CLI'
        run: |
          az account show
          az group show --name rg-learning-day1
```

**Expected result**: Successful authentication without any stored client secret! 🎉

---

## 📊 Security Improvements

### **Before (Day 23 - Service Principal with Secret)**
```
❌ Long-lived secret stored in GitHub (valid for 2 years)
❌ If leaked, attacker has persistent access
❌ Manual rotation required
❌ Secret visible to anyone with repo admin access
```

### **After (Day 45 - OIDC)**
```
✅ No secrets stored in GitHub
✅ Tokens expire after 1 hour
✅ Automatic rotation (new token per workflow run)
✅ Azure validates request came from specific repo/branch
✅ Industry best practice (GitHub, AWS, GCP all support OIDC)
```

---

## 🎯 Next Steps

1. ✅ Azure app registration created
2. ✅ Federated credential configured
3. ⏳ Complete role assignment (Portal or updated CLI)
4. ⏳ Add 3 secrets to GitHub
5. ⏳ Test OIDC authentication
6. ⏳ Update existing pipeline to use OIDC
7. ⏳ Add advanced security scanning (Trivy, Gitleaks)

---

## 📚 Resources

- **Microsoft Docs**: [Use GitHub Actions to connect to Azure](https://learn.microsoft.com/en-us/azure/developer/github/connect-from-azure)
- **GitHub Docs**: [About security hardening with OpenID Connect](https://docs.github.com/en/actions/deployment/security-hardening-your-deployments/about-security-hardening-with-openid-connect)
- **OIDC Explained**: [OpenID Connect for CI/CD](https://www.youtube.com/watch?v=ZgaGJdiMqW0)

---

**Status**: Ready for role assignment + GitHub secrets configuration!
