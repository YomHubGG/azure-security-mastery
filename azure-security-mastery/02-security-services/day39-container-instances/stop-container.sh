#!/bin/bash

# Stop Secure App Container - Save Azure Costs
# Remove container instance when not needed

echo "🛑 Stopping Secure App Container Instance..."
echo "=========================================="

RESOURCE_GROUP="rg-containers-learning"
CONTAINER_NAME="secure-app-demo"

echo "📋 Container Details:"
echo "- Resource Group: $RESOURCE_GROUP"
echo "- Container: $CONTAINER_NAME"

echo ""
echo "🔍 Checking if container exists..."

# Check if container exists
if az container show --resource-group $RESOURCE_GROUP --name $CONTAINER_NAME &>/dev/null; then
    echo "✅ Container found. Getting final status..."
    
    # Show container status before deletion
    az container show \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_NAME \
        --query "{Name:name, State:instanceView.state, FQDN:ipAddress.fqdn}" \
        --output table
    
    echo ""
    echo "🗑️  Deleting container..."
    
    az container delete \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_NAME \
        --yes
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ Secure app container stopped and deleted successfully!"
        echo "💰 Azure costs for this container have stopped."
        echo ""
        echo "� Free tier impact: ~300 vCPU-seconds used (49,700 remaining)"
        echo ""
        echo "�🔄 To restart later, run: ./start-container.sh"
    else
        echo "❌ Failed to delete container. Check the error messages above."
        exit 1
    fi
else
    echo "ℹ️  Container '$CONTAINER_NAME' not found."
    echo "   Either it's already deleted or hasn't been created yet."
    echo ""
    echo "🚀 To start the container, run: ./start-container.sh"
fi