# 🔧 Day 25: PowerShell Security Scripts

**Date**: October 1, 2025  
**Focus**: Automating Azure security audits and compliance checks  
**Duration**: 1-2 hours  
**Session**: #13

---

## 🎯 **Today's Learning Objectives** (From 1-Year Plan)

- ✅ Write Azure security audit scripts
- ✅ Automate resource compliance checking  
- ✅ Create security report generation
- ✅ Implement automated remediation
- ✅ Commit: "PowerShell security automation"

---

## 📚 **Why PowerShell for Azure Security?**

**PowerShell is the automation language of Azure**:
- **Native Azure Integration**: Az PowerShell module
- **Security Automation**: Compliance scanning, auditing, remediation
- **Enterprise Standard**: Used in production environments globally
- **Career Essential**: Required skill for Azure security roles

### **What You'll Build Today**
1. **Security Audit Script**: Scan all resources for compliance
2. **Compliance Checker**: Validate security configurations
3. **Report Generator**: Professional security reports
4. **Auto-Remediation**: Fix common security issues

---

## 🛠️ **Phase 1: PowerShell Environment Setup** (15 minutes)

### **Install Az PowerShell Module**

```bash
# Check if PowerShell is installed
pwsh --version

# Install Az module (if not already installed)
pwsh -Command "Install-Module -Name Az -Repository PSGallery -Force -AllowClobber"

# Verify installation
pwsh -Command "Get-Module -ListAvailable Az.*"
```

### **Azure Authentication**

```powershell
# Connect to Azure
Connect-AzAccount

# Verify connection
Get-AzContext

# Set subscription
Set-AzContext -SubscriptionId "a174b81f-93c2-4089-afa0-6f82a5165c86"
```

---

## 🔍 **Phase 2: Security Audit Script** (30 minutes)

### **Script 1: Comprehensive Security Audit**

Create `security-audit.ps1`:

