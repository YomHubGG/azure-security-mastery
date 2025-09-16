#!/bin/bash
# 🛑 Stop All Minishell Containers
# Usage: ./stop-minishell.sh

echo "🛑 Stopping all Minishell containers to save costs..."

# Get all container instances with 'minishell' in the name
CONTAINERS=$(az container list --resource-group rg-learning-day1 --query "[?contains(name, 'minishell')].name" --output tsv)

if [ -z "$CONTAINERS" ]; then
    echo "ℹ️  No Minishell containers found running."
    exit 0
fi

echo "Found containers to stop:"
echo "$CONTAINERS"
echo ""

# Stop each container
for container in $CONTAINERS; do
    echo "🗑️  Deleting container: $container"
    az container delete --resource-group rg-learning-day1 --name "$container" --yes
done

echo ""
echo "✅ All Minishell containers stopped!"
echo "💰 Cost savings: ~$0.10-0.20 per hour saved"
echo ""
echo "💡 To restart tomorrow: ./start-minishell.sh"
