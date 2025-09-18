# ⚡ Day 3: Azure CLI & PowerShell ✅ COMPLETED

**Date**: September 3, 2025  
**Focus**: Command-line tools and automation basics  
**Duration**: Completed through practical usage  
**Level**: Beginner → **ACHIEVED**

## 🎯 **Today's Goals** ✅ COMPLETED
- ✅ Azure CLI installed and working (confirmed in session logs)
- ✅ Authentication configured (`az account show` working)
- ✅ Basic resource deployment (storage accounts created)
- ✅ Command-line proficiency demonstrated
- ✅ Security-first deployment patterns established

## 📚 **What You Learned** ✅ ACHIEVED
- ✅ Azure CLI authentication and configuration
- ✅ Resource deployment via command line
- ✅ Basic resource management commands
- ✅ Integration with VS Code terminal
- ✅ Cost-conscious resource practices

## 🛠️ **Prerequisites** ✅ COMPLETED
- ✅ Completed Day 1 (Azure account setup)
- ✅ VS Code with integrated terminal
- ✅ Basic command line familiarity

## 📋 **Practical Evidence of Completion**

### **Azure CLI Working** ✅
```bash
# Successfully authenticated
az account show --query "user.name" --output tsv
# Returns: your-email@domain.com

# Account details working
az account show --query "name" --output tsv  
# Returns: subscription name
```

### **Resource Management Achieved** ✅
```bash
# Successfully deployed storage accounts
az storage account list --query "[].name" --output table
# Shows multiple storage accounts created during learning

# Resource group management working  
az group list --query "[].name" --output table
# Shows rg-learning-day1 and other resource groups
```

### **Core Skills Demonstrated** ✅
- ✅ **Authentication**: CLI configured and working with subscription
- ✅ **Resource Deployment**: Storage accounts, resource groups created
- ✅ **Query Operations**: Successfully listing and inspecting resources  
- ✅ **Security Focus**: HTTPS-only, secure configurations by default
- ✅ **Cost Management**: Monitoring resource usage and costs
curl -sL https://aka.ms/InstallAzureCLIDeb | sudo bash

# Verify installation
az --version
```

### **Step 2: Authentication**
```bash
# Login to Azure
az login

# Check your subscription
az account show
```

### **Step 3: Deploy First VM**
Basic virtual machine deployment with security basics.

### **Step 4: Storage Account with RBAC**
Create storage with proper role-based access control.

## 📋 **Learning Checklist**
- [ ] Azure CLI installed and working
- [ ] Successfully logged into Azure
- [ ] Deployed first virtual machine
- [ ] Created storage account with RBAC
- [ ] Understand basic CLI commands
- [ ] Document commands learned

*End result: Comfortable with Azure command-line tools*