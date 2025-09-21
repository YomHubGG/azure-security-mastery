# 🔧 Day 19: Data Protection - Hands-On Lab

## 🎯 **Lab Objectives**
- Explore existing data protection features in your Azure environment
- Understand classification concepts through practical examples
- Learn to identify and protect sensitive data

## 📋 **Lab Steps**

### **Step 1: Inventory Your Data Assets**
```bash
# List all storage accounts (your data repositories)
az storage account list --query "[].{Name:name, Tier:sku.tier, Encryption:encryption.services.blob.enabled}" --output table

# Check Key Vaults (secrets and sensitive data storage)
az keyvault list --query "[].{Name:name, Location:location, SoftDelete:properties.enableSoftDelete}" --output table

# List container registries (application data)
az acr list --query "[].{Name:name, Tier:sku.tier, AdminEnabled:adminUserEnabled}" --output table
```

### **Step 2: Examine Data Protection Features**
```bash
# Check storage account encryption details
az storage account show --name "stlearning4uybw3c2lbkwm" --resource-group "rg-learning-day1" --query "encryption" --output json

# Verify Key Vault soft delete protection
az keyvault show --name "kvlearning4uybw3c2lbkwm" --query "{SoftDelete:properties.enableSoftDelete, PurgeProtection:properties.enablePurgeProtection}" --output table

# Check container registry security features
az acr show --name "devacr4uybw3c2lbkwm" --query "{AdminEnabled:adminUserEnabled, PublicNetwork:publicNetworkAccess}" --output table
```

### **Step 3: Data Classification Exercise**
Create sample files with different sensitivity levels:

```bash
# Create classification examples (local files for demonstration)
echo "Public: This is marketing material" > public-data.txt
echo "Internal: Employee handbook - internal use only" > internal-data.txt  
echo "Confidential: Customer database backup - restricted access" > confidential-data.txt
echo "Restricted: Encryption keys and security secrets" > restricted-data.txt

# Show file sizes (data governance consideration)
ls -la *-data.txt
```

### **Step 4: Data Governance Assessment**
```bash
# Check Azure Policy compliance (governance framework)
az policy assignment list --query "[].{Name:displayName, Scope:scope}" --output table

# List resource tags (data classification metadata)
az resource list --query "[?tags != null].{Name:name, Tags:tags}" --output table

# Check RBAC assignments (data access control)
az role assignment list --all --query "[].{Principal:principalName, Role:roleDefinitionName, Scope:scope}" --output table
```

## 📊 **Assessment Questions**

After running the commands, answer these:

1. **Data Discovery**: How many data storage resources do you have?
2. **Classification**: What types of data would you store in each resource?
3. **Protection**: Which resources have encryption enabled?
4. **Governance**: Are your resources properly tagged for classification?
5. **Access Control**: Who has access to your sensitive data stores?

## 🎯 **Key Learnings**

- ✅ **Data Inventory**: You have X storage accounts, Y Key Vaults, Z container registries
- ✅ **Encryption**: All storage has encryption enabled by default
- ✅ **Access Control**: RBAC controls who can access your data
- ✅ **Governance**: Tags and policies help classify and manage data
- ✅ **Soft Delete**: Key Vaults have data recovery protection

## 📝 **Next Steps for Data Protection**

1. **Implement tagging strategy** for data classification
2. **Set up Azure Policy** for data governance enforcement  
3. **Configure DLP policies** for sensitive data protection
4. **Enable audit logging** for data access monitoring
5. **Plan retention policies** for data lifecycle management

---

**Remember**: Real-world data protection requires Microsoft 365 E5 or specific compliance licenses for full Purview/DLP capabilities. Today we explored the Azure infrastructure foundation!