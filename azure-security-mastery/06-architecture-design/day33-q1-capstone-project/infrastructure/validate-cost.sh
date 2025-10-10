#!/bin/bash

# ============================================================================
# 💰 COST SAFETY VALIDATOR - Day 33 Capstone
# ============================================================================
# Validates that ALL resources in Bicep template are FREE tier
# Prevents accidental deployment of paid resources
# ============================================================================

echo ""
echo "🔍 Cost Safety Validation for Day 33 Capstone"
echo "=============================================="
echo ""

BICEP_FILE="main.bicep"
ERRORS=0

# Check if bicep file exists
if [ ! -f "$BICEP_FILE" ]; then
    echo "❌ Error: $BICEP_FILE not found"
    exit 1
fi

echo "✅ Found $BICEP_FILE"
echo ""
echo "Validating resource SKUs..."
echo ""

# ============================================================================
# VALIDATE APP SERVICE PLAN
# ============================================================================

if grep -q "name: 'F1'" "$BICEP_FILE"; then
    echo "✅ App Service Plan: F1 (FREE tier) - OK"
else
    echo "❌ App Service Plan: NOT F1 - DANGER!"
    ERRORS=$((ERRORS + 1))
fi

# ============================================================================
# VALIDATE STORAGE ACCOUNT
# ============================================================================

if grep -q "name: 'Standard_LRS'" "$BICEP_FILE"; then
    echo "✅ Storage Account: Standard_LRS (FREE 5GB) - OK"
else
    echo "❌ Storage Account: NOT Standard_LRS - CHECK COST!"
    ERRORS=$((ERRORS + 1))
fi

# ============================================================================
# VALIDATE KEY VAULT
# ============================================================================

if grep -q "name: 'standard'" "$BICEP_FILE" && grep -q "Microsoft.KeyVault" "$BICEP_FILE"; then
    echo "✅ Key Vault: Standard (10K ops free) - OK"
else
    echo "❌ Key Vault: NOT standard tier - CHECK COST!"
    ERRORS=$((ERRORS + 1))
fi

# ============================================================================
# CHECK FOR DANGEROUS RESOURCES
# ============================================================================

echo ""
echo "Checking for expensive resources..."
echo ""

DANGEROUS_RESOURCES=(
    "Microsoft.ContainerRegistry"
    "Microsoft.Sql/servers"
    "Microsoft.DBforPostgreSQL"
    "Microsoft.DBforMySQL"
    "Microsoft.Compute/virtualMachines"
    "Microsoft.ContainerService"
    "Microsoft.Cache/Redis"
    "Premium"
    "Standard_ZRS"
    "Standard_GRS"
)

for RESOURCE in "${DANGEROUS_RESOURCES[@]}"; do
    if grep -q "$RESOURCE" "$BICEP_FILE"; then
        echo "⚠️  WARNING: Found potentially expensive resource: $RESOURCE"
        ERRORS=$((ERRORS + 1))
    fi
done

if [ $ERRORS -eq 0 ]; then
    echo "✅ No expensive resources detected"
fi

# ============================================================================
# VALIDATE SECURITY FEATURES
# ============================================================================

echo ""
echo "Validating security features..."
echo ""

SECURITY_CHECKS=(
    "httpsOnly: true:HTTPS enforcement"
    "minimumTlsVersion: 'TLS1_2':TLS 1.2"
    "enableSoftDelete: true:Key Vault soft delete"
    "enablePurgeProtection: true:Key Vault purge protection"
    "allowBlobPublicAccess: false:No public blob access"
    "supportsHttpsTrafficOnly: true:Storage HTTPS only"
)

for CHECK in "${SECURITY_CHECKS[@]}"; do
    IFS=':' read -r PATTERN DESC <<< "$CHECK"
    if grep -q "$PATTERN" "$BICEP_FILE"; then
        echo "✅ $DESC"
    else
        echo "⚠️  Missing: $DESC"
    fi
done

# ============================================================================
# SUMMARY
# ============================================================================

echo ""
echo "=============================================="
echo ""

if [ $ERRORS -eq 0 ]; then
    echo "✅ COST VALIDATION PASSED"
    echo ""
    echo "📊 Estimated Monthly Cost: €0.00"
    echo ""
    echo "Safe to deploy! All resources are FREE tier."
    echo ""
    exit 0
else
    echo "❌ COST VALIDATION FAILED"
    echo ""
    echo "Found $ERRORS potential cost issue(s)."
    echo "Please review the template before deploying!"
    echo ""
    exit 1
fi
