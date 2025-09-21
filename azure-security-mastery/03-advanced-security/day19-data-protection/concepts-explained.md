# 📋 Data Protection Concepts - Practical Examples

## 🔒 **Soft Delete in Action**

### **Scenario**: Accidentally deleted customer database backup

**Without Soft Delete**:
```
Employee deletes backup file → File gone forever → Company loses critical data → Disaster! 💥
```

**With Soft Delete (90 days)**:
```
Employee deletes backup file → File marked as deleted → IT notices within 30 days → File recovered → Crisis avoided! ✅
```

**CLI Commands**:
```bash
# Enable soft delete on storage account
az storage blob service-properties delete-policy update --account-name "stlearning4uybw3c2lbkwm" --enable true --days-retained 30

# List soft-deleted blobs (if any existed)
az storage blob list --container-name "backups" --account-name "stlearning4uybw3c2lbkwm" --include deleted

# Recover a soft-deleted blob
az storage blob undelete --container-name "backups" --name "important-backup.sql" --account-name "stlearning4uybw3c2lbkwm"
```

## 🏷️ **SKU Selection Guide**

### **Storage Account SKUs**:

| SKU | Cost | Use Case | Your Usage |
|-----|------|----------|------------|
| **Standard_LRS** | 💰 Cheap | Development, non-critical | ✅ Perfect for learning |
| **Standard_ZRS** | 💰💰 Medium | Production, zone resilience | Overkill for labs |
| **Premium_LRS** | 💰💰💰 Expensive | High-performance apps | Too expensive for learning |

### **Key Vault SKUs**:
```bash
# Check your Key Vault SKU
az keyvault show --name "kvlearning4uybw3c2lbkwm" --query "properties.sku" --output table
```

## 📦 **Blob Classification Example**

### **Data Sensitivity Classification**:

```bash
# Public blobs (marketing materials)
Container: "public-content"
├── company-logo.png (Block Blob - Public)
├── product-brochure.pdf (Block Blob - Public)

# Internal blobs (employee resources)  
Container: "internal-docs"
├── employee-handbook.pdf (Block Blob - Internal)
├── policy-documents.docx (Block Blob - Internal)

# Confidential blobs (customer data)
Container: "customer-data" 
├── customer-database-backup.sql (Block Blob - Confidential)
├── financial-reports.xlsx (Block Blob - Confidential)

# Restricted blobs (security secrets)
Container: "security-vault"
├── encryption-keys.key (Block Blob - Restricted)  
├── certificates.pfx (Block Blob - Restricted)
```

### **Blob Security Features**:
```bash
# Encryption: All blobs encrypted automatically
# Access Control: Container-level and blob-level permissions
# Versioning: Keep multiple versions of files
# Immutable Storage: WORM (Write Once, Read Many) for compliance
```

## 🎯 **Key Takeaways**

1. **Soft Delete** = Safety net for accidents (like Ctrl+Z for the cloud)
2. **SKU** = Performance/cost tier (Economy vs Business vs First Class)  
3. **Blob** = Any file stored in Azure (photos, documents, backups, anything!)
4. **Data Classification** = Organizing by sensitivity (Public → Internal → Confidential → Restricted)
5. **Protection Layers** = Encryption + Access Control + Soft Delete + Versioning

---

**Remember**: In enterprise environments, you'd have hundreds of storage accounts with thousands of containers holding millions of blobs - all needing proper classification and protection!