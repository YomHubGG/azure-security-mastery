#!/bin/bash
# GitHub Container Registry - Push Day 35 Secure Container
# Run this in NATIVE terminal (Ctrl+Alt+T), not VS Code terminal!
# Reason: VS Code terminal has "no new privileges" flag set

set -e

echo "=============================================="
echo "GitHub Container Registry - Push Script"
echo "=============================================="
echo ""

# Configuration
GITHUB_USER="yomhubgg"  # Must be lowercase for ghcr.io!
IMAGE_NAME="secure-app"
IMAGE_TAG="1.0.0"
DAY35_DIR="$HOME/cybersecurity-journey/azure-security-mastery/02-security-services/day35-docker-security"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Step 1: Check GitHub token
echo -e "${BLUE}Step 1: GitHub Token${NC}"
if [ -z "$CR_PAT" ]; then
    echo -e "${YELLOW}⚠️  CR_PAT not set. Please enter your GitHub Personal Access Token:${NC}"
    read -s CR_PAT
    export CR_PAT
fi
echo -e "${GREEN}✓ Token ready${NC}"
echo ""

# Step 2: Check if image exists locally
echo -e "${BLUE}Step 2: Check Local Image${NC}"
if podman images | grep -q "$IMAGE_NAME.*$IMAGE_TAG"; then
    echo -e "${GREEN}✓ Found: $IMAGE_NAME:$IMAGE_TAG${NC}"
    REBUILD=false
else
    echo -e "${YELLOW}⚠️  Image not found. Need to rebuild.${NC}"
    REBUILD=true
fi
echo ""

# Step 3: Rebuild if needed
if [ "$REBUILD" = true ]; then
    echo -e "${BLUE}Step 3: Building Image${NC}"
    cd "$DAY35_DIR" || exit 1
    
    if [ ! -f "Dockerfile.secure" ]; then
        echo -e "${RED}✗ Dockerfile.secure not found in $DAY35_DIR${NC}"
        exit 1
    fi
    
    echo "Building from Dockerfile.secure..."
    podman build -f Dockerfile.secure -t "$IMAGE_NAME:$IMAGE_TAG" .
    echo -e "${GREEN}✓ Build complete${NC}"
    echo ""
else
    echo -e "${BLUE}Step 3: Build${NC}"
    echo -e "${GREEN}✓ Skipped (image exists)${NC}"
    echo ""
fi

# Step 4: Login to GitHub Container Registry
echo -e "${BLUE}Step 4: GitHub CR Login${NC}"
echo "$CR_PAT" | podman login ghcr.io -u "$GITHUB_USER" --password-stdin
echo -e "${GREEN}✓ Authenticated to ghcr.io${NC}"
echo ""

# Step 5: Tag for GitHub
echo -e "${BLUE}Step 5: Tag Image${NC}"
podman tag "$IMAGE_NAME:$IMAGE_TAG" "ghcr.io/$GITHUB_USER/$IMAGE_NAME:$IMAGE_TAG"
podman tag "$IMAGE_NAME:$IMAGE_TAG" "ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest"
echo -e "${GREEN}✓ Tagged:${NC}"
echo "  - ghcr.io/$GITHUB_USER/$IMAGE_NAME:$IMAGE_TAG"
echo "  - ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest"
echo ""

# Step 6: Push to GitHub
echo -e "${BLUE}Step 6: Push to GitHub CR${NC}"
echo "Pushing $IMAGE_TAG..."
podman push "ghcr.io/$GITHUB_USER/$IMAGE_NAME:$IMAGE_TAG"
echo ""
echo "Pushing latest..."
podman push "ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest"
echo -e "${GREEN}✓ Push complete!${NC}"
echo ""

# Step 7: Verify
echo -e "${BLUE}Step 7: Verification${NC}"
echo "View your package at:"
echo -e "${GREEN}👉 https://github.com/$GITHUB_USER?tab=packages${NC}"
echo ""
echo "Pull command for others:"
echo -e "${YELLOW}podman pull ghcr.io/$GITHUB_USER/$IMAGE_NAME:$IMAGE_TAG${NC}"
echo ""

# Step 8: Cleanup token from environment
echo -e "${BLUE}Step 8: Security Cleanup${NC}"
unset CR_PAT
echo -e "${GREEN}✓ Token removed from environment${NC}"
echo ""

echo "=============================================="
echo -e "${GREEN}✅ SUCCESS! Container pushed to GitHub CR${NC}"
echo "=============================================="
echo ""
echo "Next steps:"
echo "1. Make package public (if desired):"
echo "   - Go to https://github.com/$GITHUB_USER?tab=packages"
echo "   - Click on $IMAGE_NAME"
echo "   - Settings → Danger Zone → Change visibility → Public"
echo ""
echo "2. Add to README.md:"
echo "   ![Container](https://img.shields.io/badge/container-ghcr.io-blue)"
echo ""
echo "3. Use in Day 39 (Azure Container Instances):"
echo "   az container create --image ghcr.io/$GITHUB_USER/$IMAGE_NAME:$IMAGE_TAG"
