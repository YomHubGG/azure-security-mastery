#!/bin/bash
# 🔍 Comprehensive Secret Scanning Report
# Scans Git repository for leaked secrets using TruffleHog
# Usage: ./secret-scan-report.sh

set -euo pipefail

# Configuration
REPO_PATH="/home/yom/cybersecurity-journey/azure-security-mastery"
OUTPUT_DIR="$REPO_PATH/02-security-services/day53-secret-management-rotation"
REPORT_FILE="$OUTPUT_DIR/scan-report.txt"

# Colors
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}🔍 Secret Scanning Report${NC}" | tee "$REPORT_FILE"
echo "Repository: $REPO_PATH" | tee -a "$REPORT_FILE"
echo "Date: $(date)" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

cd "$REPO_PATH"

# Check if TruffleHog is installed
if ! command -v trufflehog &> /dev/null; then
  echo -e "${YELLOW}⚠️  TruffleHog not found. Installing...${NC}" | tee -a "$REPORT_FILE"
  echo "" | tee -a "$REPORT_FILE"
  echo "To install TruffleHog:" | tee -a "$REPORT_FILE"
  echo "  wget https://github.com/trufflesecurity/trufflehog/releases/download/v3.63.2/trufflehog_3.63.2_linux_amd64.tar.gz" | tee -a "$REPORT_FILE"
  echo "  tar -xzf trufflehog_3.63.2_linux_amd64.tar.gz" | tee -a "$REPORT_FILE"
  echo "  sudo mv trufflehog /usr/local/bin/" | tee -a "$REPORT_FILE"
  echo "" | tee -a "$REPORT_FILE"
  echo -e "${RED}❌ Exiting. Please install TruffleHog and run again.${NC}" | tee -a "$REPORT_FILE"
  exit 1
fi

echo -e "${GREEN}✅ TruffleHog version:${NC}" | tee -a "$REPORT_FILE"
trufflehog --version | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

# Scan 1: Git history (verified secrets only)
echo -e "${BLUE}📜 Scanning Git history for verified secrets...${NC}" | tee -a "$REPORT_FILE"
echo "This scans ALL commits for leaked credentials" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

GIT_SCAN_FILE="$OUTPUT_DIR/trufflehog-git-verified.json"

trufflehog git file://. \
  --only-verified \
  --json > "$GIT_SCAN_FILE" 2>&1 | tee -a "$REPORT_FILE"

if [ -f "$GIT_SCAN_FILE" ]; then
  VERIFIED_COUNT=$(jq length "$GIT_SCAN_FILE" 2>/dev/null || echo 0)
  
  if [ "$VERIFIED_COUNT" -gt 0 ]; then
    echo -e "${RED}🚨 CRITICAL: $VERIFIED_COUNT verified secrets found in Git history${NC}" | tee -a "$REPORT_FILE"
    echo "" | tee -a "$REPORT_FILE"
    echo "Secrets found:" | tee -a "$REPORT_FILE"
    jq -r '.[] | "  - \(.DetectorName): \(.SourceMetadata.Data.Commit.file) (commit: \(.SourceMetadata.Data.Commit.commit[:8]))"' "$GIT_SCAN_FILE" 2>/dev/null | tee -a "$REPORT_FILE"
    echo "" | tee -a "$REPORT_FILE"
    echo "⚠️  ACTION REQUIRED:" | tee -a "$REPORT_FILE"
    echo "  1. Rotate all leaked credentials immediately" | tee -a "$REPORT_FILE"
    echo "  2. Review full report: cat $GIT_SCAN_FILE | jq" | tee -a "$REPORT_FILE"
    echo "  3. Consider using git-filter-repo to remove from history (DESTRUCTIVE)" | tee -a "$REPORT_FILE"
  else
    echo -e "${GREEN}✅ No verified secrets found in Git history${NC}" | tee -a "$REPORT_FILE"
  fi
else
  echo -e "${YELLOW}⚠️  Git scan failed or produced no output${NC}" | tee -a "$REPORT_FILE"
  VERIFIED_COUNT=0
fi

echo "" | tee -a "$REPORT_FILE"

