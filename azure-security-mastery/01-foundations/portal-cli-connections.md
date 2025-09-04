# 🔄 Portal ↔ CLI Connection Sheet

**Purpose**: Connect what you see in portal to what you did with CLI/Bicep

## 🏗️ **Your Infrastructure Connections**

### **Storage Account** (Your First Success!)
```
PORTAL VIEW                          CLI/BICEP CODE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📁 Resource Group: rg-learning-day1   az group create --name rg-learning-day1
💾 Storage Account: stlearning...      resource storageAccount 'Microsoft.Storage/...'
🔒 HTTPS Only: ✅ Enabled             supportsHttpsTrafficOnly: true
🔐 TLS Version: 1.2                   minimumTlsVersion: 'TLS1_2'  
🚫 Public Access: Disabled            allowBlobPublicAccess: false
🌍 Location: West Europe              location: resourceGroup().location
💰 Tier: Standard LRS                 sku: { name: 'Standard_LRS' }
```

### **Resource Groups**
```
PORTAL VIEW                          CLI COMMAND
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📋 Name: rg-learning-day1             --name rg-learning-day1
🌍 Region: West Europe                --location westeurope
✅ Status: Succeeded                  Result of successful deployment
📅 Created: [Date you ran command]     When you executed the CLI command
```

## 🔍 **Portal Navigation to CLI Commands**

### **When you see... in Portal → Remember this CLI command**

| Portal Action | CLI Equivalent |
|---------------|----------------|
| Resource Groups → Create | `az group create --name [name] --location [location]` |
| Storage Account → Create | `az deployment group create --template-file storage-account.bicep` |
| Activity Log → View | `az monitor activity-log list` |
| Cost Analysis → View | `az consumption usage list` |
| Resource → Delete | `az resource delete --name [name] --resource-group [rg]` |

## 👀 **What to Look For During Your Tour**

### **Security Indicators** 🔒:
- Green checkmarks = Good security
- Yellow warnings = Recommendations
- Red alerts = Security issues
- Lock icons = Protected resources

### **Cost Indicators** 💰:
- € symbols = Paid services
- "Free" labels = No cost services
- Usage bars = Resource consumption
- Forecast graphs = Predicted costs

### **Status Indicators** 📊:
- Green dots = Running/healthy
- Red dots = Issues/failures  
- Yellow dots = Warnings
- Gray dots = Stopped/deallocated

## 🤔 **Questions to Ask While Exploring**

### **At Each Resource**:
1. "How did my CLI command create this?"
2. "What would I click to recreate this manually?"
3. "Where do I see the security settings I configured?"
4. "What additional options are available?"

### **In Activity Logs**:
1. "Can I find my deployment commands here?"
2. "Who made changes and when?"
3. "Which operations succeeded/failed?"

### **In Cost Management**:
1. "How much is each resource costing?"
2. "Which resources are free?"
3. "What would happen if I scale up?"

## 📚 **Learning Multipliers**

### **Portal Benefits** (What CLI Can't Show You):
- 📊 **Visual dashboards** and charts
- 🗺️ **Network topology diagrams**  
- 💰 **Real-time cost breakdowns**
- 🔍 **Interactive troubleshooting**
- 📈 **Monitoring graphs**

### **CLI Benefits** (What Portal Makes Tedious):
- 🚀 **Automation** at scale
- 🔄 **Repeatable** deployments
- 📝 **Version control** of infrastructure
- ⚡ **Speed** of operations
- 🧪 **Testing** and validation

## 🎯 **After Your Portal Tour**

You'll understand WHY professional Azure engineers use both:
- **Portal** for exploration, monitoring, troubleshooting
- **CLI/IaC** for creation, automation, production

This combination makes you a complete Azure professional! 💪

---

**💡 Remember**: Every professional Azure Security Engineer needs both visual understanding AND automation skills. You're building the complete skill set!
