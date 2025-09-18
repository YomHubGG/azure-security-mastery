# ⚡ Day 3: Azure CLI & PowerShell

**Date**: TBD  
**Focus**: Command-line tools and automation basics  
**Duration**: 1-2 hours  
**Level**: Beginner

## 🎯 **Today's Goals** (From 1-Year Plan)
- ✅ Install Azure CLI and Azure PowerShell
- ✅ Configure authentication and profiles
- ✅ Deploy first Virtual Machine
- ✅ Create Storage Account with RBAC
- ✅ Commit: "Initial Azure setup and basic services"

## 📚 **What You'll Learn**
- Install and configure Azure CLI
- Basic Azure PowerShell commands
- Deploy your first VM
- Create storage with proper security
- Use command line for Azure management

## 🛠️ **Prerequisites**
- Completed Day 1 (Azure account setup)
- Basic command line familiarity

## 📋 **Step-by-Step Lab**

### **Step 1: Install Tools**
```bash
# Azure CLI installation (Linux)
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