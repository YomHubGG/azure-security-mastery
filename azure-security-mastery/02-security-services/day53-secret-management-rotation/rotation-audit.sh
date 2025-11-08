#!/bin/bash
# 🔐 Secret Rotation Audit Script
# Checks all Key Vault secrets for expiration dates and rotation status
# Usage: ./rotation-audit.sh [vault-name] [warning-days]

set -euo pipefail

# Configuration
VAULT_NAME="${1:-kvlearning4uybw3c2lbkwm}"
WARNING_DAYS="${2:-30}"

# Colors for output
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔐 Key Vault Secret Rotation Audit${NC}"
echo "Vault: $VAULT_NAME"
echo "Warning threshold: $WARNING_DAYS days"
echo ""

# Check if Azure CLI is authenticated
if ! az account show &>/dev/null; then
  echo -e "${RED}❌ Not authenticated with Azure CLI${NC}"
  echo "Run: az login"
  exit 1
fi

# Check if Key Vault exists
if ! az keyvault show --name "$VAULT_NAME" &>/dev/null; then
  echo -e "${RED}❌ Key Vault '$VAULT_NAME' not found${NC}"
  exit 1
fi

# Get all secrets
echo -e "${BLUE}📋 Fetching secrets from Key Vault...${NC}"
SECRETS=$(az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "[].{name:name, expires:attributes.expires, enabled:attributes.enabled}" \
  --output json)

if [ "$(echo "$SECRETS" | jq length)" -eq 0 ]; then
  echo -e "${YELLOW}⚠️  No secrets found in Key Vault${NC}"
  exit 0
fi

# Calculate timestamps
NOW=$(date +%s)
WARNING_THRESHOLD=$(($NOW + ($WARNING_DAYS * 86400)))
CRITICAL_THRESHOLD=$(($NOW + (7 * 86400)))

# Counters
HEALTHY=0
WARNING=0
CRITICAL=0
EXPIRED=0
NO_EXPIRATION=0

echo -e "${BLUE}📊 Secret Status:${NC}"
echo ""

# Parse each secret
echo "$SECRETS" | jq -r '.[] | @json' | while IFS= read -r secret; do
  NAME=$(echo "$secret" | jq -r '.name')
  EXPIRES=$(echo "$secret" | jq -r '.expires // "null"')
  ENABLED=$(echo "$secret" | jq -r '.enabled')
  
  # Check if secret is disabled
  if [ "$ENABLED" = "false" ]; then
    echo -e "${YELLOW}⏸  $NAME - DISABLED${NC}"
    continue
  fi
  
  # Check if expiration is set
  if [ "$EXPIRES" = "null" ]; then
    echo -e "${YELLOW}⚠️  $NAME - NO EXPIRATION SET (action required)${NC}"
    NO_EXPIRATION=$((NO_EXPIRATION + 1))
    continue
  fi
  
  # Calculate days until expiry
  EXPIRY_TIMESTAMP=$(date -d "$EXPIRES" +%s 2>/dev/null || echo 0)
  
  if [ $EXPIRY_TIMESTAMP -eq 0 ]; then
    echo -e "${YELLOW}⚠️  $NAME - Invalid expiration date${NC}"
    continue
  fi
  
  DAYS_UNTIL_EXPIRY=$(( ($EXPIRY_TIMESTAMP - $NOW) / 86400 ))
  
  # Categorize and display
  if [ $DAYS_UNTIL_EXPIRY -lt 0 ]; then
    echo -e "${RED}🔴 $NAME - EXPIRED ${DAYS_UNTIL_EXPIRY#-} days ago (rotate NOW)${NC}"
    EXPIRED=$((EXPIRED + 1))
  elif [ $DAYS_UNTIL_EXPIRY -lt 7 ]; then
    echo -e "${RED}🚨 $NAME - Expires in $DAYS_UNTIL_EXPIRY days (CRITICAL - rotate immediately)${NC}"
    CRITICAL=$((CRITICAL + 1))
  elif [ $DAYS_UNTIL_EXPIRY -lt $WARNING_DAYS ]; then
    echo -e "${YELLOW}🟡 $NAME - Expires in $DAYS_UNTIL_EXPIRY days (rotate soon)${NC}"
    WARNING=$((WARNING + 1))
  else
    echo -e "${GREEN}✅ $NAME - Expires in $DAYS_UNTIL_EXPIRY days (healthy)${NC}"
    HEALTHY=$((HEALTHY + 1))
  fi
done

echo ""
echo -e "${BLUE}📋 Rotation Policy Summary:${NC}"

# Count secrets with/without expiration
WITH_EXPIRATION=$(echo "$SECRETS" | jq '[.[] | select(.expires != null)] | length')
WITHOUT_EXPIRATION=$(echo "$SECRETS" | jq '[.[] | select(.expires == null)] | length')
TOTAL_SECRETS=$(echo "$SECRETS" | jq length)

echo "Total secrets: $TOTAL_SECRETS"
echo -e "${GREEN}Secrets with expiration: $WITH_EXPIRATION${NC}"
echo -e "${YELLOW}Secrets WITHOUT expiration: $WITHOUT_EXPIRATION${NC}"

echo ""
echo -e "${BLUE}🎯 Action Required:${NC}"

if [ $EXPIRED -gt 0 ]; then
  echo -e "${RED}🔴 $EXPIRED expired secrets - ROTATE IMMEDIATELY${NC}"
fi

if [ $CRITICAL -gt 0 ]; then
  echo -e "${RED}🚨 $CRITICAL secrets expire within 7 days - URGENT${NC}"
fi

if [ $WARNING -gt 0 ]; then
  echo -e "${YELLOW}🟡 $WARNING secrets expire within $WARNING_DAYS days - Schedule rotation${NC}"
fi

if [ $NO_EXPIRATION -gt 0 ]; then
  echo -e "${YELLOW}⚠️  $NO_EXPIRATION secrets without expiration - Set rotation policy${NC}"
fi

if [ $EXPIRED -eq 0 ] && [ $CRITICAL -eq 0 ] && [ $WARNING -eq 0 ] && [ $NO_EXPIRATION -eq 0 ]; then
  echo -e "${GREEN}✅ All secrets are healthy${NC}"
fi

echo ""
echo -e "${BLUE}💡 Remediation Commands:${NC}"
echo ""
echo "# Set expiration on a secret (90 days):"
echo "az keyvault secret set \\"
echo "  --vault-name $VAULT_NAME \\"
echo "  --name <secret-name> \\"
echo "  --value '<secret-value>' \\"
echo "  --expires \"\$(date -u -d '+90 days' '+%Y-%m-%dT%H:%M:%SZ')\""
echo ""
echo "# Rotate a secret (generate new value):"
echo "az keyvault secret set \\"
echo "  --vault-name $VAULT_NAME \\"
echo "  --name <secret-name> \\"
echo "  --value '<new-secret-value>' \\"
echo "  --expires \"\$(date -u -d '+90 days' '+%Y-%m-%dT%H:%M:%SZ')\""
echo ""

# Exit code for monitoring systems
if [ $EXPIRED -gt 0 ] || [ $CRITICAL -gt 0 ]; then
  exit 2  # Critical
elif [ $WARNING -gt 0 ] || [ $NO_EXPIRATION -gt 0 ]; then
  exit 1  # Warning
else
  exit 0  # OK
fi
