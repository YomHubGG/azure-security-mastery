# Azure Cost Management Guide 💰

## Current Usage Analysis (as of 2025-09-02)

### 🏃 Active Resources
```
Resource                     Type                    Cost/Hour    Status
────────────────────────────────────────────────────────────────────
dev-secure-container        Container Instance      €0.0134      Running
devacr4uybw3c2lbkwm         Container Registry      €0           Free tier
devsec4uybw3c2lbkwm         Storage Account         €0.001       Active
Public IP (4.175.207.4)     Public IP Address       €0.004       Active
```

### 📊 Cost Estimates
- **Daily Cost**: €0.35-0.45/day
- **Monthly Projection**: €10-15/month
- **Running Time**: Container active since 2025-09-01 17:21 (~14.5 hours)

## 💡 Cost Optimization Strategies

### 1. Container Instance Management
```bash
# Stop container when not needed
az container stop --name dev-secure-container --resource-group rg-learning-day1

# Start when needed
az container start --name dev-secure-container --resource-group rg-learning-day1

# Delete completely (can redeploy with Bicep)
az container delete --name dev-secure-container --resource-group rg-learning-day1 --yes
```

### 2. Learning Schedule Optimization
- **Development Hours**: Stop containers outside learning sessions
- **Weekend Pause**: Stop all non-essential resources on weekends
- **Session-based**: Only run resources during active learning

### 3. Free Tier Maximization
- Container Registry Basic: 10GB storage free
- Storage Accounts: 5GB LRS free for 12 months
- Virtual Network: Completely free
- Network Security Groups: Free

## 🎁 Azure Free Credits Check

### How to Check Your Credits:
1. **Portal Method**:
   - Go to https://portal.azure.com
   - Search "Cost Management + Billing"
   - Click "Credits + offers"
   - View remaining free credit

2. **New Account Benefits**:
   - $200 (€170) free credit for 30 days
   - 12 months of limited free services
   - Your subscription: `a174b81f-93c2-4089-afa0-6f82a5165c86`

## 🚨 Cost Alerts Setup

### Recommended Budget Alerts:
```bash
# Set up budget alert (requires portal)
# 1. Go to Cost Management + Billing
# 2. Create Budget
# 3. Set alert at €50/month (safe threshold)
# 4. Configure email notifications
```

### Daily Monitoring Commands:
```bash
# Quick resource check
az resource list --output table

# Container status
az container show --name dev-secure-container --resource-group rg-learning-day1 --query "containers[0].instanceView.currentState.state"

# Stop all containers in resource group
az container list --resource-group rg-learning-day1 --query "[].name" -o tsv | xargs -I {} az container stop --name {} --resource-group rg-learning-day1
```

## 📈 Learning vs Cost Balance

### High-Value, Low-Cost Resources:
- ✅ Virtual Networks (Free)
- ✅ Network Security Groups (Free)
- ✅ Storage Accounts (Free tier available)
- ✅ Container Registry Basic (10GB free)

### Cost-Conscious Learning:
- 🟡 Container Instances (€0.32/day) - Stop when not in use
- 🟡 Public IPs (€0.10/day) - Consider deallocating
- 🔴 Premium services - Avoid during learning phase

## 💾 Resource Cleanup Scripts

### Daily Cleanup:
```bash
#!/bin/bash
echo "🧹 Daily cleanup routine..."

# Stop container instances
az container stop --name dev-secure-container --resource-group rg-learning-day1

echo "✅ Containers stopped to save costs"
```

### Weekend Cleanup:
```bash
#!/bin/bash
echo "🧹 Weekend cleanup routine..."

# Stop and deallocate expensive resources
az container stop --name dev-secure-container --resource-group rg-learning-day1

# Note: Keep storage and registry for continuity
echo "✅ Weekend cost optimization complete"
```

## 🎯 Next Session Cost Planning

### Before Next Learning Session:
1. Check current credit balance
2. Review this cost analysis
3. Decide on resource allocation strategy
4. Set daily spending alerts

### Cost-Effective Learning Path:
1. **Phase 4 (Application Security)**: Use free Key Vault tier
2. **Phase 5 (Compliance)**: Policy and governance are free
3. **Phase 6 (Incident Response)**: Log Analytics has free tier
4. **Phase 7 (Specialization)**: Choose cost-effective specializations

---
*Generated: 2025-09-02 | Azure subscription: a174b81f-93c2-4089-afa0-6f82a5165c86*
