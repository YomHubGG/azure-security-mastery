# 🚀 Quick Setup: Logic Apps in Portal

## Method 1: Azure Portal (FASTEST - Recommended)

### Step 1: Create Logic App
```
Azure Portal → Create Resource → Logic App (Consumption)
- Name: workflow-nsg-alert
- Resource Group: rg-security-automation
- Location: West Europe
- Plan: Consumption (pay-per-execution)
```

### Step 2: NSG Security Alert Workflow

**Trigger**: HTTP Request (webhook)
1. Open workflow → Logic App Designer
2. Add trigger: "When a HTTP request is received"
3. Generate schema from sample (paste NSG event JSON)

**Actions**:
1. **Parse JSON** - Extract event details
2. **Condition** - If resource type = NSG
3. **Send email (V2)** - Gmail/Outlook connector
   - To: your-email@example.com
   - Subject: `⚠️ NSG Modified: {resource name}`
   - Body: Event details

**Test**: Copy HTTP POST URL → Use in Azure Monitor alert

---

## Method 2: Simplified - Recurrence Trigger

### Workflow: Daily Compliance Check

**Trigger**: Recurrence (Daily 8 AM)

**Actions**:
1. **HTTP** - Call Azure REST API
   - Method: GET
   - URI: `https://management.azure.com/subscriptions/{sub}/resourceGroups/rg-learning-day1/resources`
   - Authentication: Managed Identity

2. **Condition** - Check resource count

3. **Send email** - Daily status report

---

## CLI Alternative (Simple Test)

```bash
# Get Logic App URL after creation in portal
WORKFLOW_URL=$(az logic workflow show \
  --name workflow-nsg-alert \
  --resource-group rg-security-automation \
  --query accessEndpoint -o tsv)

echo "Workflow URL: $WORKFLOW_URL"

# Trigger manually
curl -X POST "$WORKFLOW_URL" \
  -H "Content-Type: application/json" \
  -d '{"test": "NSG change detected"}'
```

---

## 🎯 Quick Win: Email Alert Workflow (5 min)

**Fastest Path**:
1. Portal → Logic Apps → Create (Consumption)
2. Designer → Blank template
3. Add: **Recurrence** (Every hour)
4. Add: **Send email (V2)** (use personal email)
   - Subject: "Azure Security - Hourly Check"
   - Body: "All systems operational"
5. Save → Run → Check email! ✅

**This proves**:
- Logic Apps working
- Email connector configured
- Automation foundation ready

Expand from there! 🚀
