# 📋 Day 1: Azure Foundation Setup Checklist

**Date**: August 28, 2025 (Wednesday)
**Time Allocated**: 2 hours
**Goal**: Complete Azure environment setup and first deployment

## 🎯 **Phase 1: Azure Account (30 minutes)**

### **Step 1: Create Azure Free Account** ⏱️ 15 min
- [ ] Visit: https://azure.microsoft.com/free/
- [ ] Sign up with personal Microsoft account
- [ ] Verify email and phone number
- [ ] Complete identity verification
- [ ] **Note**: €170 free credits + 12 months free services

### **Step 2: Initial Security Setup** ⏱️ 15 min
- [ ] Enable MFA on Azure account
- [ ] Set up Azure Active Directory tenant
- [ ] Create admin user (never use root account)
- [ ] Configure security defaults
- [ ] Set up billing alerts (€5, €25, €50)

## 🎯 **Phase 2: Local Environment Test (20 minutes)**

### **Step 3: Azure CLI Authentication** ⏱️ 10 min ✅
```bash
# Login to Azure
az login

# Verify account
az account show

# List available subscriptions
az account list --output table
```

### **Step 4: PowerShell Azure Connection** ⏱️ 10 min
```powershell
# Start PowerShell
pwsh

# Connect to Azure
Connect-AzAccount

# Verify connection
Get-AzSubscription
```

## 🎯 **Phase 3: First Infrastructure (45 minutes)**

### **Step 5: Resource Group Creation** ⏱️ 10 min ✅
```bash
# Create resource group in West Europe
az group create --name rg-learning-day1 --location westeurope

# Verify creation
az group show --name rg-learning-day1 --output table
```

### **Step 6: First Bicep Template** ⏱️ 25 min ✅

**Create**: `storage-account.bicep`
```bicep
// Parameters
param storageAccountName string = 'stlearning${uniqueString(resourceGroup().id)}'
param location string = resourceGroup().location

// Storage account resource
resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  sku: {
    name: 'Standard_LRS'
  }
  kind: 'StorageV2'
  properties: {
    accessTier: 'Hot'
    supportsHttpsTrafficOnly: true
    minimumTlsVersion: 'TLS1_2'
    allowBlobPublicAccess: false
  }
}

// Output
output storageAccountName string = storageAccount.name
output storageAccountId string = storageAccount.id
```

### **Step 7: Deploy Infrastructure** ⏱️ 10 min ✅
```bash
# Deploy Bicep template
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file storage-account.bicep \
  --parameters storageAccountName=stlearningday1

# Verify deployment
az storage account list --resource-group rg-learning-day1 --output table
```

## 🎯 **Phase 4: Documentation & Git (25 minutes)**

### **Step 8: GitHub Repository** ⏱️ 15 min ✅
- [x] Create GitHub repository: `azure-security-mastery`
- [x] Clone locally to cybersecurity-journey folder
- [x] Add initial README with project structure
- [x] Add .gitignore for Azure/Bicep projects

### **Step 9: First Commit** ⏱️ 10 min ✅
```bash
# Add files
git add .
git commit -m "Day 1: Azure foundation setup complete

- Azure free account created and configured
- MFA and security defaults enabled
- First Bicep template deployed (secure storage account)
- Resource group and basic infrastructure established

Tools verified:
- Azure CLI authenticated
- PowerShell Az modules working
- Bicep templates deploying successfully

Next: Day 3 - Network security architecture"

# Push to GitHub
git push origin main
```

## ✅ **Success Criteria**

At the end of Day 1, you should have:
- [x] **Working Azure subscription** with free credits
- [x] **Secure account setup** with MFA enabled
- [x] **Resource group created** in West Europe
- [x] **Storage account deployed** using Bicep IaC
- [x] **GitHub repository** with first commit
- [x] **Development environment** fully functional
- [x] **Confidence** in Azure CLI, PowerShell, and Bicep

## 📝 **Learning Log Entry**

**What I learned today**:
- tenants
- Bicep config // Infrastructure as code
-  // resources

**Challenges encountered**:
- 
- 

**Solutions found**:
- 
- 

**Tomorrow I will**:
- 
- 

**Confidence level (1-10)**: ___

## 🚀 **Day 3 Preview**

Next session focus:
- Azure Virtual Networks and Network Security Groups
- Azure Bastion for secure access
- Network security best practices
- More advanced Bicep templates

---

*Remember: This is Day 1 of your journey to €70K-120K Azure Security Engineer role!* 💪
