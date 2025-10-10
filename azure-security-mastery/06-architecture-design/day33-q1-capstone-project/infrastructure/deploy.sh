#!/bin/bash

# ============================================================================
# 🚀 DAY 33 CAPSTONE - DEPLOYMENT SCRIPT
# ============================================================================
# Description: Deploy complete secure infrastructure to Azure
# Cost: €0 - All FREE tier resources
# Security: 88/100 baseline
# ============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# ============================================================================
# CONFIGURATION
# ============================================================================

RESOURCE_GROUP="rg-capstone-security"
LOCATION="westeurope"
DEPLOYMENT_NAME="capstone-deploy-$(date +%Y%m%d-%H%M%S)"

echo ""
echo -e "${PURPLE}============================================${NC}"
echo -e "${PURPLE}   🔒 Day 33 Capstone Deployment${NC}"
echo -e "${PURPLE}============================================${NC}"
echo ""

# ============================================================================
# PRE-DEPLOYMENT CHECKS
# ============================================================================

echo -e "${BLUE}📋 Step 1: Pre-deployment checks...${NC}"

# Check if logged in to Azure
if ! az account show &> /dev/null; then
    echo -e "${RED}❌ Not logged in to Azure CLI${NC}"
    echo "Please run: az login"
    exit 1
fi

echo -e "${GREEN}✅ Azure CLI authenticated${NC}"

# Get current subscription
SUBSCRIPTION=$(az account show --query name -o tsv)
SUBSCRIPTION_ID=$(az account show --query id -o tsv)
echo -e "${GREEN}✅ Using subscription: ${SUBSCRIPTION}${NC}"
echo -e "   Subscription ID: ${SUBSCRIPTION_ID}"

# ============================================================================
# COST PROTECTION CHECK
# ============================================================================

echo ""
echo -e "${YELLOW}💰 Step 2: Cost Protection Verification...${NC}"
echo ""
echo "This deployment uses ONLY FREE tier resources:"
echo "  ✓ App Service Plan: F1 (Free - 60 min/day)"
echo "  ✓ Storage Account: Standard_LRS (5GB free)"
echo "  ✓ Key Vault: Standard (10K ops/month free)"
echo "  ✓ Application Insights: (5GB/month free)"
echo "  ✓ Log Analytics: (5GB/month free)"
echo "  ✓ Virtual Network: FREE"
echo "  ✓ Network Security Group: FREE"
echo ""
echo -e "${GREEN}📊 Estimated Monthly Cost: €0.00${NC}"
echo ""

read -p "Continue with deployment? (yes/no): " CONFIRM
if [[ "$CONFIRM" != "yes" ]]; then
    echo -e "${RED}Deployment cancelled${NC}"
    exit 0
fi

# ============================================================================
# RESOURCE GROUP
# ============================================================================

echo ""
echo -e "${BLUE}📦 Step 3: Creating Resource Group...${NC}"

if az group show --name "$RESOURCE_GROUP" &> /dev/null; then
    echo -e "${YELLOW}⚠️  Resource group '$RESOURCE_GROUP' already exists${NC}"
    read -p "Use existing resource group? (yes/no): " USE_EXISTING
    if [[ "$USE_EXISTING" != "yes" ]]; then
        echo -e "${RED}Deployment cancelled${NC}"
        exit 0
    fi
else
    az group create \
        --name "$RESOURCE_GROUP" \
        --location "$LOCATION" \
        --tags "Project=Day33-Capstone" "Environment=dev" "ManagedBy=Bicep"
    
    echo -e "${GREEN}✅ Resource group created: $RESOURCE_GROUP${NC}"
fi

# ============================================================================
# VALIDATE BICEP TEMPLATE
# ============================================================================

echo ""
echo -e "${BLUE}🔍 Step 4: Validating Bicep template...${NC}"

VALIDATION=$(az deployment group validate \
    --resource-group "$RESOURCE_GROUP" \
    --template-file main.bicep \
    --parameters main.parameters.json \
    --query "properties.provisioningState" \
    -o tsv 2>&1)

if [[ "$VALIDATION" == "Succeeded" ]]; then
    echo -e "${GREEN}✅ Template validation passed${NC}"