```powershell
<#
.SYNOPSIS
    Azure Security Audit Script
.DESCRIPTION
    Audits Azure resources for security compliance
.NOTES
    Author: Security Engineering Team
    Date: October 1, 2025
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$ResourceGroupName = "rg-learning-day1"
)

# Initialize results
$results = @()

Write-Host "🔍 Starting Azure Security Audit..." -ForegroundColor Cyan
Write-Host "Target: $ResourceGroupName" -ForegroundColor Yellow
Write-Host ""

# 1. Storage Account Security Check
Write-Host "📦 Auditing Storage Accounts..." -ForegroundColor Green
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName

foreach ($storage in $storageAccounts) {
    $audit = @{
        ResourceName = $storage.StorageAccountName
        ResourceType = "Storage Account"
        HTTPSOnly = $storage.EnableHttpsTrafficOnly
        TLSVersion = $storage.MinimumTlsVersion
        PublicAccess = $storage.AllowBlobPublicAccess
        Location = $storage.Location
        Status = "PASS"
        Issues = @()
    }
    
    # Check HTTPS enforcement
    if (-not $storage.EnableHttpsTrafficOnly) {
        $audit.Status = "FAIL"
        $audit.Issues += "HTTPS not enforced"
    }
    
    # Check TLS version
    if ($storage.MinimumTlsVersion -ne "TLS1_2") {
        $audit.Status = "WARNING"
        $audit.Issues += "TLS version not 1.2"
    }
    
    # Check public access
    if ($storage.AllowBlobPublicAccess) {
        $audit.Status = "FAIL"
        $audit.Issues += "Public blob access enabled"
    }
    
    $results += New-Object PSObject -Property $audit
}

# 2. Network Security Group Audit
Write-Host "🛡️ Auditing Network Security Groups..." -ForegroundColor Green
$nsgs = Get-AzNetworkSecurityGroup -ResourceGroupName $ResourceGroupName

foreach ($nsg in $nsgs) {
    $audit = @{
        ResourceName = $nsg.Name
        ResourceType = "Network Security Group"
        RuleCount = $nsg.SecurityRules.Count
        Location = $nsg.Location
        Status = "PASS"
        Issues = @()
    }
    
    # Check for permissive rules
    foreach ($rule in $nsg.SecurityRules) {
        if ($rule.SourceAddressPrefix -eq "*" -and $rule.Access -eq "Allow") {
            $audit.Status = "WARNING"
            $audit.Issues += "Rule '$($rule.Name)' allows traffic from any source"
        }
    }
    
    $results += New-Object PSObject -Property $audit
}

# 3. Key Vault Security Check
Write-Host "🔐 Auditing Key Vaults..." -ForegroundColor Green
$keyVaults = Get-AzKeyVault -ResourceGroupName $ResourceGroupName

foreach ($vault in $keyVaults) {
    $audit = @{
        ResourceName = $vault.VaultName
        ResourceType = "Key Vault"
        SoftDelete = $vault.EnableSoftDelete
        PurgeProtection = $vault.EnablePurgeProtection
        Location = $vault.Location
        Status = "PASS"
        Issues = @()
    }
    
    if (-not $vault.EnableSoftDelete) {
        $audit.Status = "FAIL"
        $audit.Issues += "Soft delete not enabled"
    }
    
    if (-not $vault.EnablePurgeProtection) {
        $audit.Status = "WARNING"
        $audit.Issues += "Purge protection not enabled"
    }
    
    $results += New-Object PSObject -Property $audit
}

# Generate Report
Write-Host ""
Write-Host "📊 Security Audit Summary" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
Write-Host ""

$passCount = ($results | Where-Object {$_.Status -eq "PASS"}).Count
$warnCount = ($results | Where-Object {$_.Status -eq "WARNING"}).Count
$failCount = ($results | Where-Object {$_.Status -eq "FAIL"}).Count

Write-Host "✅ PASS: $passCount" -ForegroundColor Green
Write-Host "⚠️ WARNING: $warnCount" -ForegroundColor Yellow
Write-Host "❌ FAIL: $failCount" -ForegroundColor Red
Write-Host ""

# Display detailed results
$results | Format-Table ResourceName, ResourceType, Status, @{Name="Issues";Expression={$_.Issues -join ", "}} -AutoSize

# Export to JSON
$reportPath = "./security-audit-report-$(Get-Date -Format 'yyyyMMdd-HHmmss').json"
$results | ConvertTo-Json -Depth 10 | Out-File $reportPath
Write-Host "📄 Full report saved to: $reportPath" -ForegroundColor Green

# Return results
return $results
```

### **Run the Security Audit**

```bash
# Execute the security audit
pwsh ./security-audit.ps1 -ResourceGroupName "rg-learning-day1"
```

---

## ⚙️ **Phase 3: Compliance Automation Script** (30 minutes)

### **Script 2: Automated Compliance Checker**

Create `compliance-check.ps1`:

```powershell
<#
.SYNOPSIS
    Azure Compliance Checker
.DESCRIPTION
    Validates resources against security baseline
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$ResourceGroupName = "rg-learning-day1"
)

function Test-StorageCompliance {
    param($StorageAccount)
    
    $compliance = @{
        Resource = $StorageAccount.StorageAccountName
        Compliant = $true
        Findings = @()
    }
    
    # Baseline requirements
    if (-not $StorageAccount.EnableHttpsTrafficOnly) {
        $compliance.Compliant = $false
        $compliance.Findings += "HTTPS enforcement missing"
    }
    
    if ($StorageAccount.MinimumTlsVersion -ne "TLS1_2") {
        $compliance.Compliant = $false
        $compliance.Findings += "TLS 1.2 not enforced"
    }
    
    if ($StorageAccount.AllowBlobPublicAccess) {
        $compliance.Compliant = $false
        $compliance.Findings += "Public access enabled"
    }
    
    return $compliance
}

Write-Host "🔒 Azure Security Compliance Check" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

$allCompliant = $true
$findings = @()

# Check storage accounts
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName
foreach ($storage in $storageAccounts) {
    $result = Test-StorageCompliance -StorageAccount $storage
    
    if (-not $result.Compliant) {
        $allCompliant = $false
        $findings += $result
        Write-Host "❌ $($result.Resource): NOT COMPLIANT" -ForegroundColor Red
        foreach ($finding in $result.Findings) {
            Write-Host "   - $finding" -ForegroundColor Yellow
        }
    } else {
        Write-Host "✅ $($result.Resource): COMPLIANT" -ForegroundColor Green
    }
}

Write-Host ""
if ($allCompliant) {
    Write-Host "✅ All resources are compliant!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "❌ Compliance issues found. Review required." -ForegroundColor Red
    exit 1
}
```