# Scan 2: Current filesystem (all entropy)
echo -e "${BLUE}📁 Scanning current files for high-entropy strings...${NC}" | tee -a "$REPORT_FILE"
echo "This scans working directory for potential secrets" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

FILESYSTEM_SCAN_FILE="$OUTPUT_DIR/trufflehog-filesystem.json"

trufflehog filesystem . \
  --exclude-paths .git \
  --exclude-paths node_modules \
  --exclude-paths .venv \
  --json > "$FILESYSTEM_SCAN_FILE" 2>&1 | tee -a "$REPORT_FILE" || true

if [ -f "$FILESYSTEM_SCAN_FILE" ]; then
  FILESYSTEM_COUNT=$(jq length "$FILESYSTEM_SCAN_FILE" 2>/dev/null || echo 0)
  echo -e "${BLUE}Potential secrets in files: $FILESYSTEM_COUNT${NC}" | tee -a "$REPORT_FILE"
  
  if [ "$FILESYSTEM_COUNT" -gt 0 ]; then
    echo "" | tee -a "$REPORT_FILE"
    echo "Review these findings (may include false positives):" | tee -a "$REPORT_FILE"
    jq -r '.[] | "  - \(.DetectorName): \(.SourceMetadata.Data.Filesystem.file)"' "$FILESYSTEM_SCAN_FILE" 2>/dev/null | head -20 | tee -a "$REPORT_FILE"
    
    if [ "$FILESYSTEM_COUNT" -gt 20 ]; then
      echo "  ... and $((FILESYSTEM_COUNT - 20)) more" | tee -a "$REPORT_FILE"
    fi
  fi
else
  echo -e "${YELLOW}⚠️  Filesystem scan failed or produced no output${NC}" | tee -a "$REPORT_FILE"
  FILESYSTEM_COUNT=0
fi

echo "" | tee -a "$REPORT_FILE"

# Summary
echo -e "${BLUE}========================================${NC}" | tee -a "$REPORT_FILE"
echo -e "${BLUE}📊 Scan Summary${NC}" | tee -a "$REPORT_FILE"
echo -e "${BLUE}========================================${NC}" | tee -a "$REPORT_FILE"
echo "Verified secrets in Git history: $VERIFIED_COUNT" | tee -a "$REPORT_FILE"
echo "Potential secrets in current files: $FILESYSTEM_COUNT" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

if [ "$VERIFIED_COUNT" -gt 0 ]; then
  echo -e "${RED}🔴 CRITICAL: Verified secrets found - immediate action required${NC}" | tee -a "$REPORT_FILE"
  EXIT_CODE=2
elif [ "$FILESYSTEM_COUNT" -gt 0 ]; then
  echo -e "${YELLOW}🟡 WARNING: Potential secrets found - review recommended${NC}" | tee -a "$REPORT_FILE"
  EXIT_CODE=1
else
  echo -e "${GREEN}✅ No verified secrets found - repository appears clean${NC}" | tee -a "$REPORT_FILE"
  EXIT_CODE=0
fi

echo "" | tee -a "$REPORT_FILE"
echo -e "${BLUE}📋 Full results available:${NC}" | tee -a "$REPORT_FILE"
echo "  - Git history: $GIT_SCAN_FILE" | tee -a "$REPORT_FILE"
echo "  - Current files: $FILESYSTEM_SCAN_FILE" | tee -a "$REPORT_FILE"
echo "  - This report: $REPORT_FILE" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

echo -e "${BLUE}💡 Next Steps:${NC}" | tee -a "$REPORT_FILE"
echo "1. Review scan results: cat $GIT_SCAN_FILE | jq" | tee -a "$REPORT_FILE"
echo "2. Rotate any leaked credentials in Azure Key Vault" | tee -a "$REPORT_FILE"
echo "3. Set up pre-commit hooks to prevent future leaks" | tee -a "$REPORT_FILE"
echo "4. Configure GitHub secret scanning (automatic for public repos)" | tee -a "$REPORT_FILE"
echo "" | tee -a "$REPORT_FILE"

exit $EXIT_CODE
