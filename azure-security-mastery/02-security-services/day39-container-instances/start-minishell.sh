#!/bin/bash

# Start Minishell Container - Cost-Effective Deployment
# Deploy only when needed to minimize Azure costs

echo "🚀 Starting Minishell Container Instance..."
echo "=========================================="

RESOURCE_GROUP="rg-learning-day1"
LOCATION="westeurope"
TEMPLATE_FILE="minishell-aci-secure.bicep"

echo "📋 Deployment Details:"
echo "- Resource Group: $RESOURCE_GROUP"
echo "- Location: $LOCATION"
echo "- Template: $TEMPLATE_FILE"

echo ""
echo "🔧 Deploying container group..."

az deployment group create \
  --resource-group $RESOURCE_GROUP \
  --template-file $TEMPLATE_FILE \
  --parameters location=$LOCATION \
  --name "minishell-$(date +%Y%m%d-%H%M)"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Minishell container deployed successfully!"
    echo ""
    echo "🌐 Getting connection details..."
    
    # Get the FQDN
    FQDN=$(az container show \
        --resource-group $RESOURCE_GROUP \
        --name cg-minishell-secure \
        --query "ipAddress.fqdn" \
        --output tsv)
    
    echo "🔗 Access your minishell at: http://$FQDN"
    echo "📊 Monitor in Azure Portal: https://portal.azure.com"
    echo ""
    echo "💰 COST TIP: Remember to run ./stop-minishell.sh when done!"
    echo "    Container instances are billed per second while running."
else
    echo "❌ Deployment failed. Check the error messages above."
    exit 1
fi