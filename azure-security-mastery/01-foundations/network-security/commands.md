# � Passenger Journey - Network Security Day
*Brain-Soaking Mode: Observation & Pattern Recognition*

**Today's Philosophy**: "I'm here to watch, absorb, and get familiar. Understanding comes with repetition."

## 🧠 Pattern Observation Session

### Step 1: Look at What We've Built
```bash
# Navigate to our workspace
cd /home/yom/cybersecurity-journey/azure-security-mastery/01-foundations/network-security/

# Observe: What does a multi-resource template look like?
echo "=== 👀 OBSERVING: Multi-Resource Bicep Template ==="
cat multi-resource.bicep
echo ""
echo "🧠 Brain-Soak Notes:"
echo "- Multiple resources defined in one file"
echo "- Notice the @description comments"
echo "- See how resources reference each other"
echo "- Parameters at the top control behavior"
```

### Step 2: See Different Environments in Action
```bash
# Observe: How do dev vs prod differ?
echo "=== 👀 OBSERVING: Environment Differences ==="
echo "DEV settings:"
cat dev.parameters.json
echo ""
echo "PROD settings:"  
cat prod.parameters.json
echo ""
echo "🧠 Brain-Soak Notes:"
echo "- Same template, different values"
echo "- Different sizes, different security levels"
echo "- One template → Many environments"
```

### Step 3: Watch Validation in Action
```bash
# Observe: How Azure checks our template
echo "=== 👀 OBSERVING: Template Validation Process ==="

# Step 1: Compile Bicep to ARM (watch for syntax errors)
echo "Building template..."
bicep build multi-resource.bicep

# Step 2: Ask Azure "would this work?" (logical validation)
echo "Asking Azure to validate..."
az deployment group validate \
  --resource-group rg-learning-day1 \
  --template-file multi-resource.bicep \
  --parameters @dev.parameters.json
```

## Phase 2: What-If Analysis

```bash
# See what would be created (dry run)
az deployment group what-if \
  --resource-group rg-learning-day1 \
  --template-file multi-resource.bicep \
  --parameters @dev.parameters.json
```

## Phase 3: Deploy Development Environment

```bash
# Deploy dev environment
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file multi-resource.bicep \
  --parameters @dev.parameters.json \
  --name "deploy-dev-$(date +%Y%m%d-%H%M%S)"
```

## Phase 4: Verify Deployment

```bash
# List all resources in the group
az resource list --resource-group rg-learning-day1 --output table

# Check specific resources
az network vnet show --resource-group rg-learning-day1 --name dev-vnet-security
az network nsg show --resource-group rg-learning-day1 --name dev-nsg-security
az storage account show --resource-group rg-learning-day1 --name [storage-account-name]
```

## Phase 5: Production Environment (Optional)

```bash
# Create production resource group
az group create --name rg-learning-prod --location westeurope

# Deploy prod environment
az deployment group create \
  --resource-group rg-learning-prod \
  --template-file multi-resource.bicep \
  --parameters @prod.parameters.json \
  --name "deploy-prod-$(date +%Y%m%d-%H%M%S)"
```

## Cleanup Commands (End of Session)

```bash
# If you want to clean up dev resources
az deployment group delete --resource-group rg-learning-day1 --name [deployment-name]

# Or delete specific resources
az network vnet delete --resource-group rg-learning-day1 --name dev-vnet-security
```

---

**💡 Pro Tips:**
- Always run `bicep build` first to catch syntax errors
- Use `what-if` to preview changes before deploying
- Name deployments with timestamps for tracking
- Keep parameter files for reusable configurations