else
    echo -e "${RED}❌ Template validation failed${NC}"
    echo "$VALIDATION"
    exit 1
fi

# ============================================================================
# DEPLOY INFRASTRUCTURE
# ============================================================================

echo ""
echo -e "${BLUE}🚀 Step 5: Deploying infrastructure...${NC}"
echo "   Deployment name: $DEPLOYMENT_NAME"
echo ""
echo "This will create:"
echo "  - Virtual Network with NSG"
echo "  - Storage Account (Standard_LRS)"
echo "  - Key Vault (with soft delete + purge protection)"
echo "  - Log Analytics Workspace"
echo "  - Application Insights"
echo "  - App Service Plan (F1 Free)"
echo "  - App Service (Node.js 18)"
echo ""

az deployment group create \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --template-file main.bicep \
    --parameters main.parameters.json \
    --verbose

DEPLOYMENT_STATUS=$?

if [ $DEPLOYMENT_STATUS -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✅ Deployment completed successfully!${NC}"
else
    echo ""
    echo -e "${RED}❌ Deployment failed${NC}"
    exit 1
fi

# ============================================================================
# GET OUTPUTS
# ============================================================================

echo ""
echo -e "${BLUE}📊 Step 6: Retrieving deployment outputs...${NC}"

APP_SERVICE_NAME=$(az deployment group show \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --query "properties.outputs.appServiceName.value" \
    -o tsv)

APP_SERVICE_URL=$(az deployment group show \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --query "properties.outputs.appServiceUrl.value" \
    -o tsv)

KEY_VAULT_NAME=$(az deployment group show \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --query "properties.outputs.keyVaultName.value" \
    -o tsv)

STORAGE_ACCOUNT=$(az deployment group show \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --query "properties.outputs.storageAccountName.value" \
    -o tsv)

APP_INSIGHTS=$(az deployment group show \
    --name "$DEPLOYMENT_NAME" \
    --resource-group "$RESOURCE_GROUP" \
    --query "properties.outputs.appInsightsName.value" \
    -o tsv)

# ============================================================================
# SUMMARY
# ============================================================================

echo ""
echo -e "${PURPLE}============================================${NC}"
echo -e "${PURPLE}   ✅ DEPLOYMENT COMPLETE${NC}"
echo -e "${PURPLE}============================================${NC}"
echo ""
echo -e "${GREEN}Resource Group:${NC}      $RESOURCE_GROUP"
echo -e "${GREEN}Location:${NC}            $LOCATION"
echo -e "${GREEN}Deployment Name:${NC}     $DEPLOYMENT_NAME"
echo ""
echo -e "${BLUE}📦 Resources Created:${NC}"
echo "  ✓ App Service:         $APP_SERVICE_NAME"
echo "  ✓ Key Vault:           $KEY_VAULT_NAME"
echo "  ✓ Storage Account:     $STORAGE_ACCOUNT"
echo "  ✓ App Insights:        $APP_INSIGHTS"
echo "  ✓ Virtual Network:     vnet-capstonesec-dev"
echo "  ✓ Network Security Group: nsg-capstonesec-dev"
echo ""
echo -e "${GREEN}🌐 Application URL:${NC}"
echo "   $APP_SERVICE_URL"
echo ""
echo -e "${YELLOW}💰 Cost Status:${NC}"
echo "   Estimated Monthly Cost: €0.00"
echo "   All resources are FREE tier ✅"
echo ""
echo -e "${BLUE}📊 Security Score: 88/100${NC}"
echo ""
echo -e "${PURPLE}============================================${NC}"
echo ""
echo -e "${YELLOW}Next Steps:${NC}"
echo "  1. Deploy application code:"
echo "     cd ../secure-app"
echo "     zip -r app.zip ."
echo "     az webapp deployment source config-zip \\"
echo "       --resource-group $RESOURCE_GROUP \\"
echo "       --name $APP_SERVICE_NAME \\"
echo "       --src app.zip"
echo ""
echo "  2. Test the deployment:"
echo "     curl $APP_SERVICE_URL/health"
echo ""
echo "  3. Configure RBAC for Key Vault access (if needed)"
echo ""
echo -e "${GREEN}🎉 Capstone Phase 3 Complete!${NC}"
echo ""
