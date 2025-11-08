#!/bin/bash
# 🔐 Managed Identity Security Audit
# Audits all Managed Identities for security best practices
# Usage: ./managed-identity-audit.sh

set -euo pipefail

# Colors
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}🔐 Managed Identity Security Audit${NC}"
echo "Date: $(date)"
echo ""

# Check Azure authentication
if ! az account show &>/dev/null; then
  echo -e "${RED}❌ Not authenticated with Azure CLI${NC}"
  echo "Run: az login"
  exit 1
fi

SUBSCRIPTION_NAME=$(az account show --query name -o tsv)
SUBSCRIPTION_ID=$(az account show --query id -o tsv)

echo "Subscription: $SUBSCRIPTION_NAME"
echo "Subscription ID: $SUBSCRIPTION_ID"
echo ""

# Part 1: System-Assigned Managed Identities
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📋 System-Assigned Managed Identities${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

SYSTEM_IDENTITIES=$(az resource list \
  --query "[?identity.type == 'SystemAssigned' || identity.type == 'SystemAssigned, UserAssigned'].{name:name, type:type, resourceGroup:resourceGroup, principalId:identity.principalId}" \
  --output json)

SYSTEM_COUNT=$(echo "$SYSTEM_IDENTITIES" | jq length)

if [ "$SYSTEM_COUNT" -eq 0 ]; then
  echo -e "${YELLOW}⚠️  No system-assigned identities found${NC}"
else
  echo "Found $SYSTEM_COUNT resource(s) with system-assigned identity:"
  echo ""
  echo "$SYSTEM_IDENTITIES" | jq -r '.[] | "  • \(.name) (\(.type)) - \(.resourceGroup)"'
  echo ""
fi

# Part 2: User-Assigned Managed Identities
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📋 User-Assigned Managed Identities${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

USER_IDENTITIES=$(az identity list --output json)
USER_COUNT=$(echo "$USER_IDENTITIES" | jq length)

if [ "$USER_COUNT" -eq 0 ]; then
  echo -e "${YELLOW}⚠️  No user-assigned identities found${NC}"
else
  echo "Found $USER_COUNT user-assigned identity/identities:"
  echo ""
  echo "$USER_IDENTITIES" | jq -r '.[] | "  • \(.name) (\(.resourceGroup))\n    Principal ID: \(.principalId)\n    Client ID: \(.clientId)"'
  echo ""
fi

# Part 3: Permission Audit
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🔍 Permission Audit${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Audit system-assigned identities
if [ "$SYSTEM_COUNT" -gt 0 ]; then
  echo "Auditing system-assigned identity permissions..."
  echo ""
  
  echo "$SYSTEM_IDENTITIES" | jq -r '.[].principalId' | while IFS= read -r PRINCIPAL_ID; do
    if [ -n "$PRINCIPAL_ID" ] && [ "$PRINCIPAL_ID" != "null" ]; then
      RESOURCE_NAME=$(echo "$SYSTEM_IDENTITIES" | jq -r ".[] | select(.principalId == \"$PRINCIPAL_ID\") | .name")
      
      echo -e "${BLUE}Resource: $RESOURCE_NAME${NC}"
      echo "Principal ID: $PRINCIPAL_ID"
      
      # Get role assignments
      ROLES=$(az role assignment list \
        --assignee "$PRINCIPAL_ID" \
        --query "[].{Role:roleDefinitionName, Scope:scope}" \
        --output json 2>/dev/null || echo "[]")
      
      ROLE_COUNT=$(echo "$ROLES" | jq length)
      
      if [ "$ROLE_COUNT" -eq 0 ]; then
        echo -e "${YELLOW}  ⚠️  No role assignments found${NC}"
      else
        echo "  Role assignments: $ROLE_COUNT"
        
        # Check for overprivileged roles
        OWNER_COUNT=$(echo "$ROLES" | jq '[.[] | select(.Role == "Owner")] | length')
        CONTRIBUTOR_COUNT=$(echo "$ROLES" | jq '[.[] | select(.Role == "Contributor")] | length')
        
        if [ "$OWNER_COUNT" -gt 0 ]; then
          echo -e "${RED}  🔴 CRITICAL: Owner role assigned ($OWNER_COUNT)${NC}"
          echo "$ROLES" | jq -r '.[] | select(.Role == "Owner") | "    Scope: \(.Scope)"'
        fi
        
        if [ "$CONTRIBUTOR_COUNT" -gt 0 ]; then
          SCOPE_LEVEL=$(echo "$ROLES" | jq -r '.[] | select(.Role == "Contributor") | .Scope' | head -1)
          if [[ "$SCOPE_LEVEL" == *"/subscriptions/"* ]] && [[ "$SCOPE_LEVEL" != *"/resourceGroups/"* ]]; then
            echo -e "${RED}  🔴 WARNING: Contributor at subscription level${NC}"
          else
            echo -e "${YELLOW}  🟡 Contributor role assigned ($CONTRIBUTOR_COUNT)${NC}"
          fi
          echo "$ROLES" | jq -r '.[] | select(.Role == "Contributor") | "    Scope: \(.Scope)"'
        fi
        
        # Show all roles
        echo "  All roles:"
        echo "$ROLES" | jq -r '.[] | "    • \(.Role) - \(.Scope)"'
      fi
      
      echo ""
    fi
  done
fi

# Audit user-assigned identities
if [ "$USER_COUNT" -gt 0 ]; then
  echo "Auditing user-assigned identity permissions..."
  echo ""
  
  echo "$USER_IDENTITIES" | jq -r '.[].principalId' | while IFS= read -r PRINCIPAL_ID; do
    if [ -n "$PRINCIPAL_ID" ] && [ "$PRINCIPAL_ID" != "null" ]; then
      IDENTITY_NAME=$(echo "$USER_IDENTITIES" | jq -r ".[] | select(.principalId == \"$PRINCIPAL_ID\") | .name")
      
      echo -e "${BLUE}Identity: $IDENTITY_NAME${NC}"
      echo "Principal ID: $PRINCIPAL_ID"
      
      ROLES=$(az role assignment list \
        --assignee "$PRINCIPAL_ID" \
        --query "[].{Role:roleDefinitionName, Scope:scope}" \
        --output json 2>/dev/null || echo "[]")
      
      ROLE_COUNT=$(echo "$ROLES" | jq length)
      
      if [ "$ROLE_COUNT" -eq 0 ]; then
        echo -e "${YELLOW}  ⚠️  No role assignments found${NC}"
      else
        echo "  Role assignments: $ROLE_COUNT"
        
        OWNER_COUNT=$(echo "$ROLES" | jq '[.[] | select(.Role == "Owner")] | length')
        CONTRIBUTOR_COUNT=$(echo "$ROLES" | jq '[.[] | select(.Role == "Contributor")] | length')
        
        if [ "$OWNER_COUNT" -gt 0 ]; then
          echo -e "${RED}  🔴 CRITICAL: Owner role assigned${NC}"
        fi
        
        if [ "$CONTRIBUTOR_COUNT" -gt 0 ]; then
          echo -e "${YELLOW}  🟡 Contributor role assigned${NC}"
        fi
        
        echo "  All roles:"
        echo "$ROLES" | jq -r '.[] | "    • \(.Role) - \(.Scope)"'
      fi
      
      echo ""
    fi
  done
fi

# Part 4: Best Practices Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}✅ Best Practices Checklist${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

echo "1. Least Privilege Principle:"
echo "   • Avoid Owner/Contributor at subscription level"
echo "   • Use specific roles (Key Vault Secrets User, Storage Blob Data Reader)"
echo ""

echo "2. System-Assigned vs User-Assigned:"
echo "   • System-assigned: Use for single-resource scenarios (auto-deleted with resource)"
echo "   • User-assigned: Use for shared identity across multiple resources"
echo ""

echo "3. Monitoring:"
echo "   • Enable Azure AD sign-in logs"
echo "   • Alert on role assignment changes"
echo "   • Regular permission audits (like this script)"
echo ""

echo "4. Remediation Commands:"
echo "   # Remove excessive permissions:"
echo "   az role assignment delete --assignee <principal-id> --role Owner"
echo ""
echo "   # Grant specific Key Vault access:"
echo "   az role assignment create \\"
echo "     --assignee <principal-id> \\"
echo "     --role 'Key Vault Secrets User' \\"
echo "     --scope '/subscriptions/<sub>/resourceGroups/<rg>/providers/Microsoft.KeyVault/vaults/<vault>'"
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📊 Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo "Total system-assigned identities: $SYSTEM_COUNT"
echo "Total user-assigned identities: $USER_COUNT"
echo ""

if [ "$SYSTEM_COUNT" -eq 0 ] && [ "$USER_COUNT" -eq 0 ]; then
  echo -e "${GREEN}✅ No Managed Identities found (nothing to audit)${NC}"
else
  echo -e "${GREEN}✅ Audit complete${NC}"
  echo "Review the output above for any security concerns"
fi

echo ""
