#!/bin/bash
# 🔔 Secret Expiration Monitoring Script
# Designed for cron jobs - sends alerts for expiring secrets
# Usage: ./expiration-monitor.sh [vault-name] [alert-days] [critical-days]

set -euo pipefail

# Configuration
VAULT_NAME="${1:-kvlearning4uybw3c2lbkwm}"
ALERT_DAYS="${2:-30}"
CRITICAL_DAYS="${3:-7}"

# Email settings (optional - set to receive email alerts)
EMAIL_RECIPIENT="${EMAIL_RECIPIENT:-}"
EMAIL_SUBJECT="🔔 Key Vault Secret Expiration Alert"

# Log file
LOG_FILE="/tmp/keyvault-expiration-$(date +%Y%m%d).log"

# Function to log messages
log() {
  echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

# Function to send email (requires mailutils)
send_email() {
  if [ -n "$EMAIL_RECIPIENT" ] && command -v mail &> /dev/null; then
    echo "$1" | mail -s "$EMAIL_SUBJECT" "$EMAIL_RECIPIENT"
    log "Email sent to $EMAIL_RECIPIENT"
  fi
}

log "=========================================="
log "Secret Expiration Alert Report"
log "Vault: $VAULT_NAME"
log "Alert threshold: $ALERT_DAYS days"
log "Critical threshold: $CRITICAL_DAYS days"
log "=========================================="

# Check Azure authentication
if ! az account show &>/dev/null 2>&1; then
  log "ERROR: Not authenticated with Azure CLI"
  exit 3
fi

# Check Key Vault existence
if ! az keyvault show --name "$VAULT_NAME" &>/dev/null 2>&1; then
  log "ERROR: Key Vault '$VAULT_NAME' not found"
  exit 3
fi

# Get secrets
SECRETS=$(az keyvault secret list \
  --vault-name "$VAULT_NAME" \
  --query "[?attributes.expires != null && attributes.enabled == \`true\`].{name:name, expires:attributes.expires}" \
  --output json 2>/dev/null)

if [ "$(echo "$SECRETS" | jq length)" -eq 0 ]; then
  log "INFO: No secrets with expiration dates found"
  exit 0
fi

# Calculate timestamps
NOW=$(date +%s)
ALERT_THRESHOLD=$(($NOW + ($ALERT_DAYS * 86400)))
CRITICAL_THRESHOLD=$(($NOW + ($CRITICAL_DAYS * 86400)))

# Counters
WARNING_COUNT=0
CRITICAL_COUNT=0
EXPIRED_COUNT=0

# Alert message
ALERT_MESSAGE=""

# Parse each secret
echo "$SECRETS" | jq -r '.[] | @json' | while IFS= read -r secret; do
  NAME=$(echo "$secret" | jq -r '.name')
  EXPIRES=$(echo "$secret" | jq -r '.expires')
  EXPIRY_TIMESTAMP=$(date -d "$EXPIRES" +%s 2>/dev/null || echo 0)
  
  if [ $EXPIRY_TIMESTAMP -eq 0 ]; then
    continue
  fi
  
  DAYS_REMAINING=$(( ($EXPIRY_TIMESTAMP - $NOW) / 86400 ))
  EXPIRY_DATE=$(date -d "@$EXPIRY_TIMESTAMP" "+%Y-%m-%d")
  
  # Check expiration status
  if [ $EXPIRY_TIMESTAMP -lt $NOW ]; then
    log "🔴 EXPIRED: $NAME (expired on $EXPIRY_DATE)"
    ALERT_MESSAGE="$ALERT_MESSAGE\n🔴 EXPIRED: $NAME (expired on $EXPIRY_DATE)"
    EXPIRED_COUNT=$((EXPIRED_COUNT + 1))
  elif [ $EXPIRY_TIMESTAMP -lt $CRITICAL_THRESHOLD ]; then
    log "🚨 CRITICAL: $NAME expires in $DAYS_REMAINING days ($EXPIRY_DATE)"
    ALERT_MESSAGE="$ALERT_MESSAGE\n🚨 CRITICAL: $NAME expires in $DAYS_REMAINING days ($EXPIRY_DATE)"
    CRITICAL_COUNT=$((CRITICAL_COUNT + 1))
  elif [ $EXPIRY_TIMESTAMP -lt $ALERT_THRESHOLD ]; then
    log "⚠️  WARNING: $NAME expires in $DAYS_REMAINING days ($EXPIRY_DATE)"
    ALERT_MESSAGE="$ALERT_MESSAGE\n⚠️  WARNING: $NAME expires in $DAYS_REMAINING days ($EXPIRY_DATE)"
    WARNING_COUNT=$((WARNING_COUNT + 1))
  fi
done

# Summary
log "=========================================="
log "Summary:"
log "  Expired: $EXPIRED_COUNT"
log "  Critical (< $CRITICAL_DAYS days): $CRITICAL_COUNT"
log "  Warning (< $ALERT_DAYS days): $WARNING_COUNT"
log "=========================================="

# Send email if alerts exist
if [ $EXPIRED_COUNT -gt 0 ] || [ $CRITICAL_COUNT -gt 0 ] || [ $WARNING_COUNT -gt 0 ]; then
  FULL_MESSAGE="Key Vault: $VAULT_NAME\n\n$ALERT_MESSAGE\n\nTotal:\n- Expired: $EXPIRED_COUNT\n- Critical: $CRITICAL_COUNT\n- Warning: $WARNING_COUNT"
  send_email "$FULL_MESSAGE"
fi

# Exit codes for monitoring systems
if [ $EXPIRED_COUNT -gt 0 ] || [ $CRITICAL_COUNT -gt 0 ]; then
  log "EXIT CODE: 2 (CRITICAL)"
  exit 2
elif [ $WARNING_COUNT -gt 0 ]; then
  log "EXIT CODE: 1 (WARNING)"
  exit 1
else
  log "EXIT CODE: 0 (OK)"
  exit 0
fi
