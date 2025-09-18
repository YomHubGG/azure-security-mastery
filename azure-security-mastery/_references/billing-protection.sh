#!/bin/bash

# EMERGENCY BILLING PROTECTION SETUP
# Run this to protect against unexpected Azure charges

echo "🚨 SETTING UP BILLING PROTECTION"
echo "================================"

SUBSCRIPTION_ID=$(az account show --query id -o tsv)

echo "📧 1. Creating Billing Alert"
echo "---------------------------"

# Create action group for billing alerts (email notification)
echo "Setting up email notifications for billing..."

# Note: This requires manual setup in portal for email configuration
echo "⚠️  MANUAL STEP REQUIRED:"
echo "   1. Go to: https://portal.azure.com/#blade/Microsoft_Azure_Billing/SubscriptionsBlade"
echo "   2. Click your subscription → Budgets"
echo "   3. Create budget with $50 limit"
echo "   4. Set alerts at 80%, 90%, 100%"

echo ""
echo "🔒 2. Current Spending Check"
echo "----------------------------"

# Check current month spending
echo "Current subscription: $SUBSCRIPTION_ID"
echo "Resource groups and estimated costs:"

az group list --query "[].{Name:name, Location:location}" --output table

echo ""
echo "📊 3. High-Cost Resources Audit"
echo "-------------------------------"

# List resources that could cause high bills
echo "Resources that may incur significant costs:"
az resource list --query "[?sku.tier=='Standard' || sku.tier=='Premium' || contains(type,'ContainerRegistry') || contains(type,'serverFarms')].{Name:name, Type:type, Tier:sku.tier, Size:sku.name}" --output table

echo ""
echo "🛡️ 4. Defender for Cloud Status"
echo "--------------------------------"

echo "Current Defender plans and trial time:"
for plan in VirtualMachines StorageAccounts KeyVaults ContainerRegistry; do
    echo "Checking $plan..."
    az security pricing show --name "$plan" --query "{name:name, tier:pricingTier, trialTime:freeTrialRemainingTime}" --output table 2>/dev/null || echo "Plan $plan not found"
done

echo ""
echo "⚠️  CRITICAL REMINDERS:"
echo "========================"
echo "1. Set calendar reminder for October 17th (trial expires ~Oct 18)"
echo "2. Defender will auto-charge ~$65-85/month after trial"
echo "3. Storage accounts already billing ~$20-30/month"
echo "4. Container registry billing ~$5/month"
echo ""
echo "💰 ESTIMATED TOTAL AFTER TRIAL: $90-120/month"
echo ""
echo "🛑 TO AVOID CHARGES: Run the revert-to-free.sh script before Oct 17"