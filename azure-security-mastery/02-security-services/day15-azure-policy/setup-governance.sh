#!/bin/bash

# Day 15: Azure Policy & Governance Setup
# Implement governance policies and resource tagging strategy

echo "📋 Day 15: Azure Policy & Governance Setup"
echo "=========================================="

SUBSCRIPTION_ID=$(az account show --query id -o tsv)
RESOURCE_GROUP="rg-learning-day1"

echo ""
echo "🏢 1. Management Group Setup"
echo "----------------------------"

# Create management group for learning environment
echo "Creating management group for governance..."
az account management-group create \
  --name "mg-security-learning" \
  --display-name "Security Learning Environment" \
  2>/dev/null || echo "Management group may already exist"

echo ""
echo "📋 2. Built-in Policy Assignments"
echo "---------------------------------"

# Assign built-in policy: Require HTTPS for storage accounts
echo "Assigning HTTPS requirement policy for storage accounts..."
az policy assignment create \
  --name "require-https-storage" \
  --display-name "Require HTTPS for Storage Accounts" \
  --policy "/providers/Microsoft.Authorization/policyDefinitions/404c3081-a854-4457-ae30-26a93ef643f9" \
  --scope "/subscriptions/$SUBSCRIPTION_ID/resourceGroups/$RESOURCE_GROUP"

# Assign built-in policy: Require tags on resources
echo "Assigning required tags policy..."
az policy assignment create \
  --name "require-tags" \
  --display-name "Require Project and Environment Tags" \
  --policy "/providers/Microsoft.Authorization/policyDefinitions/1e30110a-5ceb-460c-a204-c1c3969c6d62" \
  --scope "/subscriptions/$SUBSCRIPTION_ID/resourceGroups/$RESOURCE_GROUP" \
  --params '{
    "tagName": {
      "value": "Project"
    }
  }'

echo ""
echo "🏷️  3. Resource Tagging Strategy"
echo "--------------------------------"

# Apply standard tags to existing resources
echo "Applying standard tags to resource group..."
az group update \
  --name $RESOURCE_GROUP \
  --tags \
    Environment=Learning \
    Project=Azure-Security \
    CostCenter=Education \
    Owner=Student \
    CreatedDate=$(date +%Y-%m-%d)

# Tag Key Vault
KV_NAME=$(az keyvault list --resource-group $RESOURCE_GROUP --query "[0].name" -o tsv)
if [ ! -z "$KV_NAME" ]; then
  echo "Tagging Key Vault: $KV_NAME"
  az keyvault update \
    --name $KV_NAME \
    --resource-group $RESOURCE_GROUP \
    --tags \
      Environment=Learning \
      Project=Azure-Security \
      Service=KeyVault \
      Criticality=High
fi

# Tag Storage Accounts
echo "Tagging storage accounts..."
az storage account list --resource-group $RESOURCE_GROUP --query "[].name" -o tsv | while read STORAGE_NAME; do
  if [ ! -z "$STORAGE_NAME" ]; then
    echo "Tagging storage account: $STORAGE_NAME"
    az storage account update \
      --name $STORAGE_NAME \
      --resource-group $RESOURCE_GROUP \
      --tags \
        Environment=Learning \
        Project=Azure-Security \
        Service=Storage \
        Criticality=Medium
  fi
done

echo ""
echo "📊 4. Compliance Monitoring"
echo "---------------------------"

# List policy assignments
echo "Current policy assignments:"
az policy assignment list --scope "/subscriptions/$SUBSCRIPTION_ID/resourceGroups/$RESOURCE_GROUP" --output table

echo ""
echo "🏷️  Current resource tags:"
az resource list --resource-group $RESOURCE_GROUP --query "[].{Name:name, Type:type, Tags:tags}" --output table

echo ""
echo "✅ Azure Policy & Governance Setup Complete!"
echo "==========================================="

echo ""
echo "📋 What we've implemented:"
echo "- ✅ Management group structure"
echo "- ✅ HTTPS enforcement policy for storage"
echo "- ✅ Required tags policy"
echo "- ✅ Consistent resource tagging"
echo "- ✅ Compliance monitoring foundation"

echo ""
echo "🌐 Monitor compliance in Azure Portal:"
echo "https://portal.azure.com/#blade/Microsoft_Azure_Policy/PolicyMenuBlade/Compliance"