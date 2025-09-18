#!/bin/bash

# Day 13: Microsoft Defender for Cloud Setup
# Enable comprehensive threat protection across Azure resources

echo "🛡️ Day 13: Microsoft Defender for Cloud Setup"
echo "============================================="

RESOURCE_GROUP="rg-learning-day1"

echo ""
echo "📊 1. Current Security Status"
echo "-----------------------------"

# Check current Defender pricing/status
echo "✅ Current Defender for Cloud pricing tiers:"
az security pricing list --output table

echo ""
echo "🔧 2. Enable Defender Plans"
echo "---------------------------"

# Enable Defender for Virtual Machines
echo "Enabling Defender for Virtual Machines..."
az security pricing create \
  --name "VirtualMachines" \
  --tier "Standard"

# Enable Defender for Storage Accounts  
echo "Enabling Defender for Storage Accounts..."
az security pricing create \
  --name "StorageAccounts" \
  --tier "Standard"

# Enable Defender for Key Vaults
echo "Enabling Defender for Key Vaults..."
az security pricing create \
  --name "KeyVaults" \
  --tier "Standard"

# Enable Defender for Container Registries (for future use)
echo "Enabling Defender for Container Registries..."
az security pricing create \
  --name "ContainerRegistry" \
  --tier "Standard"

echo ""
echo "📈 3. Security Score & Recommendations"
echo "-------------------------------------"

# Get security score (if available)
echo "Current security posture:"
az security task list --output table --query "[?state=='Active'].{Resource:resourceId,Recommendation:securityTaskParameters.name}" || echo "Security tasks will populate after Defender activation"

echo ""
echo "🔔 4. Security Contacts Configuration"
echo "------------------------------------"

# Set up security contact (replace with actual email)
az security contact create \
  --email "security@yourdomain.com" \
  --phone "1234567890" \
  --alert-notifications true \
  --alerts-admins true

echo ""
echo "✅ Defender for Cloud Setup Complete!"
echo "===================================="

echo ""
echo "📋 Next Steps:"
echo "- Monitor security recommendations in Azure Portal"
echo "- Review and implement high-priority security tasks"
echo "- Set up Log Analytics workspace for advanced monitoring"
echo "- Configure automated response playbooks"

echo ""
echo "🌐 Access Defender for Cloud:"
echo "https://portal.azure.com/#blade/Microsoft_Azure_Security/SecurityMenuBlade/0"