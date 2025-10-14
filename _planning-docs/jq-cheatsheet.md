# jq - JSON Processor Cheatsheet

**What is jq?** Command-line JSON processor (like `grep` for JSON)  
**Why you need it:** Azure CLI, Bicep, npm, package.json all use JSON!  
**Installation:** `sudo apt install jq` (or `brew install jq` on Mac)

---

## 🚀 **Installation**

```bash
# Parrot/Debian/Ubuntu
sudo apt update && sudo apt install -y jq

# Check version after install
jq --version
```

---

## 📚 **Common Use Cases**

### **1. Pretty-Print JSON**
```bash
# Before: Ugly one-line JSON
cat package.json

# After: Beautiful formatted JSON
cat package.json | jq '.'
```

### **2. Extract Specific Fields**
```bash
# Get just the name
cat package.json | jq '.name'

# Get nested values
cat package.json | jq '.scripts.start'

# Multiple fields
cat package.json | jq '.name, .version'
```

### **3. Azure CLI Magic**
```bash
# Get all resource names
az resource list | jq '.[].name'

# Filter by type
az resource list | jq '.[] | select(.type == "Microsoft.Storage/storageAccounts") | .name'

# Extract specific properties
az storage account show -n STORAGE_NAME -g RG_NAME | jq '{name: .name, location: .location, https: .enableHttpsTrafficOnly}'
```

### **4. Array Operations**
```bash
# Get first item
cat array.json | jq '.[0]'

# Get last item
cat array.json | jq '.[-1]'

# Get array length
cat array.json | jq '. | length'

# Map over array
cat array.json | jq '.[] | .name'
```

### **5. Filter and Transform**
```bash
# Filter objects
jq '.[] | select(.age > 25)'

# Map values
jq '.users | map(.name)'

# Group by
jq 'group_by(.category)'
```

---

## 🎯 **Real Azure Security Examples**

### **Analyze Azure Resources**
```bash
# Get all storage accounts with their security settings
az storage account list | jq '.[] | {
  name: .name,
  https_only: .enableHttpsTrafficOnly,
  tls_version: .minimumTlsVersion,
  public_access: .allowBlobPublicAccess
}'

# Find resources without HTTPS enforcement
az storage account list | jq '.[] | select(.enableHttpsTrafficOnly == false) | .name'

# Count resources by location
az resource list | jq 'group_by(.location) | map({location: .[0].location, count: length})'
```

### **Parse Bicep/ARM Templates**
```bash
# Extract all resource types
az deployment group show -g RG_NAME -n DEPLOYMENT_NAME | jq '.properties.template.resources[].type'

# Find resources with specific tags
az resource list | jq '.[] | select(.tags.Environment == "production")'
```

### **VS Code Settings Analysis**
```bash
# Extract all telemetry settings
cat ~/.config/Code/User/settings.json | jq 'to_entries | map(select(.key | test("telemetry"))) | from_entries'

# Get all extension IDs
code --list-extensions --show-versions | jq -R 'split("@") | {name: .[0], version: .[1]}'
```

### **Security Audit Reports**
```bash
# Parse PowerShell JSON output
cat security-report.json | jq '.resources[] | select(.securityScore < 80)'

# Generate summary
cat audit-results.json | jq '{
  total_resources: (.resources | length),
  secure: (.resources | map(select(.secure)) | length),
  percentage: ((.resources | map(select(.secure)) | length) / (.resources | length) * 100)
}'
```

---

## 🔥 **Advanced Techniques**

### **Conditional Logic**
```bash
# If-then-else
jq 'if .age > 18 then "adult" else "minor" end'

# Multiple conditions
jq 'if .type == "storage" and .secure == true then "GOOD" else "CHECK" end'
```

### **String Manipulation**
```bash
# Lowercase
jq '.name | ascii_downcase'

# Split strings
jq '.tags | split(",")'

# Concatenate
jq '.firstName + " " + .lastName'
```

### **Working with Dates**
```bash
# Parse ISO dates
jq '.createdAt | fromdateiso8601'

# Format dates
jq 'now | todateiso8601'
```

### **Custom Functions**
```bash
# Define and use function
jq 'def double(x): x * 2; .numbers | map(double(.))'
```

---

## 💡 **Practical Azure Security Workflow**

### **Daily Security Check Script**
```bash
#!/bin/bash
# check-azure-security.sh

echo "🔍 Azure Security Status"

# Get all storage accounts
az storage account list > /tmp/storage.json

# Analyze security
cat /tmp/storage.json | jq -r '
  .[] | 
  "Storage: \(.name) | HTTPS: \(.enableHttpsTrafficOnly) | TLS: \(.minimumTlsVersion)"
'

# Count insecure resources
INSECURE=$(cat /tmp/storage.json | jq '[.[] | select(.enableHttpsTrafficOnly == false)] | length')

echo "⚠️  Insecure storage accounts: $INSECURE"
```

### **Cost Analysis**
```bash
# Get cost data and analyze
az consumption usage list --start-date 2025-10-01 --end-date 2025-10-14 \
  | jq '[group_by(.instanceName) | .[] | {
      resource: .[0].instanceName,
      total_cost: (map(.pretaxCost) | add)
    }] | sort_by(.total_cost) | reverse'
```

---

## 🎓 **Learning Resources**

### **Interactive Tutorial**
```bash
# After installing jq
echo '{"name":"test","value":42}' | jq '.'
```

### **Official Manual**
```bash
# View built-in help
jq --help

# Or visit: https://jqlang.github.io/jq/manual/
```

### **Practice Playground**
- https://jqplay.org/ - Online jq playground
- Test queries without installing

---

## 🚀 **Why You Need It for Azure Security**

### **Azure CLI Returns JSON**
```bash
# Every Azure CLI command outputs JSON
az account show
az resource list
az storage account show
az keyvault secret list
```

### **Configuration Files are JSON**
```bash
# VS Code settings
~/.config/Code/User/settings.json

# npm packages
package.json

# Azure DevOps
azure-pipelines.json

# Bicep compiled output
*.json (ARM templates)
```

### **API Responses**
```bash
# When calling Azure REST APIs
curl https://management.azure.com/... | jq '.value'
```

---

## 📊 **Quick Reference Table**

| Task | Command |
|------|---------|
| Pretty print | `jq '.'` |
| Get field | `jq '.field'` |
| Get nested | `jq '.parent.child'` |
| Array element | `jq '.[0]'` |
| Filter | `jq '.[] \| select(.age > 25)'` |
| Map array | `jq '.[] \| .name'` |
| Count | `jq '. \| length'` |
| Keys | `jq 'keys'` |
| Values | `jq 'values'` |
| Type check | `jq 'type'` |

---

## ✅ **Installation & First Test**

```bash
# Install
sudo apt install -y jq

# Test with your VS Code settings
cat ~/.config/Code/User/settings.json | jq '.'

# Filter telemetry settings
cat ~/.config/Code/User/settings.json | jq 'to_entries | map(select(.key | contains("telemetry")))'

# Success! 🎉
```

---

## 🎯 **Bottom Line**

**Without jq**: Painful JSON parsing with grep/sed/awk  
**With jq**: Clean, readable, powerful JSON manipulation  

**For Azure Security**: ESSENTIAL tool  
**Install time**: 30 seconds  
**Value**: Infinite ♾️

---

**Install now?**
```bash
sudo apt install -y jq
```
