#!/usr/bin/env bash
set -euo pipefail

# azure-hardening-audit.sh
# Automated Azure security hardening audit script
# Usage: ./azure-hardening-audit.sh

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔒 Azure Security Hardening Audit${NC}"
echo "=================================="
echo ""

# Check Azure CLI authentication
if ! az account show &>/dev/null; then
    echo -e "${RED}❌ Not authenticated to Azure CLI${NC}"
    echo "Run: az login"
    exit 2
fi

SUBSCRIPTION=$(az account show --query name -o tsv)
echo -e "📋 Subscription: ${BLUE}$SUBSCRIPTION${NC}"
echo ""

# 1. Check Storage Accounts for HTTPS enforcement
echo -e "${BLUE}📦 Checking Storage Accounts...${NC}"
STORAGE_ACCOUNTS=$(az storage account list --query "[].{Name:name, HTTPS:supportsHttpsTrafficOnly, TLS:minimumTlsVersion, PublicAccess:publicNetworkAccess}" -o json)

STORAGE_COUNT=$(echo "$STORAGE_ACCOUNTS" | jq 'length')

if [ "$STORAGE_COUNT" -eq 0 ]; then
    echo -e "  ${YELLOW}⚠️  No storage accounts found${NC}"
else
    HTTPS_ENABLED=$(echo "$STORAGE_ACCOUNTS" | jq '[.[] | select(.HTTPS==true)] | length')
    TLS12_ENABLED=$(echo "$STORAGE_ACCOUNTS" | jq '[.[] | select(.TLS=="TLS1_2")] | length')
    PUBLIC_DISABLED=$(echo "$STORAGE_ACCOUNTS" | jq '[.[] | select(.PublicAccess=="Disabled")] | length')

    if [ "$HTTPS_ENABLED" -eq "$STORAGE_COUNT" ]; then
      echo -e "  ${GREEN}✅ HTTPS enforcement: $HTTPS_ENABLED/$STORAGE_COUNT${NC}"
    else
      echo -e "  ${RED}❌ HTTPS enforcement: $HTTPS_ENABLED/$STORAGE_COUNT${NC}"
      echo "$STORAGE_ACCOUNTS" | jq -r '.[] | select(.HTTPS==false) | "     - \(.Name): HTTPS disabled"'
    fi

    if [ "$TLS12_ENABLED" -eq "$STORAGE_COUNT" ]; then
      echo -e "  ${GREEN}✅ TLS 1.2 minimum: $TLS12_ENABLED/$STORAGE_COUNT${NC}"
    else
      echo -e "  ${YELLOW}⚠️  TLS 1.2 minimum: $TLS12_ENABLED/$STORAGE_COUNT${NC}"
      echo "$STORAGE_ACCOUNTS" | jq -r '.[] | select(.TLS!="TLS1_2") | "     - \(.Name): \(.TLS // "Not set")"'
    fi

    if [ "$PUBLIC_DISABLED" -eq "$STORAGE_COUNT" ]; then
      echo -e "  ${GREEN}✅ Public access disabled: $PUBLIC_DISABLED/$STORAGE_COUNT${NC}"
    else
      echo -e "  ${YELLOW}⚠️  Public access disabled: $PUBLIC_DISABLED/$STORAGE_COUNT${NC}"
      echo "$STORAGE_ACCOUNTS" | jq -r '.[] | select(.PublicAccess!="Disabled") | "     - \(.Name): \(.PublicAccess)"'
    fi
fi

# 2. Check Key Vaults for soft delete
echo ""
echo -e "${BLUE}🔑 Checking Key Vaults...${NC}"
KEY_VAULTS=$(az keyvault list --query "[].{Name:name, SoftDelete:properties.enableSoftDelete, PurgeProtection:properties.enablePurgeProtection}" -o json 2>/dev/null || echo "[]")

KV_COUNT=$(echo "$KEY_VAULTS" | jq 'length')

if [ "$KV_COUNT" -eq 0 ]; then
    echo -e "  ${YELLOW}⚠️  No key vaults found${NC}"
else
    SOFT_DELETE=$(echo "$KEY_VAULTS" | jq '[.[] | select(.SoftDelete==true)] | length')
    PURGE_PROTECT=$(echo "$KEY_VAULTS" | jq '[.[] | select(.PurgeProtection==true)] | length')

    if [ "$SOFT_DELETE" -eq "$KV_COUNT" ]; then
      echo -e "  ${GREEN}✅ Soft delete: $SOFT_DELETE/$KV_COUNT${NC}"
    else
      echo -e "  ${RED}❌ Soft delete: $SOFT_DELETE/$KV_COUNT${NC}"
      echo "$KEY_VAULTS" | jq -r '.[] | select(.SoftDelete==false or .SoftDelete==null) | "     - \(.Name): Soft delete disabled"'
    fi

    if [ "$PURGE_PROTECT" -eq "$KV_COUNT" ]; then
      echo -e "  ${GREEN}✅ Purge protection: $PURGE_PROTECT/$KV_COUNT${NC}"
    else
      echo -e "  ${YELLOW}⚠️  Purge protection: $PURGE_PROTECT/$KV_COUNT${NC}"
      echo "$KEY_VAULTS" | jq -r '.[] | select(.PurgeProtection==false or .PurgeProtection==null) | "     - \(.Name): Purge protection disabled"'
    fi
fi

# 3. Check Network Security Groups for permissive rules
echo ""
echo -e "${BLUE}🛡️  Checking Network Security Groups...${NC}"
NSGS=$(az network nsg list --query "[].{Name:name}" -o json 2>/dev/null || echo "[]")
NSG_COUNT=$(echo "$NSGS" | jq 'length')

