# 🔒 Day 19: Data Classification & Protection

**Date**: September 21, 2025 (ACTIVE SESSION)  
**Focus**: Microsoft Purview and data protection strategies  
**Duration**: 1-2 hours  
**Session**: #10 (according to every-other-day schedule)

## 🎯 **Today's Learning Goals** 

- ✅ Understand data classification fundamentals
- ✅ Explore Microsoft Purview capabilities  
- ✅ Learn Azure Information Protection concepts
- ✅ Master data governance CLI commands
- ✅ Create data protection strategy framework

## 📚 **Data Protection Theory Deep Dive**

### **What is Data Classification?**
Data classification is the process of organizing data by relevant categories so it can be used and protected more efficiently. Think of it as **digital filing system with security labels**.

### **The Data Protection Hierarchy** 🏗️
```
1. DATA DISCOVERY → Find all data across your organization
2. DATA CLASSIFICATION → Label data by sensitivity (Public, Internal, Confidential, Restricted)  
3. DATA PROTECTION → Apply security controls based on classification
4. DATA GOVERNANCE → Monitor, audit, and maintain compliance
5. DATA LIFECYCLE → Manage retention, deletion, and archival
```

### **Microsoft Purview: The Control Center** 🎮
Microsoft Purview is Azure's **unified data governance platform**. It's like having a **security manager for all your data**.

**Core Components**:
- **Data Map**: Automated discovery of data across your estate
- **Data Catalog**: Searchable inventory of all data assets  
- **Data Insights**: Analytics about your data landscape
- **Data Policy**: Automated governance and access control
- **Data Lineage**: Track how data flows through systems

### **Azure Information Protection (AIP)** 🏷️
AIP provides **data classification and labeling** capabilities:

**Sensitivity Labels**:
- 🟢 **Public**: No restrictions (marketing materials, public docs)
- 🟡 **Internal**: Company-only (internal memos, policies)  
- 🟠 **Confidential**: Restricted access (financial data, customer info)
- 🔴 **Highly Confidential**: Maximum protection (trade secrets, legal docs)

**Protection Actions**:
- **Encryption**: Automatic encryption based on label
- **Access Controls**: Who can view, edit, or share
- **Watermarking**: Visual marking of protected documents
- **Tracking**: Monitor how protected content is used

## 🏗️ **Today's Focus Areas**

### **Phase 1: Data Classification**
- Understand data types and sensitivity levels
- Learn classification schemas (Public, Internal, Confidential, Restricted)
- Explore automated classification capabilities

### **Phase 2: Microsoft Purview Overview**
- Data discovery and cataloging
- Data lineage and impact analysis
- Compliance and governance features

### **Phase 3: Protection Policies**
- Information protection labels
- DLP policy creation
- Encryption strategies

## 💻 **CLI Commands for Data Protection**

### **Azure Purview Account Management** (Enterprise Feature - Not Available in Free Tier)
```bash
# Check if Purview is available in your subscription
az purview account list --output table
# Expected result: Empty (Purview requires paid enterprise licenses)

# Alternative: Explore data governance with Azure Policy instead
az policy definition list --query "[?contains(displayName, 'data') || contains(displayName, 'governance')].{Name:displayName, Type:policyType}" --output table

# Check what data sources you actually have for governance
az resource list --query "[?type=='Microsoft.Storage/storageAccounts' || type=='Microsoft.KeyVault/vaults'].{Name:name, Type:type, Location:location}" --output table
```

### **Azure Information Protection & Sensitivity Labels**
```bash
# Check if Information Protection is available in your tenant
az rest --method GET --url "https://graph.microsoft.com/v1.0/informationProtection/policy/labels" --headers "Content-Type=application/json"

# List existing sensitivity labels (requires Microsoft Graph permissions)
az rest --method GET --url "https://graph.microsoft.com/beta/security/informationProtection/sensitivityLabels" --headers "Content-Type=application/json"
```

### **Data Loss Prevention (DLP) Policies**
```bash
# Check Office 365 Security & Compliance status
az rest --method GET --url "https://graph.microsoft.com/v1.0/security/securityActions" --headers "Content-Type=application/json"

# List DLP policies (requires Security & Compliance permissions)
az rest --method GET --url "https://graph.microsoft.com/beta/security/informationProtection/dataLossPreventionPolicies" --headers "Content-Type=application/json"
```

### **Azure Storage Data Protection Features**
```bash
# Check blob storage encryption status
az storage account show --name "your-storage-account" --resource-group "rg-learning-day1" --query "encryption" --output table

# Enable blob soft delete (data protection feature)
az storage blob service-properties delete-policy update --account-name "your-storage-account" --enable true --days-retained 30

# Check soft delete configuration
az storage blob service-properties delete-policy show --account-name "your-storage-account" --output table

# Enable blob versioning for data protection
az storage blob service-properties update --account-name "your-storage-account" --enable-versioning true
```

### **Cost and Billing Monitoring** 💰
```bash
# Check current spending (important after upgrade email!)
az consumption usage list --top 5 --output table

# List all resources and their costs
az consumption usage list --include-additional-properties --include-meter-details --output table

# Check free tier usage
az consumption reservation summary list --grain monthly --output table
```

### **Azure Policy for Data Governance**
```bash
# List data protection related policies
az policy definition list --query "[?contains(displayName, 'data') || contains(displayName, 'encrypt')].{Name:displayName, Type:policyType}" --output table

# Check which policies are assigned to your subscription
az policy assignment list --output table

# Show details of encryption-related policy
az policy definition show --name "encryption-policy-id" --output yaml
```

## 🛠️ **Prerequisites**
- Completed Days 1-17 (security fundamentals)
- Understanding of data types in your organization
- Access to Microsoft 365 admin center (if available)

## 📋 **Learning Resources**

### **Microsoft Learn Paths**:
- [Describe capabilities of Microsoft compliance solutions](https://learn.microsoft.com/en-us/training/paths/describe-capabilities-of-microsoft-compliance-solutions/)
- [Implement information protection with Microsoft Purview](https://learn.microsoft.com/en-us/training/paths/purview-implement-information-protection/)

### **Hands-On Labs**:
- Data classification exercise ✅ COMPLETED
- CLI commands for data governance ✅ COMPLETED  
- Practical tagging and policy assessment ✅ COMPLETED

## 🎯 **Expected Outcomes** ✅ ACHIEVED

By the end of this session, you understand:
- ✅ How to classify and protect sensitive data (Public → Internal → Confidential → Highly Confidential)
- ✅ Microsoft Purview's role in data governance (enterprise-level data discovery and protection)
- ✅ Practical data governance using Azure Policy, tags, and RBAC (free-tier alternative)
- ✅ Data protection best practices (encryption, soft delete, access control)
- ✅ Real-world constraints (Purview requires expensive enterprise licenses)

## 📊 **Assessment** ✅ COMPLETED

**Knowledge Check**:
- ✅ Can you identify different data classification levels? (4 levels: Public, Internal, Confidential, Highly Confidential)
- ✅ Do you understand Azure data governance components? (Policy, RBAC, encryption, auditing)
- ✅ Can you explain practical vs enterprise approaches? (Free Azure tools vs expensive Purview)

**Practical Skills**:
- ✅ Assessed your actual Azure data estate (10 resources: 7 storage + 3 key vaults)
- ✅ Learned CLI commands for data discovery and governance
- ✅ Understood real-world Azure service limitations and alternatives

---

**Next Session**: Day 21 - Application Security (September 22, 2025)