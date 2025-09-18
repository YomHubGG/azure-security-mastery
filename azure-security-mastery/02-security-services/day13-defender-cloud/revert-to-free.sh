#!/bin/bash

# Revert Defender for Cloud to Free Tier
# Run this if you want to stop the trial and avoid future charges

echo "🔄 Reverting Defender for Cloud to Free Tier"
echo "============================================="

echo "Disabling Defender for Virtual Machines..."
az security pricing create --name "VirtualMachines" --tier "Free"

echo "Disabling Defender for Storage Accounts..."
az security pricing create --name "StorageAccounts" --tier "Free"

echo "Disabling Defender for Key Vaults..."
az security pricing create --name "KeyVaults" --tier "Free"

echo "Disabling Defender for Container Registries..."
az security pricing create --name "ContainerRegistry" --tier "Free"

echo ""
echo "✅ All Defender plans reverted to Free tier"
echo "💰 No future charges will be incurred"
echo "📊 Basic security recommendations still available"

# Check final status
echo ""
echo "📋 Final pricing status:"
az security pricing list --output table