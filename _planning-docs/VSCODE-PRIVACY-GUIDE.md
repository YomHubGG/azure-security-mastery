# 🔒 VS Code Privacy & Telemetry Guide

**Date**: October 14, 2025  
**Purpose**: Complete guide to disabling Microsoft telemetry in VS Code  
**Status**: Production-ready configuration

---

## 🎯 Quick Start

### **Option 1: Automated Script** (Recommended)
```bash
# Run the automated configuration script
chmod +x disable-vscode-telemetry.sh
./disable-vscode-telemetry.sh
```

### **Option 2: Manual Configuration**
1. Open VS Code
2. Press `Ctrl+Shift+P` (Command Palette)
3. Type: `Preferences: Open Settings (JSON)`
4. Copy-paste the settings below

---

## 📋 Complete Privacy Settings

```json
{
    // === TELEMETRY (PRIMARY) ===
    "telemetry.telemetryLevel": "off",
    
    // === CRASH REPORTING ===
    "telemetry.enableCrashReporter": false,
    
    // === EXPERIMENTS ===
    "workbench.enableExperiments": false,
    
    // === NATURAL LANGUAGE SEARCH ===
    // Sends your search queries to Microsoft servers
    "workbench.settings.enableNaturalLanguageSearch": false,
    
    // === EXTENSION MANAGEMENT ===
    "extensions.autoCheckUpdates": false,
    "extensions.autoUpdate": false,
    "extensions.ignoreRecommendations": true,
    
    // === UPDATE SETTINGS ===
    "update.mode": "manual",
    "update.showReleaseNotes": false,
    
    // === NPM TELEMETRY ===
    "npm.fetchOnlinePackageInfo": false,
    
    // === EDITOR SETTINGS ===
    "editor.accessibilitySupport": "off",
    
    // === TERMINAL ===
    "terminal.integrated.enablePersistentSessions": false,
    "terminal.integrated.experimentalUseTitleEvent": false,
    
    // === SECURITY (BONUS) ===
    "security.workspace.trust.enabled": true,
    "security.workspace.trust.startupPrompt": "always",
    "security.workspace.trust.banner": "always"
}
```

---

## 🔍 What Each Setting Does

### **Core Telemetry**

#### `"telemetry.telemetryLevel": "off"`
- **Purpose**: Master switch for all telemetry
- **What it blocks**: Usage data, feature analytics, error reports
- **Impact**: None - VS Code works perfectly without it

#### `"telemetry.enableCrashReporter": false`
- **Purpose**: Disables crash report uploads to Microsoft
- **What it blocks**: Stack traces, crash dumps sent to MS servers
- **Impact**: You won't help Microsoft debug crashes (that's fine)

### **Experiments**

#### `"workbench.enableExperiments": false`
- **Purpose**: Stops Microsoft from A/B testing features on you
- **What it blocks**: Randomly enabled experimental features
- **Impact**: More predictable VS Code experience

### **Search Privacy**

#### `"workbench.settings.enableNaturalLanguageSearch": false`
- **Purpose**: Prevents sending search queries to Microsoft
- **What it blocks**: "Search settings using natural language" feature
- **Impact**: Settings search becomes keyword-only (faster anyway)

### **Extension Privacy**

#### `"extensions.autoCheckUpdates": false`
#### `"extensions.autoUpdate": false`
#### `"extensions.ignoreRecommendations": true`
- **Purpose**: Stops constant "phone home" for extension updates
- **What it blocks**: Automatic extension marketplace queries
- **Impact**: You manually update extensions (more control)

### **Update Control**

#### `"update.mode": "manual"`
- **Purpose**: No automatic VS Code updates
- **What it blocks**: Background update checks and downloads
- **Impact**: You choose when to update (stability)

### **NPM Privacy**

#### `"npm.fetchOnlinePackageInfo": false`
- **Purpose**: Stops querying npm registry for package info
- **What it blocks**: Online lookups when hovering over packages
- **Impact**: Slightly less convenient but more private

---

## ⚠️ Important Considerations

### **GitHub Copilot**

GitHub Copilot **requires** sending your code to OpenAI servers. That's how it works.

**To keep Copilot enabled** (recommended for your learning):
```json
{
    "github.copilot.enable": {
        "*": true
    }
}
```

**To disable Copilot entirely** (maximum privacy):
```json
{
    "github.copilot.enable": {
        "*": false
    }
}
```

**Our Recommendation**: Keep Copilot ON
- You're already using Azure (Microsoft cloud)
- The productivity boost is worth it for your learning pace
- Your code isn't sensitive (learning exercises)
- Professional developers use Copilot widely

---

## 🛡️ Network-Level Blocking (Nuclear Option)

If you want **zero network communication** with Microsoft telemetry servers:

### **Method 1: /etc/hosts (Simple)**
```bash
# Edit hosts file
sudo nano /etc/hosts

# Add these lines:
127.0.0.1 vortex.data.microsoft.com
127.0.0.1 dc.services.visualstudio.com
127.0.0.1 mobile.events.data.microsoft.com
127.0.0.1 pipe.aria.microsoft.com
127.0.0.1 dc.trafficmanager.net
```

### **Method 2: Firewall (Advanced)**
```bash
# Using ufw (Uncomplicated Firewall)
sudo ufw deny out to vortex.data.microsoft.com
sudo ufw deny out to dc.services.visualstudio.com
```

### **⚠️ Warning**: May Break Features
- Extension marketplace might be affected
- Update checks will fail (expected)
- Some legitimate VS Code features may error

