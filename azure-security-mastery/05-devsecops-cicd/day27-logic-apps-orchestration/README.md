# 🔄 Day 27: Logic Apps & Security Orchestration

**Date**: October 1, 2025 (Session #13.5 - Bonus)  
**Focus**: Automated security workflows and incident response  
**Cost**: €0 (Consumption tier: 4,000 free actions/month)

---

## 🎯 Learning Objectives (Per 1-Year Plan)

```bash
# Day 27: Logic Apps & Security Orchestration
├── Create automated security workflows
├── Set up incident response automation  
├── Implement approval processes
├── Add notification and alerting
└── Commit: "Security orchestration platform"
```

---

## 📚 Why Logic Apps for Security?

**Serverless Security Orchestration**:
- **No Infrastructure**: Pay-per-execution, zero maintenance
- **300+ Connectors**: Email, Teams, Azure services, APIs
- **Visual Designer**: Build workflows without code
- **Enterprise Integration**: Connects all your security tools
- **Cost-Effective**: 4,000 free actions/month (plenty for learning)

**Real-World Use Cases**:
1. Auto-respond to security alerts
2. Compliance violation notifications
3. Key Vault secret expiration monitoring
4. Security report distribution automation
5. Multi-step incident response workflows

---

## 🛠️ Phase 1: Create Security Alert Workflow (20 min)

### Workflow 1: NSG Rule Change Alert

**Trigger**: When NSG rules modified → Email security team

```bash
# Create Logic App resource group (if needed)
az group create --name rg-security-automation --location westeurope

# Create consumption Logic App
az logic workflow create \
  --resource-group rg-security-automation \
  --name workflow-nsg-alert \
  --location westeurope \
  --definition '{
    "definition": {
      "$schema": "https://schema.management.azure.com/providers/Microsoft.Logic/schemas/2016-06-01/workflowdefinition.json#",
      "triggers": {
        "Recurrence": {
          "type": "Recurrence",
          "recurrence": {
            "frequency": "Hour",
            "interval": 1
          }
        }
      },
      "actions": {},
      "outputs": {}
    }
  }'
```

**Visual Designer Setup** (Portal):
1. Open Azure Portal → Logic Apps
2. Open `workflow-nsg-alert`
3. Add trigger: "When an Azure Resource Manager event occurs"
   - Subscription: Your subscription
   - Resource Type: `Microsoft.Network/networkSecurityGroups`
   - Event Type: `Microsoft.Resources.ResourceWriteSuccess`

4. Add action: "Send an email (V2)"
   - To: Your email
   - Subject: `⚠️ NSG Rule Changed`
   - Body: Dynamic content from trigger

5. Save workflow

---

## 🔐 Phase 2: Key Vault Monitoring Workflow (20 min)

### Workflow 2: Secret Expiration Alert

**Integration with Day 25 PowerShell Scripts**:

```bash
az logic workflow create \
  --resource-group rg-security-automation \
  --name workflow-keyvault-monitor \
  --location westeurope
```

**Designer Steps**:
1. **Trigger**: Recurrence (Daily at 9 AM)
2. **Action 1**: Run PowerShell script (check secret expiration)
   - Use Azure Automation Runbook OR
   - HTTP webhook to execute script
3. **Condition**: If secrets expire in < 30 days
4. **Action 2**: Send email alert with details
5. **Action 3**: Post to Teams channel (optional)

---

## 📊 Phase 3: Compliance Report Distribution (20 min)

### Workflow 3: Daily Security Report

**Automate Day 25 Report Generation**:

```bash
az logic workflow create \
  --resource-group rg-security-automation \
  --name workflow-security-report \
  --location westeurope
```

**Designer Steps**:
1. **Trigger**: Recurrence (Daily at 8 AM)
2. **Action 1**: Execute PowerShell script
   - Run `generate-report.ps1` from Day 25
3. **Action 2**: Get blob content (HTML report from storage)
4. **Action 3**: Send email with report attached
5. **Action 4**: Archive report to blob storage

---

## 🎯 Quick CLI Commands

```bash
# List Logic Apps
az logic workflow list --resource-group rg-security-automation -o table

# Show workflow details
az logic workflow show --name workflow-nsg-alert --resource-group rg-security-automation

# Get workflow run history
az logic workflow run list --name workflow-nsg-alert --resource-group rg-security-automation

# Manual trigger test
az logic workflow run trigger --name workflow-nsg-alert --resource-group rg-security-automation --trigger-name manual
```

---

## ✅ Success Criteria

- ✅ 3 Logic Apps created for security automation
- ✅ Email notifications working
- ✅ Integration with Day 25 PowerShell scripts
- ✅ Workflow documentation complete
- ✅ Zero cost (within free tier)

---

## 💼 Portfolio Value

**Demonstrates**:
- Security orchestration expertise
- Serverless automation architecture
- Incident response automation
- Enterprise integration patterns
- Cost-effective security solutions

**Career Skills**: Security Operations, Incident Response, SOAR platforms

---

Ready to build automated security workflows! 🚀
