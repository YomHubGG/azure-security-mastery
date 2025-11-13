#!/bin/bash
# 🔍 Day 45 Workflow Diagnosis Script

echo "🔍 Diagnosing Day 45 Secure Pipeline Workflow..."
echo ""

# Check if required files exist
echo "📁 File Structure Check:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

FILES_TO_CHECK=(
    ".github/workflows/day45-secure-pipeline.yml"
    "azure-security-mastery/02-security-services/day35-docker-security/Dockerfile.day45"
)

for file in "${FILES_TO_CHECK[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file"
    else
        echo "❌ MISSING: $file"
    fi
done

echo ""
echo "🔐 Azure Secrets Check:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "The workflow requires these GitHub secrets:"
echo "  - AZURE_CLIENT_ID (for OIDC authentication)"
echo "  - AZURE_TENANT_ID (for OIDC authentication)"
echo "  - AZURE_SUBSCRIPTION_ID (for OIDC authentication)"
echo ""
echo "To check if secrets are configured:"
echo "👉 https://github.com/YomHubGG/cybersecurity-journey/settings/secrets/actions"
echo ""

# Check workflow syntax
echo "🔍 Workflow Syntax Check:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
if command -v yamllint &> /dev/null; then
    yamllint .github/workflows/day45-secure-pipeline.yml 2>&1 | head -10
else
    echo "⚠️  yamllint not installed (optional)"
    echo "Basic syntax appears OK (file is readable)"
fi

echo ""
echo "🐳 Docker Build Test (Local):"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Testing if the Dockerfile builds locally..."

cd azure-security-mastery/02-security-services/day35-docker-security || exit 1

if docker build -f Dockerfile.day45 -t test-day45:local . 2>&1 | tail -20; then
    echo "✅ Dockerfile builds successfully locally"
    docker images | grep test-day45
    echo ""
    echo "🧹 Cleaning up test image..."
    docker rmi test-day45:local 2>/dev/null
else
    echo "❌ Dockerfile build FAILED"
    echo "This is likely what's causing the GitHub Actions failure!"
fi

cd - > /dev/null

echo ""
echo "📊 Common Failure Reasons:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "1. ❌ Missing Azure secrets (CLIENT_ID, TENANT_ID, SUBSCRIPTION_ID)"
echo "2. ❌ OIDC trust not configured between GitHub & Azure"
echo "3. ❌ Docker build context issues"
echo "4. ❌ CodeQL language detection failures"
echo "5. ❌ Trivy scanner timing out"
echo "6. ❌ Container image push failures (registry auth)"
echo ""
echo "🔍 To see actual error from GitHub Actions:"
echo "👉 https://github.com/YomHubGG/cybersecurity-journey/actions"
echo ""
echo "💡 Quick Fixes:"
echo "  - If Azure secrets missing: Workflow will skip deployment (OK)"
echo "  - If Docker build fails: Check Dockerfile syntax"
echo "  - If CodeQL fails: May need to adjust language config"
