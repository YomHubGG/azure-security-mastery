#!/bin/bash

# Stop Minishell Container - Save Azure Costs
# Remove container instance when not needed

echo "🛑 Stopping Minishell Container Instance..."
echo "=========================================="

RESOURCE_GROUP="rg-learning-day1"
CONTAINER_GROUP="cg-minishell-secure"

echo "📋 Container Details:"
echo "- Resource Group: $RESOURCE_GROUP"
echo "- Container Group: $CONTAINER_GROUP"

echo ""
echo "🔍 Checking if container exists..."

# Check if container group exists
if az container show --resource-group $RESOURCE_GROUP --name $CONTAINER_GROUP &>/dev/null; then
    echo "✅ Container found. Getting final status..."
    
    # Show container status before deletion
    az container show \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_GROUP \
        --query "{name:name, state:containers[0].instanceView.currentState.state, fqdn:ipAddress.fqdn}" \
        --output table
    
    echo ""
    echo "🗑️  Deleting container group..."
    
    az container delete \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_GROUP \
        --yes
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ Minishell container stopped and deleted successfully!"
        echo "💰 Azure costs for this container have stopped."
        echo ""
        echo "🔄 To restart later, run: ./start-minishell.sh"
    else
        echo "❌ Failed to delete container. Check the error messages above."
        exit 1
    fi
else
    echo "ℹ️  Container group '$CONTAINER_GROUP' not found."
    echo "   Either it's already deleted or hasn't been created yet."
    echo ""
    echo "🚀 To start the container, run: ./start-minishell.sh"
fi