---

## 🔧 **Phase 4: Auto-Remediation Script** (30 minutes)

### **Script 3: Security Auto-Remediation**

Create `auto-remediate.ps1`:

```powershell
<#
.SYNOPSIS
    Azure Security Auto-Remediation
.DESCRIPTION
    Automatically fixes common security issues
.PARAMETER WhatIf
    Simulates changes without applying them
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$ResourceGroupName = "rg-learning-day1",
    
    [Parameter(Mandatory=$false)]
    [switch]$WhatIf
)

Write-Host "🔧 Azure Security Auto-Remediation" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

if ($WhatIf) {
    Write-Host "⚠️ SIMULATION MODE - No changes will be applied" -ForegroundColor Yellow
    Write-Host ""
}

$remediationCount = 0

# Fix storage account security
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName

foreach ($storage in $storageAccounts) {
    Write-Host "Checking: $($storage.StorageAccountName)..." -ForegroundColor Cyan
    
    $needsUpdate = $false
    $updates = @{}
    
    # Check HTTPS
    if (-not $storage.EnableHttpsTrafficOnly) {
        Write-Host "  🔧 Enabling HTTPS-only traffic..." -ForegroundColor Yellow
        $updates['EnableHttpsTrafficOnly'] = $true
        $needsUpdate = $true
        $remediationCount++
    }
    
    # Check TLS version
    if ($storage.MinimumTlsVersion -ne "TLS1_2") {
        Write-Host "  🔧 Setting minimum TLS version to 1.2..." -ForegroundColor Yellow
        $updates['MinimumTlsVersion'] = "TLS1_2"
        $needsUpdate = $true
        $remediationCount++
    }
    
    # Check public access
    if ($storage.AllowBlobPublicAccess) {
        Write-Host "  🔧 Disabling public blob access..." -ForegroundColor Yellow
        $updates['AllowBlobPublicAccess'] = $false
        $needsUpdate = $true
        $remediationCount++
    }
    
    # Apply updates
    if ($needsUpdate -and -not $WhatIf) {
        try {
            Set-AzStorageAccount -ResourceGroupName $ResourceGroupName `
                                 -Name $storage.StorageAccountName `
                                 @updates
            Write-Host "  ✅ Remediation applied successfully" -ForegroundColor Green
        } catch {
            Write-Host "  ❌ Remediation failed: $_" -ForegroundColor Red
        }
    } elseif ($needsUpdate) {
        Write-Host "  ℹ️ Would apply remediation (simulation mode)" -ForegroundColor Blue
    } else {
        Write-Host "  ✅ No remediation needed" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "📊 Remediation Summary" -ForegroundColor Cyan
Write-Host "Total issues fixed: $remediationCount" -ForegroundColor $(if ($remediationCount -gt 0) {"Yellow"} else {"Green"})
```

---

## 📊 **Phase 5: Security Report Generator** (15 minutes)

### **Script 4: Professional Security Report**

Create `generate-report.ps1`:

