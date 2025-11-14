#!/bin/bash
# Quick cleanup script for Day 59 Q2 Capstone
# Run this immediately after testing to avoid costs!

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

RESOURCE_GROUP="rg-learning-day1"
CONTAINER_NAME="securecloud-q2-capstone"

echo -e "${YELLOW}🧹 Cleaning up Q2 Capstone deployment...${NC}"
echo ""

# Check if container exists
if az container show --resource-group "$RESOURCE_GROUP" --name "$CONTAINER_NAME" &>/dev/null; then
    echo -e "${YELLOW}🗑️  Deleting container: $CONTAINER_NAME${NC}"
    az container delete --resource-group "$RESOURCE_GROUP" --name "$CONTAINER_NAME" --yes
    echo -e "${GREEN}✅ Container deleted successfully!${NC}"
    echo ""
    echo -e "${GREEN}💰 Cost saved! No charges will accrue.${NC}"
else
    echo -e "${YELLOW}⚠️  Container not found: $CONTAINER_NAME${NC}"
    echo -e "   (May have already been deleted)"
fi

echo ""
echo -e "${BLUE}📊 Deployment Duration:${NC}"
echo -e "   Keep it under 5 minutes for minimal cost (~€0.05)"
echo ""
echo -e "${GREEN}✅ Cleanup complete!${NC}"
