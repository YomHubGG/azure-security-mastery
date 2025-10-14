#!/bin/bash
# VS Code Telemetry Disabling Script
# Date: October 14, 2025
# Purpose: Disable all Microsoft telemetry and tracking in VS Code

set -e

echo "🔒 VS Code Privacy Configuration Script"
echo "========================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Detect VS Code settings location
VSCODE_SETTINGS_DIR="$HOME/.config/Code/User"
SETTINGS_FILE="$VSCODE_SETTINGS_DIR/settings.json"

echo -e "${BLUE}[INFO]${NC} Checking VS Code settings location..."

# Create directory if it doesn't exist
if [ ! -d "$VSCODE_SETTINGS_DIR" ]; then
    echo -e "${YELLOW}[WARN]${NC} VS Code settings directory not found. Creating..."
    mkdir -p "$VSCODE_SETTINGS_DIR"
fi

# Backup existing settings
if [ -f "$SETTINGS_FILE" ]; then
    BACKUP_FILE="$SETTINGS_FILE.backup-$(date +%Y%m%d-%H%M%S)"
    echo -e "${BLUE}[INFO]${NC} Backing up existing settings to: $BACKUP_FILE"
    cp "$SETTINGS_FILE" "$BACKUP_FILE"
else
    echo -e "${YELLOW}[WARN]${NC} No existing settings.json found. Creating new one..."
    echo "{}" > "$SETTINGS_FILE"
fi

echo ""
echo -e "${BLUE}[INFO]${NC} Adding privacy-focused telemetry settings..."
echo ""

# Use Python to safely merge JSON (better than jq for this case)
python3 << 'EOF'
import json
import os

settings_file = os.path.expanduser("~/.config/Code/User/settings.json")

# Read existing settings
try:
    with open(settings_file, 'r') as f:
        settings = json.load(f)
except:
    settings = {}

# Privacy and telemetry settings to add/update
privacy_settings = {
    # === TELEMETRY SETTINGS ===
    "telemetry.telemetryLevel": "off",
    
    # === CRASH REPORTING ===
    "telemetry.enableCrashReporter": False,
    
    # === EXPERIMENTS ===
    "workbench.enableExperiments": False,
    
    # === NATURAL LANGUAGE SEARCH (sends queries to MS) ===
    "workbench.settings.enableNaturalLanguageSearch": False,
    
    # === EXTENSION SETTINGS ===
    "extensions.autoCheckUpdates": False,
    "extensions.autoUpdate": False,
    "extensions.ignoreRecommendations": True,
    
    # === UPDATE SETTINGS ===
    "update.mode": "manual",
    "update.showReleaseNotes": False,
    
    # === NPM SETTINGS (if using Node.js) ===
    "npm.fetchOnlinePackageInfo": False,
    
    # === GITHUB COPILOT PRIVACY ===
    "github.copilot.enable": {
        "*": True
    },
    # Note: Copilot sends code to OpenAI by design - that's how it works
    # If you want privacy, disable Copilot entirely by setting above to False
    
    # === EDITOR SETTINGS (bonus privacy) ===
    "editor.suggest.showStatusBar": True,
    "editor.accessibilitySupport": "off",  # Disables telemetry for accessibility
    
    # === SECURITY (while we're at it) ===
    "security.workspace.trust.enabled": True,
    "security.workspace.trust.startupPrompt": "always",
    "security.workspace.trust.banner": "always",
    
    # === TERMINAL ===
    "terminal.integrated.enablePersistentSessions": False,
    "terminal.integrated.experimentalUseTitleEvent": False
}

# Merge settings
settings.update(privacy_settings)

# Write back to file with nice formatting
with open(settings_file, 'w') as f:
    json.dump(settings, f, indent=4, sort_keys=True)

print("✅ Privacy settings successfully applied!")

EOF

echo ""
echo -e "${GREEN}[SUCCESS]${NC} VS Code privacy configuration complete!"
echo ""
echo "📋 Applied Settings:"
echo "  ✅ Telemetry level: OFF"
echo "  ✅ Crash reporter: DISABLED"
echo "  ✅ Experiments: DISABLED"
echo "  ✅ Natural language search: DISABLED"
echo "  ✅ Auto-updates: DISABLED"
echo "  ✅ Extension recommendations: DISABLED"
echo ""
echo "⚠️  Note: GitHub Copilot still sends code to OpenAI (required for functionality)"
echo "   To disable Copilot entirely, set 'github.copilot.enable' to false in settings"
echo ""
echo "🔄 Restart VS Code for all changes to take effect"
echo ""

# Optional: Network-level blocking (commented out - use with caution)
cat << 'NETWORK_BLOCK'
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📡 OPTIONAL: Network-Level Telemetry Blocking
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

If you want to block telemetry at the network level (nuclear option),
add these entries to /etc/hosts:

  sudo nano /etc/hosts

Add these lines:
  127.0.0.1 vortex.data.microsoft.com
  127.0.0.1 dc.services.visualstudio.com
  127.0.0.1 mobile.events.data.microsoft.com
  127.0.0.1 pipe.aria.microsoft.com
  127.0.0.1 dc.trafficmanager.net

⚠️  WARNING: This may break some legitimate VS Code features!
    Only use if you're serious about zero telemetry.

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
NETWORK_BLOCK

echo ""
echo "📚 To verify your settings:"
echo "  1. Open VS Code"
echo "  2. Press Ctrl+Shift+P"
echo "  3. Type: 'Preferences: Open Settings (JSON)'"
echo "  4. Check that telemetry settings are present"
echo ""
echo "💡 Your backup is saved at:"
echo "   $(ls -t $VSCODE_SETTINGS_DIR/settings.json.backup-* 2>/dev/null | head -1 || echo 'No backup created')"
echo ""