**Recommendation**: Only use if you're paranoid about telemetry. The JSON settings above are sufficient for 99% of users.

---

## ✅ Verification Steps

### **1. Check Settings File**
```bash
# View your settings
cat ~/.config/Code/User/settings.json | grep telemetry
```

Expected output:
```
    "telemetry.enableCrashReporter": false,
    "telemetry.telemetryLevel": "off",
```

### **2. Verify in VS Code UI**
1. Open VS Code
2. Go to: `File` → `Preferences` → `Settings`
3. Search for: `telemetry`
4. Verify "Telemetry Level" is set to **off**

### **3. Check Network Traffic (Paranoid Mode)**
```bash
# Monitor VS Code network connections
sudo netstat -tunap | grep code

# Or use tcpdump to see what's being sent
sudo tcpdump -i any host vortex.data.microsoft.com
```

If configured correctly, you should see **zero connections** to Microsoft telemetry servers.

---

## 📊 Privacy Levels Explained

### **Level 1: Default (Microsoft Telemetry ON)**
```
Telemetry: ✅ Enabled
Crash Reports: ✅ Enabled
Experiments: ✅ Enabled
Extensions: ✅ Auto-update
Privacy: ❌ Low
```

### **Level 2: JSON Settings (Recommended)** ← **You are here**
```
Telemetry: ❌ Disabled
Crash Reports: ❌ Disabled
Experiments: ❌ Disabled
Extensions: 🔧 Manual update
Privacy: ✅ High
```

### **Level 3: Network Blocking (Paranoid)**
```
Telemetry: 🚫 Blocked at network
Crash Reports: 🚫 Blocked
Experiments: 🚫 Blocked
Extensions: ⚠️ May break
Privacy: ✅ Maximum
```

---

## 🎓 What Microsoft Collects (When Enabled)

For transparency, here's what telemetry sends when **enabled**:

### **Usage Data**
- Which features you use
- How often you use them
- Performance metrics
- Extension usage patterns

### **Error Reports**
- Stack traces when VS Code crashes
- Extension errors
- Configuration issues

### **System Info**
- OS version
- VS Code version
- Hardware specs (CPU, RAM)
- Screen resolution

### **What's NOT Collected**
- ✅ Your actual code
- ✅ File contents
- ✅ Usernames/passwords
- ✅ Personal information

**Microsoft's Privacy Statement**: https://privacy.microsoft.com/en-us/privacystatement

---

## 💡 Best Practices

### **For Your Cybersecurity Journey**

1. ✅ **Disable telemetry** (what we're doing now)
2. ✅ **Keep Copilot enabled** (massive productivity boost)
3. ✅ **Manual extension updates** (know what you're installing)
4. ✅ **Workspace trust enabled** (security against malicious projects)
5. ⚠️ **Don't use network blocking** (breaks legitimate features)

### **For Professional Environments**

When you land a job:
- Enterprise deployments often **require** telemetry for compliance
- IT departments may mandate specific VS Code configurations
- Be prepared to adapt settings to company policies

---

## 🔄 Restoration

If you need to **revert** to default settings:

```bash
# Restore from backup
cd ~/.config/Code/User
ls -la settings.json.backup-*

# Copy backup back
cp settings.json.backup-YYYYMMDD-HHMMSS settings.json

# Or remove privacy settings and let VS Code reset
rm settings.json
# VS Code will create new default settings on next launch
```

---

## 📚 Additional Resources

### **Microsoft's Official Documentation**
- Telemetry: https://code.visualstudio.com/docs/getstarted/telemetry
- Privacy Statement: https://privacy.microsoft.com/en-us/privacystatement

### **Community Resources**
- VSCodium Project: https://vscodium.com/
- Privacy Settings Guide: https://github.com/microsoft/vscode/wiki/Privacy

### **Your Cybersecurity Context**
You're learning **Azure Security** using Microsoft tools. Some telemetry exposure is inherent to the ecosystem:
- Azure CLI sends usage data
- Azure Portal tracks actions
- GitHub (Microsoft-owned) tracks repos

**Pragmatic approach**: Disable what you can, accept what's necessary for functionality.

---

## 🎯 Summary

### **What We Disabled**
✅ Telemetry data collection  
✅ Crash report uploads  
✅ Experimental feature rollouts  
✅ Natural language search queries  
✅ Automatic extension updates  
✅ Automatic VS Code updates  

### **What We Kept**
✅ GitHub Copilot (productivity essential)  
✅ Extension marketplace access  
✅ All core VS Code functionality  
✅ Azure development features  

### **Result**
🔒 **High privacy** with **zero functionality loss** for your Azure Security learning journey.

---

## ⚡ Quick Commands

```bash
# Run telemetry disabling script
chmod +x disable-vscode-telemetry.sh
./disable-vscode-telemetry.sh

# Check current settings
cat ~/.config/Code/User/settings.json | grep -i telemetry

# Verify VS Code isn't phoning home
sudo netstat -tunap | grep code | grep microsoft

# Open VS Code settings
code ~/.config/Code/User/settings.json
```

---

## 🚀 Next Steps

After running the script:

1. ✅ **Restart VS Code** (close all windows)
2. ✅ **Verify settings** (Ctrl+Shift+P → "Open Settings JSON")
3. ✅ **Continue Day 37** - Container Registry Security
4. ✅ **Enjoy privacy-enhanced coding!** 🎉

---

*Privacy configured. Telemetry disabled. Ready to continue your Azure Security journey! 🔒*