if [ "$NSG_COUNT" -eq 0 ]; then
    echo -e "  ${YELLOW}⚠️  No NSGs found (or Azure CLI error)${NC}"
else
    PERMISSIVE_COUNT=0
    
    while IFS= read -r nsg_name; do
        # Check for rules allowing 0.0.0.0/0 or * (suppress errors)
        PERMISSIVE_RULES=$(az network nsg show --name "$nsg_name" --query "securityRules[?sourceAddressPrefix=='*' || sourceAddressPrefix=='0.0.0.0/0'].{Name:name, Access:access, Direction:direction, DestPort:destinationPortRange}" -o json 2>/dev/null || echo "[]")
        
        RULE_COUNT=$(echo "$PERMISSIVE_RULES" | jq 'length')
        
        if [ "$RULE_COUNT" -gt 0 ]; then
            ((PERMISSIVE_COUNT++))
        fi
    done < <(echo "$NSGS" | jq -r '.[].Name')
    
    if [ "$PERMISSIVE_COUNT" -eq 0 ]; then
        echo -e "  ${GREEN}✅ No permissive NSG rules (0.0.0.0/0)${NC}"
    else
        echo -e "  ${YELLOW}⚠️  $PERMISSIVE_COUNT NSGs with permissive rules${NC}"
    fi
fi

# 4. Check public network access
echo ""
echo -e "${BLUE}🌐 Checking Public Network Access...${NC}"

if [ "$STORAGE_COUNT" -gt 0 ]; then
    PUBLIC_STORAGE=$(az storage account list --query "[?publicNetworkAccess=='Enabled'].name" -o tsv | wc -l)

    if [ "$PUBLIC_STORAGE" -eq 0 ]; then
      echo -e "  ${GREEN}✅ No storage accounts with public access${NC}"
    else
      echo -e "  ${YELLOW}⚠️  $PUBLIC_STORAGE storage accounts have public access${NC}"
      az storage account list --query "[?publicNetworkAccess=='Enabled'].name" -o tsv | while read name; do
          echo "     - $name"
      done
    fi
else
    echo -e "  ${YELLOW}⚠️  No storage accounts to check${NC}"
fi

# 5. Secure Score (if Defender enabled)
echo ""
echo -e "${BLUE}📊 Microsoft Defender Secure Score...${NC}"

# Check if Defender is enabled first
DEFENDER_ENABLED=$(az security pricing list --query "[?name=='VirtualMachines'].pricingTier" -o tsv 2>/dev/null || echo "")

if [ -z "$DEFENDER_ENABLED" ]; then
    echo -e "  ${YELLOW}⚠️  Microsoft Defender for Cloud not available${NC}"
else
    SECURE_SCORE=$(az security secure-scores list --query "[0].properties.score.percentage" -o tsv 2>/dev/null || echo "N/A")

    if [ "$SECURE_SCORE" != "N/A" ] && [ -n "$SECURE_SCORE" ]; then
      SCORE=$(echo "$SECURE_SCORE" | awk '{printf "%.0f", $1}')
      
      if [ "$SCORE" -ge 90 ]; then
        echo -e "  ${GREEN}✅ Secure Score: ${SCORE}%${NC}"
      elif [ "$SCORE" -ge 70 ]; then
        echo -e "  ${YELLOW}⚠️  Secure Score: ${SCORE}%${NC}"
      else
        echo -e "  ${RED}❌ Secure Score: ${SCORE}%${NC}"
      fi
    else
      echo -e "  ${YELLOW}⚠️  Secure Score: Not available${NC}"
    fi
fi

# 6. Summary
echo ""
echo "=================================="
TOTAL_CHECKS=0
PASSED_CHECKS=0

# Storage checks
if [ "$STORAGE_COUNT" -gt 0 ]; then
    TOTAL_CHECKS=$((TOTAL_CHECKS + 3))
    [ "$HTTPS_ENABLED" -eq "$STORAGE_COUNT" ] && ((PASSED_CHECKS++))
    [ "$TLS12_ENABLED" -eq "$STORAGE_COUNT" ] && ((PASSED_CHECKS++))
    [ "$PUBLIC_DISABLED" -eq "$STORAGE_COUNT" ] && ((PASSED_CHECKS++))
fi

# Key Vault checks
if [ "$KV_COUNT" -gt 0 ]; then
    TOTAL_CHECKS=$((TOTAL_CHECKS + 2))
    [ "$SOFT_DELETE" -eq "$KV_COUNT" ] && ((PASSED_CHECKS++))
    [ "$PURGE_PROTECT" -eq "$KV_COUNT" ] && ((PASSED_CHECKS++))
fi

# NSG check
if [ "$NSG_COUNT" -gt 0 ]; then
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    [ "$PERMISSIVE_COUNT" -eq 0 ] && ((PASSED_CHECKS++))
fi

if [ "$TOTAL_CHECKS" -eq 0 ]; then
    echo -e "${YELLOW}⚠️  No resources found to audit${NC}"
    echo "Deploy some Azure resources first!"
    exit 1
fi

PASS_RATE=$((PASSED_CHECKS * 100 / TOTAL_CHECKS))

echo -e "${BLUE}✅ Passed: $PASSED_CHECKS/$TOTAL_CHECKS ($PASS_RATE%)${NC}"
echo ""

if [ "$PASS_RATE" -ge 90 ]; then
  echo -e "${GREEN}🎉 Excellent security posture!${NC}"
  exit 0
elif [ "$PASS_RATE" -ge 70 ]; then
  echo -e "${YELLOW}⚠️  Good, but improvements needed${NC}"
  exit 1
else
  echo -e "${RED}❌ Critical hardening required${NC}"
  exit 2
fi
