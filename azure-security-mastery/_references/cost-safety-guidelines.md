# 🚨 Azure Cost Safety Guidelines 

## ⚠️ CARDINAL RULE: NO COSTLY RESOURCES WITHOUT EXPLICIT HUMAN CONSENT

### 💰 Cost-Aware Resource Categories

#### ❌ HIGH COST - ALWAYS ASK PERMISSION FIRST
```bash
# Container Instances (€0.32/day)
# Virtual Machines (€20-100+/day)
# Application Gateways (€20+/day)
# Premium Storage (€0.15+/GB/month)
# Load Balancers with rules (€15+/month)
```

#### 🟡 MEDIUM COST - USE WITH CAUTION
```bash
# Standard Storage Accounts (small costs)
# Basic Container Registry (free tier available)
# Public IP addresses (€0.10/day)
```

#### ✅ FREE/MINIMAL COST - SAFE TO USE
```bash
# Virtual Networks (FREE)
# Network Security Groups (FREE)
# Azure Resource Manager templates (FREE)
# Policy assignments (FREE)
# RBAC roles (FREE)
```

## 🛡️ Mandatory Safety Checks

### Pre-Deployment Checklist:
```bash
# 1. ALWAYS run what-if analysis first
az deployment group what-if --resource-group RG --template-file template.bicep

# 2. Check current costs before adding resources
echo "Current resources:" && az resource list --output table

# 3. Estimate monthly costs
echo "⚠️  Estimated monthly cost: [CALCULATE BEFORE DEPLOY]"

# 4. Get explicit human approval
echo "Deploy costly resources? Type 'I APPROVE COSTS' to continue:"
read -r approval
[[ "$approval" == "I APPROVE COSTS" ]] || exit 1
```

### Session End Cleanup (MANDATORY):
```bash
#!/bin/bash
echo "🧹 MANDATORY: End-of-session cleanup"

# Stop all Container Instances
az container list --query "[].{Name:name, RG:resourceGroup}" -o table
echo "Stop all containers? (y/N):"
read -r response
[[ "$response" =~ ^[Yy]$ ]] && az container list --query "[].{name:name,resourceGroup:resourceGroup}" -o tsv | while IFS=$'\t' read -r name rg; do az container delete --name "$name" --resource-group "$rg" --yes; done

# Verify no costly resources running
echo "✅ Remaining active resources:"
az resource list --query "[?type=='Microsoft.ContainerInstance/containerGroups']" -o table
```

## 🎯 Safe Learning Commands

### Cost-Free Infrastructure Practice:
```bash
# Network security (FREE)
az network vnet create --name safe-vnet --resource-group rg --address-prefix 10.0.0.0/16

# Security groups (FREE)  
az network nsg create --name safe-nsg --resource-group rg

# Policy assignments (FREE)
az policy assignment create --name safe-policy --policy "built-in-policy-id" --scope /subscriptions/sub-id

# RBAC roles (FREE)
az role assignment create --assignee user@domain.com --role Reader --scope /subscriptions/sub-id
```

### Safe Container Development:
```bash
# Build and test locally with Podman (FREE)
podman build -t app:test .
podman run -d -p 3000:3000 app:test

# Push to registry (FREE tier available)
podman push registry.azurecr.io/app:test

# ⚠️  ONLY deploy to ACI with explicit approval
```

## 📊 Daily Cost Monitoring Script

```bash
#!/bin/bash
# daily-cost-check.sh

echo "🔍 Daily Azure Cost Check"
echo "========================="

# Check running costly resources
echo "💰 Container Instances (€0.32/day each):"
az container list --query "[].{Name:name, Status:containers[0].instanceView.currentState.state, RG:resourceGroup}" -o table 2>/dev/null || echo "None found"

echo ""
echo "🏠 Virtual Machines:"
az vm list --query "[].{Name:name, Status:powerState, Size:hardwareProfile.vmSize, RG:resourceGroup}" -o table 2>/dev/null || echo "None found"

echo ""  
echo "🌐 Public IPs (€0.10/day each):"
az network public-ip list --query "[].{Name:name, IP:ipAddress, Status:provisioningState, RG:resourceGroup}" -o table

echo ""
echo "💡 RECOMMENDATION: Delete unused resources to save costs!"
```

## 🎓 Learning Phase Cost Budgets

### Phase 4 - Application Security (NEXT):
- **Key Vault**: FREE tier (10,000 operations/month)  
- **App Service**: FREE tier available
- **Budget Alert**: Set at €5/month maximum

### Phase 5 - Compliance & Governance:
- **Azure Policy**: FREE
- **RBAC**: FREE  
- **Management Groups**: FREE
- **Budget Alert**: €0/month (should be free)

### Phase 6 - Incident Response:
- **Log Analytics**: 5GB/month FREE
- **Security Center**: FREE tier
- **Budget Alert**: Set at €3/month maximum

---

## 🔒 COMMITMENT: Agent Cost Safety Promise

**I, as your AI assistant, commit to:**

1. ❌ **NEVER deploy costly resources without explicit human consent**
2. ⚠️ **ALWAYS warn about costs BEFORE any deployment**  
3. 🔍 **ALWAYS run `what-if` analysis first**
4. 🧹 **ALWAYS remind about cleanup at session end**
5. 💡 **ALWAYS suggest cost-free alternatives when possible**

**Violation of these rules = IMMEDIATE STOP and human consultation required**

---
*Safety guidelines established: September 2, 2025*
*Zero-tolerance policy for unauthorized costly deployments*
