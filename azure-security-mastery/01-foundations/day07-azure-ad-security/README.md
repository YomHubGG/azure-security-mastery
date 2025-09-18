# 🔐 Day 7: Azure Active Directory Deep Dive

**Date**: September 18, 2025  
**Focus**: Azure AD fundamentals and identity security  
**Duration**: 1-2 hours  
**Level**: Beginner fundamentals

## 🎯 **Today's Learning Goals** (From 1-Year Plan)

Following the official Day 7 curriculum:
- ✅ Configure custom roles and RBAC
- ✅ Set up Privileged Identity Management (PIM)  
- ✅ Implement Conditional Access policies
- ✅ Configure Identity Protection
- ✅ Commit: "Enterprise identity security baseline"

## 📚 **What is Azure Active Directory?**

Azure AD is Microsoft's cloud-based identity and access management service. Think of it as the **bouncer** for your Azure resources.

### **Key Concepts to Master Today**:
1. **Users and Groups** - Who can access what
2. **Roles** - What permissions people have
3. **RBAC** - Role-Based Access Control
4. **Conditional Access** - Smart security rules
5. **Identity Protection** - Detecting suspicious activity

## 🏗️ **Today's Hands-On Lab**

We'll build a **basic but secure** Azure AD setup step by step.

### **Step 1: Current Azure AD Status**
Let's see what we already have:

```bash
# Check current user and tenant
az account show --query "{subscription:name, user:user.name, tenant:tenantId}"

# List current role assignments
az role assignment list --assignee $(az account show --query user.name -o tsv) --output table
```

### **Step 2: Create Custom Role (RBAC Basics)**
We'll create a simple custom role for learning.

### **Step 3: Basic Conditional Access**
Set up a simple policy to understand the concept.

### **Step 4: Identity Protection Overview**
Explore the security features available.

## 📋 **Learning Checklist**

- [ ] Understand what Azure AD does
- [ ] Explore current tenant setup
- [ ] Create a custom role
- [ ] Understand RBAC concepts
- [ ] Set up basic Conditional Access
- [ ] Review Identity Protection features
- [ ] Document learnings

## 💡 **Key Takeaways for Today**

This is about **understanding fundamentals**, not building complex systems. We're laying the foundation for everything else.

## 🚀 **Ready to Start?**

Let's begin with understanding your current Azure AD setup and then build knowledge step by step.

---
*This is Day 7 of your 1-Year Azure Security Plan - taking it slow and steady!* 🌟