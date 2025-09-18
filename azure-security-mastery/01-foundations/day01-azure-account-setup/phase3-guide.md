# 🏗️ Phase 3: Infrastructure Deployment Guide

## 🎯 **What We're Building Today**

```
YOUR TENANT (gminilaposte.onmicrosoft.com)
    │
    └── SUBSCRIPTION (Azure subscription 1)
            │
            └── 📁 RESOURCE GROUP (rg-learning-day1)
                    │
                    └── 💾 STORAGE ACCOUNT (stlearning...)
                            │
                            ├── 🔒 Security: HTTPS only
                            ├── 🔒 TLS 1.2 minimum
                            ├── 🔒 Public access: Disabled
                            └── 📍 Location: West Europe
```

## 📚 **Learning Objectives**

By the end of this phase, you'll understand:
- ✅ What resource groups are and why they matter
- ✅ How to organize Azure resources logically
- ✅ Infrastructure as Code (IaC) with Bicep
- ✅ Security best practices from day one
- ✅ How to deploy and verify infrastructure

## 🚀 **Let's Start Building!**

### **Step 5: Resource Group Creation**

**What is a Resource Group?**
Think of it as a **folder** for your Azure resources - but smarter:
- Groups related resources together
- Applies policies and permissions to everything inside
- Enables bulk operations (delete all, move all, etc.)
- Has a location (affects metadata, not necessarily resources)

**Why "rg-learning-day1"?**
- `rg-` = Standard naming prefix for resource groups
- `learning` = Project/purpose identifier  
- `day1` = Phase/version identifier

---
*Ready to create your first resource group? Let me know when you want to proceed!* 🚀
