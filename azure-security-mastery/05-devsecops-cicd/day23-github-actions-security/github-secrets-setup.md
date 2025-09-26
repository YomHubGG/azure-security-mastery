# 🔐 GitHub Repository Secrets - Copy & Paste Guide

**Repository**: `YomHubGG/azure-security-mastery`  
**Location**: Settings → Secrets and variables → Actions → Repository secrets

---

## 🎯 **Required Secrets (3 Total)**

### **1. AZURE_CREDENTIALS**
```json
{
  "clientId": "e9e9016d-3ea7-43e5-9fbe-e14e926f6721",
  "clientSecret": "GENERATED_SECRET_FROM_AZURE_CLI",
  "subscriptionId": "a174b81f-93c2-4089-afa0-6f82a5165c86",
  "tenantId": "286ee762-df63-4515-be88-d6a2032dfe6f"
}
```

**🚨 ISSUE**: Service Principal was created but role assignment failed due to Azure CLI v2.45.0 bug.

**🔧 WORKAROUND OPTIONS**:

#### **Option A: Use Your Personal Credentials (Learning Environment)**
Since this is a learning repository, you can use your personal Azure credentials:

1. **Get your personal Azure credentials**:
```bash
# Run this in your terminal to get the format
az ad sp create-for-rbac --name "temp-github-learning" --role contributor --scopes "/subscriptions/a174b81f-93c2-4089-afa0-6f82a5165c86/resourceGroups/rg-learning-day1" --sdk-auth
```

2. **Copy the JSON output exactly** and paste it as the `AZURE_CREDENTIALS` secret value

#### **Option B: Manual Service Principal Fix**
1. Go to Azure Portal → Resource Groups → `rg-learning-day1`
2. Click "Access control (IAM)" → "Add role assignment"
3. Select Role: "Contributor"
4. Assign access to: "User, group, or service principal"
5. Select: `sp-github-actions-day23` (App ID: `e9e9016d-3ea7-43e5-9fbe-e14e926f6721`)
6. Then get the client secret with: `az ad sp credential reset --id e9e9016d-3ea7-43e5-9fbe-e14e926f6721`

---

### **2. KEY_VAULT_NAME**
```
kvlearning4uybw3c2lbkwm
```

**Simple text value** - no quotes, no JSON formatting needed.

---

### **3. SUBSCRIPTION_ID**
```
a174b81f-93c2-4089-afa0-6f82a5165c86
```

**Simple text value** - no quotes, no JSON formatting needed.

---

## 📍 **Where to Add These Secrets**

1. **Navigate to**: https://github.com/YomHubGG/azure-security-mastery
2. **Click**: Settings tab
3. **Click**: Secrets and variables → Actions
4. **Click**: "Repository secrets" tab
5. **Click**: "New repository secret" button

**For each secret**:
- **Name**: Enter the exact name (e.g., `AZURE_CREDENTIALS`)
- **Secret**: Paste the value exactly as shown above
- **Click**: "Add secret"

---

## 🛡️ **Security Validation**

### **✅ What's Secure**:
- **Key Vault name**: Public information, safe to store as secret
- **Subscription ID**: Public information within your tenant
- **Service Principal**: Limited to `rg-learning-day1` resource group only
- **GitHub Secrets**: Encrypted at rest, only accessible to workflow runs

### **🚨 What's Protected**:
- **Client Secret**: Never appears in logs or workflow output
- **Access Token**: Generated temporarily during workflow execution
- **Key Vault contents**: Accessed only through authenticated workflows

### **🎯 Best Practice Demonstrated**:
- **Principle of Least Privilege**: Service Principal scoped to single resource group
- **Separation of Concerns**: CI/CD identity separate from personal identity
- **Secret Rotation**: Service Principal credentials can be reset independently

---

## 🧪 **Test Your Setup**

After adding all 3 secrets, push any change to your repository to trigger the workflow:

```bash
# Make a small change to trigger the pipeline
echo "# Pipeline test" >> README.md
git add README.md
git commit -m "Test DevSecOps pipeline with secrets"
git push origin main
```

**Expected Result**: GitHub Actions should run successfully and access your Azure resources.

---

## 🔧 **Troubleshooting**

### **If AZURE_CREDENTIALS doesn't work**:
1. Verify JSON formatting is exact (no extra spaces/newlines)
2. Check that service principal has Contributor role on `rg-learning-day1`
3. Try Option A (personal credentials) for learning environment

### **If KEY_VAULT_NAME fails**:
1. Verify Key Vault exists: `az keyvault show --name kvlearning4uybw3c2lbkwm`
2. Check resource group: Should be in `rg-learning-day1`

### **If SUBSCRIPTION_ID is wrong**:
1. Double-check with: `az account show --query id --output tsv`
2. Ensure you're in the correct Azure subscription

---

**🎯 Ready to demonstrate enterprise DevSecOps security practices!**