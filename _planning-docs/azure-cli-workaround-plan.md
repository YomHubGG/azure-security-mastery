# Azure Learning - CLI-First Approach 🚀
*When your gaming rig won't load Azure portal but CLI works perfectly*

## The Unexpected Advantage

Your **MSI Cyborg 15** struggling with Azure portal while an old Ubuntu runs it fine? 

This is actually **forcing you into the professional workflow**! Enterprise Azure professionals use CLI/PowerShell **way more** than the portal anyway. You're getting the better training! 💪

## Why CLI-First is Superior

### 1. Enterprise Reality ✅
- **95% of Azure security work** is CLI/PowerShell/Bicep
- Portal is mostly for monitoring and troubleshooting
- Infrastructure as Code requires CLI
- Automation requires scripting

### 2. Career Benefits ✅  
- **DevOps-ready skills**: Companies love CLI proficiency
- **Speed**: 10x faster than clicking through portal
- **Repeatability**: Everything scriptable
- **Portfolio value**: Scripts = GitHub commits = green squares

### 3. Learning Benefits ✅
- No browser loading delays
- No portal crashes or freezes  
- Perfect for your 1-2 hour sessions
- Forces deeper technical understanding

## Revised Strategy

### Phase 1: Quick Account Setup (Old Ubuntu - 15 min)
```bash
# On your working Ubuntu machine:
1. portal.azure.com → Create free account
2. Set up MFA  
3. Note Subscription ID
4. Done! Back to MSI for everything else
```

### Phase 2: CLI Mastery (MSI Cyborg - All learning)
Your Parrot setup is already perfect:
- ✅ Azure CLI 2.45.0
- ✅ PowerShell 7.5.2
- ✅ Bicep 0.37.4

## Action Items Today

### Step 1: Test Your CLI Setup (MSI - 5 min)
```bash
# Verify everything works
az --version
pwsh -c "Get-Module -ListAvailable Az*"
bicep --version
```

### Step 2: Account Creation (Old Ubuntu - 10 min)
- Quick portal signup
- Get subscription ID
- Back to MSI immediately  

### Step 3: CLI Connection (MSI - 5 min)
```bash
# Device code login (browser-independent)
az login --use-device-code
az account show --output table
```

### Step 4: First Deployment (MSI - 20 min)
```bash  
# Create resource group
az group create \
  --name "rg-security-lab-001" \
  --location "West Europe"

# Deploy secure storage
az storage account create \
  --name "stseclab$(date +%s)" \
  --resource-group "rg-security-lab-001" \
  --location "West Europe" \
  --sku Standard_LRS \
  --https-only true \
  --min-tls-version TLS1_2 \
  --allow-blob-public-access false
```

## The Reality Check

**Portal users**: Click, wait, load, click, wait, load...
**CLI professionals**: Type, enter, done. Next task.

**You're getting the professional training by accident! 🚀**

---
Ready to turn this "limitation" into your competitive advantage?