```powershell
<#
.SYNOPSIS
    Generate Professional Security Report
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$ResourceGroupName = "rg-learning-day1"
)

# Collect data
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$report = @{
    GeneratedAt = $timestamp
    ResourceGroup = $ResourceGroupName
    Summary = @{}
    Resources = @()
}

# Get all resources
$resources = Get-AzResource -ResourceGroupName $ResourceGroupName

$report.Summary.TotalResources = $resources.Count
$report.Summary.StorageAccounts = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.Storage/storageAccounts"}).Count
$report.Summary.KeyVaults = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.KeyVault/vaults"}).Count
$report.Summary.NetworkSecurityGroups = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.Network/networkSecurityGroups"}).Count

# Generate HTML report
$html = @"
<!DOCTYPE html>
<html>
<head>
    <title>Azure Security Report</title>
    <style>
        body { font-family: Arial; margin: 40px; background: #f5f5f5; }
        h1 { color: #0078d4; }
        .summary { background: white; padding: 20px; border-radius: 5px; margin: 20px 0; }
        .metric { display: inline-block; margin: 10px 20px; }
        .metric-value { font-size: 32px; font-weight: bold; color: #0078d4; }
        .metric-label { color: #666; }
        table { width: 100%; border-collapse: collapse; background: white; }
        th { background: #0078d4; color: white; padding: 10px; text-align: left; }
        td { padding: 10px; border-bottom: 1px solid #ddd; }
    </style>
</head>
<body>
    <h1>🔒 Azure Security Report</h1>
    <p><strong>Generated:</strong> $timestamp</p>
    <p><strong>Resource Group:</strong> $ResourceGroupName</p>
    
    <div class="summary">
        <h2>Resource Summary</h2>
        <div class="metric">
            <div class="metric-value">$($report.Summary.TotalResources)</div>
            <div class="metric-label">Total Resources</div>
        </div>
        <div class="metric">
            <div class="metric-value">$($report.Summary.StorageAccounts)</div>
            <div class="metric-label">Storage Accounts</div>
        </div>
        <div class="metric">
            <div class="metric-value">$($report.Summary.KeyVaults)</div>
            <div class="metric-label">Key Vaults</div>
        </div>
        <div class="metric">
            <div class="metric-value">$($report.Summary.NetworkSecurityGroups)</div>
            <div class="metric-label">NSGs</div>
        </div>
    </div>
    
    <h2>Resource Inventory</h2>
    <table>
        <tr>
            <th>Resource Name</th>
            <th>Type</th>
            <th>Location</th>
        </tr>
"@

foreach ($resource in $resources) {
    $html += "<tr><td>$($resource.Name)</td><td>$($resource.ResourceType)</td><td>$($resource.Location)</td></tr>`n"
}

$html += @"
    </table>
</body>
</html>
"@

# Save report
$reportPath = "./security-report-$(Get-Date -Format 'yyyyMMdd').html"
$html | Out-File $reportPath -Encoding UTF8

Write-Host "📄 Security report generated: $reportPath" -ForegroundColor Green
Write-Host "Open in browser to view" -ForegroundColor Cyan

# Also export JSON
$reportPath = "./security-report-$(Get-Date -Format 'yyyyMMdd').json"
$report | ConvertTo-Json -Depth 10 | Out-File $reportPath
Write-Host "📄 JSON report saved: $reportPath" -ForegroundColor Green
```

---

## 🎯 **Success Criteria**

By end of session, you'll have:
- ✅ **4 PowerShell security scripts** for Azure automation
- ✅ **Security audit capabilities** to scan infrastructure
- ✅ **Compliance checking** automated
- ✅ **Auto-remediation** for common security issues
- ✅ **Professional reporting** in HTML and JSON

---

## 💼 **Career Skills Demonstrated**

**PowerShell security automation skills show**:
- **Security Operations** - Automated compliance and auditing
- **DevSecOps** - Infrastructure security validation  
- **Enterprise Automation** - Production-ready scripts
- **Incident Response** - Automated remediation capabilities

---

## 🚀 **Ready to Build Security Automation Scripts?**

Let's start by setting up PowerShell and running your first security audit!