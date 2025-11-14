#!/bin/bash
# ============================================================================
# Day 59 Q2 Capstone: Deployment Script
# Deploys SecureCloud DevSecOps Platform to Azure
# ============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
RESOURCE_GROUP="rg-learning-day1"
LOCATION="westeurope"
DEPLOYMENT_NAME="securecloud-q2-capstone-$(date +%Y%m%d-%H%M%S)"
BICEP_FILE="infrastructure/container-instance.bicep"
CONTAINER_IMAGE="ghcr.io/yomhubgg/securecloud-devsecops:latest"

echo -e "${BLUE}════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  Q2 Capstone: SecureCloud DevSecOps Platform Deployment ${NC}"
echo -e "${BLUE}════════════════════════════════════════════════════════════${NC}"
echo ""

# Step 1: Check Azure CLI authentication
echo -e "${YELLOW}🔐 Step 1: Checking Azure authentication...${NC}"
if ! az account show &>/dev/null; then
    echo -e "${RED}❌ Not logged in to Azure. Please run: az login${NC}"
    exit 1
fi

SUBSCRIPTION=$(az account show --query name -o tsv)
echo -e "${GREEN}✅ Logged in to Azure${NC}"
echo -e "   Subscription: ${SUBSCRIPTION}"
echo ""

# Step 2: Verify resource group exists
echo -e "${YELLOW}📦 Step 2: Verifying resource group...${NC}"
if az group show --name "$RESOURCE_GROUP" &>/dev/null; then
    echo -e "${GREEN}✅ Resource group exists: ${RESOURCE_GROUP}${NC}"
else
    echo -e "${RED}❌ Resource group not found: ${RESOURCE_GROUP}${NC}"
    echo -e "${YELLOW}   Creating resource group...${NC}"
    az group create --name "$RESOURCE_GROUP" --location "$LOCATION"
    echo -e "${GREEN}✅ Resource group created${NC}"
fi
echo ""

# Step 3: Validate Bicep template
echo -e "${YELLOW}🔍 Step 3: Validating Bicep template...${NC}"
az deployment group validate \
    --resource-group "$RESOURCE_GROUP" \
    --template-file "$BICEP_FILE" \
    --parameters containerImage="$CONTAINER_IMAGE" \
    --output none

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Bicep template is valid${NC}"
else
    echo -e "${RED}❌ Bicep template validation failed${NC}"
    exit 1
fi
echo ""

# Step 4: Preview changes (What-If)
echo -e "${YELLOW}👀 Step 4: Previewing deployment changes...${NC}"
echo -e "${BLUE}   (Running 'az deployment group what-if')${NC}"
az deployment group what-if \
    --resource-group "$RESOURCE_GROUP" \
    --template-file "$BICEP_FILE" \
    --parameters containerImage="$CONTAINER_IMAGE" \
    --no-pretty-print \
    | head -30

echo ""
echo -e "${YELLOW}⚠️  This deployment will use FREE TIER resources (€0.00)${NC}"
echo -e "${YELLOW}   7-minute runtime limit will apply.${NC}"
echo ""

# Step 5: Confirm deployment
read -p "$(echo -e ${BLUE}Do you want to proceed with deployment? [y/N]:${NC} )" -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}⏹️  Deployment cancelled${NC}"
    exit 0
fi
echo ""

# Step 6: Deploy to Azure
echo -e "${YELLOW}🚀 Step 6: Deploying to Azure...${NC}"
echo -e "${BLUE}   This may take 2-3 minutes...${NC}"

DEPLOYMENT_OUTPUT=$(az deployment group create \
    --resource-group "$RESOURCE_GROUP" \
    --name "$DEPLOYMENT_NAME" \
    --template-file "$BICEP_FILE" \
    --parameters containerImage="$CONTAINER_IMAGE" \
    --query properties.outputs \
    --output json)

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Deployment successful!${NC}"
else
    echo -e "${RED}❌ Deployment failed${NC}"
    exit 1
fi
echo ""

# Step 7: Extract outputs
echo -e "${YELLOW}📊 Step 7: Extracting deployment information...${NC}"

FQDN=$(echo "$DEPLOYMENT_OUTPUT" | jq -r '.fqdn.value')
URL=$(echo "$DEPLOYMENT_OUTPUT" | jq -r '.url.value')
IP_ADDRESS=$(echo "$DEPLOYMENT_OUTPUT" | jq -r '.ipAddress.value')
IDENTITY_ID=$(echo "$DEPLOYMENT_OUTPUT" | jq -r '.identityPrincipalId.value')

echo ""
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  🎉 Deployment Complete!${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "${BLUE}Container URL:${NC}      ${URL}"
echo -e "${BLUE}FQDN:${NC}               ${FQDN}"
echo -e "${BLUE}IP Address:${NC}         ${IP_ADDRESS}"
echo -e "${BLUE}Identity ID:${NC}        ${IDENTITY_ID}"
echo -e "${BLUE}Deployment:${NC}         ${DEPLOYMENT_NAME}"
echo ""

# Step 8: Health check
echo -e "${YELLOW}🏥 Step 8: Performing health check...${NC}"
echo -e "${BLUE}   Waiting 15 seconds for container to start...${NC}"
sleep 15

HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" "$URL/health" || echo "000")

if [ "$HTTP_CODE" == "200" ]; then
    echo -e "${GREEN}✅ Health check PASSED (HTTP $HTTP_CODE)${NC}"
    
    echo ""
    echo -e "${BLUE}Testing endpoints:${NC}"
    echo -e "${YELLOW}  • Health:${NC} $URL/health"
    curl -s "$URL/health" | jq .
    
    echo ""
    echo -e "${YELLOW}  • Ready:${NC} $URL/ready"
    curl -s "$URL/ready" | jq .
    
    echo ""
    echo -e "${YELLOW}  • Metrics:${NC} $URL/metrics"
    curl -s "$URL/metrics" | jq .
else
    echo -e "${YELLOW}⚠️  Health check returned HTTP $HTTP_CODE${NC}"
    echo -e "${YELLOW}   Container may still be starting up...${NC}"
    echo -e "${YELLOW}   Try visiting: $URL${NC}"
fi

echo ""
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  ✅ SecureCloud DevSecOps Platform is LIVE!${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "${BLUE}📝 Next Steps:${NC}"
echo -e "   1. Visit: ${URL}"
echo -e "   2. Test endpoints: /health, /ready, /metrics"
echo -e "   3. View in Azure Portal"
echo -e "   4. Run monitoring scripts"
echo ""
echo -e "${YELLOW}🧹 To clean up:${NC}"
echo -e "   az container delete --resource-group $RESOURCE_GROUP --name securecloud-q2-capstone --yes"
echo ""
echo -e "${YELLOW}⚠️  Remember: 7-minute free tier limit!${NC}"
echo -e "   Monitor your usage to stay within free tier."
echo ""
