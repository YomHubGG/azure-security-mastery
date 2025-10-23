#!/bin/bash

# Start Secure App Container - Cost-Effective Deployment
# Deploy YOUR published container to Azure Container Instances

echo "🚀 Starting Secure App Container Instance..."
echo "=========================================="

RESOURCE_GROUP="rg-containers-learning"
LOCATION="westeurope"
CONTAINER_NAME="secure-app-demo"
IMAGE="ghcr.io/yomhubgg/secure-app:1.0.0"
DNS_LABEL="yom-secure-app"

echo "📋 Deployment Details:"
echo "- Resource Group: $RESOURCE_GROUP"
echo "- Location: $LOCATION"
echo "- Container: $CONTAINER_NAME"
echo "- Image: $IMAGE"
echo "- DNS: $DNS_LABEL.westeurope.azurecontainer.io"

# Check if resource group exists, create if not
echo ""
echo "🔧 Checking resource group..."
if ! az group show --name $RESOURCE_GROUP &>/dev/null; then
    echo "Creating resource group..."
    az group create --name $RESOURCE_GROUP --location $LOCATION
fi

echo ""
echo "🐳 Deploying container..."

# Check if container is public or private
echo "ℹ️  If your container is PRIVATE, set GITHUB_TOKEN environment variable first:"
echo "   export GITHUB_TOKEN='your_github_pat_here'"
echo ""

if [ -z "$GITHUB_TOKEN" ]; then
    # Assume public container
    az container create \
      --resource-group $RESOURCE_GROUP \
      --name $CONTAINER_NAME \
      --image $IMAGE \
      --cpu 0.5 \
      --memory 0.5 \
      --dns-name-label $DNS_LABEL \
      --ports 3000 \
      --restart-policy OnFailure \
      --environment-variables NODE_ENV=production PORT=3000
else
    # Use GitHub Container Registry authentication
    az container create \
      --resource-group $RESOURCE_GROUP \
      --name $CONTAINER_NAME \
      --image $IMAGE \
      --registry-login-server ghcr.io \
      --registry-username yomhubgg \
      --registry-password $GITHUB_TOKEN \
      --cpu 0.5 \
      --memory 0.5 \
      --dns-name-label $DNS_LABEL \
      --ports 3000 \
      --restart-policy OnFailure \
      --secure-environment-variables NODE_ENV=production PORT=3000
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Secure app deployed successfully!"
    echo ""
    echo "🌐 Getting connection details..."
    
    # Get the FQDN
    FQDN=$(az container show \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_NAME \
        --query "ipAddress.fqdn" \
        --output tsv)
    
    IP=$(az container show \
        --resource-group $RESOURCE_GROUP \
        --name $CONTAINER_NAME \
        --query "ipAddress.ip" \
        --output tsv)
    
    echo "🔗 Access your app at:"
    echo "   http://$FQDN:3000"
    echo "   http://$IP:3000"
    echo ""
    echo "🧪 Test endpoints:"
    echo "   curl http://$FQDN:3000"
    echo "   curl http://$FQDN:3000/health"
    echo ""
    echo "📊 Monitor in Azure Portal: https://portal.azure.com"
    echo ""
    echo "💰 COST TIP: Remember to run ./stop-container.sh when done!"
    echo "    (Uses ~300 vCPU-seconds per 10 minutes = still within FREE tier)"
else
    echo "❌ Deployment failed. Check the error messages above."
    exit 1
fi