# 🏗️ Day 5: Infrastructure as Code with ARM/Bicep

**Date**: TBD  
**Focus**: Infrastructure as Code fundamentals  
**Duration**: 1-2 hours  
**Level**: Beginner

## 🎯 **Today's Goals** (From 1-Year Plan)
- ✅ Install Bicep CLI
- ✅ Write first .bicep template (Storage Account)
- ✅ Learn deployment and validation
- ✅ Set up .gitignore for sensitive files
- ✅ Commit: "First Bicep infrastructure deployment"

## 📚 **What You'll Learn**
- What is Infrastructure as Code?
- Bicep vs ARM templates
- Write your first Bicep template
- Deploy infrastructure declaratively
- Version control best practices

## 🛠️ **Prerequisites**
- Completed Day 1 (Azure account)
- Completed Day 3 (Azure CLI)

## 📋 **Step-by-Step Lab**

### **Step 1: Install Bicep**
```bash
# Install Bicep CLI
az bicep install

# Verify installation
az bicep version
```

### **Step 2: First Bicep Template**
Create a simple storage account template:

```bicep
// storage.bicep
param storageAccountName string
param location string = resourceGroup().location

resource storageAccount 'Microsoft.Storage/storageAccounts@2023-01-01' = {
  name: storageAccountName
  location: location
  sku: {
    name: 'Standard_LRS'
  }
  kind: 'StorageV2'
  properties: {
    accessTier: 'Hot'
  }
}

output storageAccountId string = storageAccount.id
```

### **Step 3: Deploy Your Template**
```bash
# Create resource group
az group create --name rg-learning --location westeurope

# Deploy template
az deployment group create \
  --resource-group rg-learning \
  --template-file storage.bicep \
  --parameters storageAccountName=mystoragelearning123
```

### **Step 4: Version Control Setup**
Create .gitignore for Azure projects and commit your work.

## 📋 **Learning Checklist**
- [ ] Bicep CLI installed
- [ ] Understand IaC concepts
- [ ] Created first Bicep template
- [ ] Successfully deployed infrastructure
- [ ] Set up proper .gitignore
- [ ] Committed work to Git

*End result: Foundation in Infrastructure as Code*