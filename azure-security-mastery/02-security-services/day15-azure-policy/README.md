# 📋 Day 15: Azure Policy & Governance

**Date**: September 16, 2025 (Scheduled)  
**Focus**: Azure Policy, governance, and compliance automation  
**Duration**: 1-2 hours  
**Session**: #8 (according to every-other-day schedule)

## 🎯 **Today's Learning Goals** 

- ✅ Create custom Azure Policies
- ✅ Set up Management Groups
- ✅ Implement resource tagging strategy
- ✅ Configure compliance reporting
- ✅ Commit: "Policy-driven governance"

## 📚 **What is Azure Policy?**

Azure Policy is a service that lets you create, assign, and manage policies to enforce rules over your Azure resources. Think of it as **automated governance** that ensures your cloud environment follows organizational standards.

### **Key Concepts**:
1. **Policy Definitions** - The rules you want to enforce
2. **Policy Assignments** - Applying policies to scopes
3. **Management Groups** - Hierarchical organization
4. **Compliance** - Tracking adherence to policies
5. **Remediation** - Automatic fixing of non-compliant resources

## 🏗️ **Today's Hands-On Lab**

### **Phase 1: Management Groups Setup**
```bash
# Create management group hierarchy
az account management-group create \
  --name "mg-security-learning" \
  --display-name "Security Learning Environment"

# Move subscription to management group
az account management-group subscription add \
  --name "mg-security-learning" \
  --subscription $(az account show --query id -o tsv)
```

### **Phase 2: Custom Policy Creation**
- Create policy to enforce specific SKUs
- Policy for required resource tags
- Policy for storage account encryption
- Policy for allowed resource locations

### **Phase 3: Resource Tagging Strategy**
```json
{
  "Environment": "Learning",
  "Project": "Azure-Security",
  "CostCenter": "Education",
  "Owner": "Student"
}
```

### **Phase 4: Compliance Monitoring**
- Set up compliance dashboard
- Configure compliance reporting
- Create alerts for policy violations

## 🎯 **Success Criteria**

By the end of Day 15:
- ✅ Management group structure created
- ✅ Custom policies defined and assigned
- ✅ Consistent tagging strategy implemented
- ✅ Compliance monitoring active
- ✅ Automated remediation configured

## 📊 **Governance Framework**

We'll establish:
- **Consistent resource naming**
- **Mandatory security settings**
- **Cost control policies**
- **Compliance tracking**
- **Automated enforcement**

## 💡 **Enterprise Governance Patterns**

This lab demonstrates:
- **Policy-as-Code approach**
- **Hierarchical governance structure**
- **Automated compliance checking**
- **Resource standardization**
- **Cost optimization through policy**

## 🔧 **Policy Examples We'll Create**

1. **Security Policies**:
   - Require HTTPS for storage accounts
   - Enforce Key Vault soft delete
   - Mandate encryption at rest

2. **Cost Control Policies**:
   - Limit VM SKU sizes
   - Restrict expensive resource types
   - Require cost center tags

3. **Compliance Policies**:
   - GDPR data residency
   - Required security configurations
   - Audit trail requirements

**Note**: This establishes the governance foundation for all future Azure work!