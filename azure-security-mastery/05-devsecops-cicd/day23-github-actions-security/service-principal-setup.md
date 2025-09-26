# Service Principal Setup - Day 23 DevSecOps

## 🚨 Azure CLI Issue Encountered

**Problem**: Azure CLI version 2.45.0 has a bug preventing role assignment creation
**Error**: `'RoleAssignmentsOperations' object has no attribute 'config'`
**Impact**: Cannot create properly scoped service principal for GitHub Actions

## 🔧 Workaround Strategy

### **Option 1: Use Personal Access Token (Learning Environment)**
For learning purposes, we'll demonstrate DevSecOps principles using personal authentication:
- ✅ **Security Scanning** - Still fully functional
- ✅ **Secrets Management** - Key Vault integration works
- ✅ **Infrastructure Validation** - Bicep security checks operational
- ⚠️ **Production Note** - Would require proper service principal in real environment

### **Option 2: Manual Role Assignment via Azure Portal**
If needed for portfolio demonstration:
1. Navigate to Azure Portal → Resource Groups → rg-learning-day1
2. Access Control (IAM) → Add role assignment
3. Select "Contributor" role
4. Assign to service principal: `sp-github-actions-day23`
5. AppId: `e9e9016d-3ea7-43e5-9fbe-e14e926f6721`

## 📋 Service Principal Details (Created Successfully)

```json
{
  "displayName": "sp-github-actions-day23",
  "id": "ac05eeb3-d7dc-4212-88b5-99b28a859c24",
  "appId": "e9e9016d-3ea7-43e5-9fbe-e14e926f6721",
  "createdDateTime": "2025-09-26T08:49:02Z"
}
```

**Security Status**: 
- ✅ Service principal created with descriptive naming
- ❌ Role assignment failed due to CLI bug
- 🔧 Workaround: Continue with user authentication for learning

## 🎯 Learning Value Maintained

This real-world obstacle demonstrates important DevSecOps skills:
- **Problem Solving** - Adapting to tool limitations
- **Security Awareness** - Understanding authentication differences
- **Documentation** - Recording issues and workarounds
- **Pragmatic Approach** - Focusing on learning objectives despite technical challenges

**Next Steps**: Continue with GitHub Actions workflow creation using available authentication methods.