#!/bin/bash
# GitHub Container Registry (ghcr.io) Setup Script
# Day 37: Container Registry Security

set -e  # Exit on error

echo "🐙 GitHub Container Registry Setup"
echo "=================================="
echo ""

# Check if token is set
if [ -z "$GITHUB_TOKEN" ]; then
    echo "⚠️  GITHUB_TOKEN not set!"
    echo ""
    echo "Please create a token and set it:"
    echo "1. Go to: https://github.com/settings/tokens"
    echo "2. Generate new token (classic)"
    echo "3. Scopes: write:packages, read:packages, delete:packages"
    echo "4. Copy token"
    echo "5. Run: export GITHUB_TOKEN=your_token_here"
    echo ""
    exit 1
fi

# Get GitHub username
read -p "Enter your GitHub username [YomHubGG]: " GITHUB_USER
GITHUB_USER=${GITHUB_USER:-YomHubGG}

echo ""
echo "📦 Logging in to ghcr.io..."
echo $GITHUB_TOKEN | podman login ghcr.io -u $GITHUB_USER --password-stdin

if [ $? -eq 0 ]; then
    echo "✅ Login successful!"
else
    echo "❌ Login failed!"
    exit 1
fi

echo ""
echo "🏷️  Tagging your Day 35 image..."

# Check if Day 35 image exists
if podman images | grep -q "secure-app.*1.0.0"; then
    podman tag secure-app:1.0.0 ghcr.io/$GITHUB_USER/secure-app:1.0.0
    podman tag secure-app:1.0.0 ghcr.io/$GITHUB_USER/secure-app:latest
    echo "✅ Tagged: ghcr.io/$GITHUB_USER/secure-app:1.0.0"
    echo "✅ Tagged: ghcr.io/$GITHUB_USER/secure-app:latest"
else
    echo "⚠️  secure-app:1.0.0 not found!"
    echo "Build it first:"
    echo "  cd azure-security-mastery/02-security-services/day35-docker-security"
    echo "  podman build -f Dockerfile.secure -t secure-app:1.0.0 ."
    exit 1
fi

echo ""
read -p "📤 Push images to GitHub? (y/N): " PUSH
if [[ "$PUSH" =~ ^[Yy]$ ]]; then
    echo "Pushing ghcr.io/$GITHUB_USER/secure-app:1.0.0..."
    podman push ghcr.io/$GITHUB_USER/secure-app:1.0.0
    
    echo "Pushing ghcr.io/$GITHUB_USER/secure-app:latest..."
    podman push ghcr.io/$GITHUB_USER/secure-app:latest
    
    echo ""
    echo "✅ Images pushed successfully!"
    echo ""
    echo "📊 View your packages:"
    echo "   https://github.com/$GITHUB_USER?tab=packages"
    echo ""
    echo "🔍 Pull commands:"
    echo "   podman pull ghcr.io/$GITHUB_USER/secure-app:1.0.0"
    echo "   podman pull ghcr.io/$GITHUB_USER/secure-app:latest"
else
    echo "Skipped push. To push later:"
    echo "  podman push ghcr.io/$GITHUB_USER/secure-app:1.0.0"
fi

echo ""
echo "🎉 Setup complete!"
echo ""
echo "💡 Tips:"
echo "  - Set visibility: https://github.com/users/$GITHUB_USER/packages/container/secure-app/settings"
echo "  - Make it public for portfolio visibility!"
echo "  - Add description and README"
echo "  - Vulnerabilities auto-scanned by Dependabot"
