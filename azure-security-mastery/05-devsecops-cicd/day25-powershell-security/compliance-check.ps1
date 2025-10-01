<#
.SYNOPSIS
    Azure Compliance Checker
.DESCRIPTION
    Validates resources against security baseline
.NOTES
    Author: Security Engineering Team
    Date: October 1, 2025
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

function Test-KeyVaultCompliance {
    param($KeyVault)
    
    $compliance = @{
        Resource = $KeyVault.VaultName
        Compliant = $true
        Findings = @()
    }
    
    # Baseline requirements
    if (-not $KeyVault.EnableSoftDelete) {
        $compliance.Compliant = $false
        $compliance.Findings += "Soft delete not enabled"
    }
    
    if (-not $KeyVault.EnablePurgeProtection) {
        $compliance.Compliant = $false
        $compliance.Findings += "Purge protection not enabled"
    }
    
    return $compliance
}

function Test-NSGCompliance {
    param($NSG)
    
    $compliance = @{
        Resource = $NSG.Name
        Compliant = $true
        Findings = @()
    }
    
    # Check for overly permissive rules
    foreach ($rule in $NSG.SecurityRules) {
        if ($rule.SourceAddressPrefix -eq "*" -and $rule.Access -eq "Allow" -and $rule.Direction -eq "Inbound") {
            $compliance.Compliant = $false
            $compliance.Findings += "Inbound rule '$($rule.Name)' allows traffic from any source (*)"
        }
    }
    
    return $compliance
}

Write-Host "🔒 Azure Security Compliance Check" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

$allCompliant = $true
$findings = @()
$totalResources = 0
$compliantResources = 0

# Check storage accounts
Write-Host "📦 Checking Storage Accounts..." -ForegroundColor Green
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName
foreach ($storage in $storageAccounts) {
    $totalResources++
    $result = Test-StorageCompliance -StorageAccount $storage
    
    if (-not $result.Compliant) {
        $allCompliant = $false
        $findings += $result
        Write-Host "  ❌ $($result.Resource): NOT COMPLIANT" -ForegroundColor Red
        foreach ($finding in $result.Findings) {
            Write-Host "     - $finding" -ForegroundColor Yellow
        }
    } else {
        $compliantResources++
        Write-Host "  ✅ $($result.Resource): COMPLIANT" -ForegroundColor Green
    }
}

# Check Key Vaults
Write-Host ""
Write-Host "🔐 Checking Key Vaults..." -ForegroundColor Green
$keyVaults = Get-AzKeyVault -ResourceGroupName $ResourceGroupName
foreach ($vault in $keyVaults) {
    $totalResources++
    $result = Test-KeyVaultCompliance -KeyVault $vault
    
    if (-not $result.Compliant) {
        $allCompliant = $false
        $findings += $result
        Write-Host "  ❌ $($result.Resource): NOT COMPLIANT" -ForegroundColor Red
        foreach ($finding in $result.Findings) {
            Write-Host "     - $finding" -ForegroundColor Yellow
        }
    } else {
        $compliantResources++
        Write-Host "  ✅ $($result.Resource): COMPLIANT" -ForegroundColor Green
    }
}

# Check NSGs
Write-Host ""
Write-Host "🛡️  Checking Network Security Groups..." -ForegroundColor Green
$nsgs = Get-AzNetworkSecurityGroup -ResourceGroupName $ResourceGroupName
foreach ($nsg in $nsgs) {
    $totalResources++
    $result = Test-NSGCompliance -NSG $nsg
    
    if (-not $result.Compliant) {
        $allCompliant = $false
        $findings += $result
        Write-Host "  ❌ $($result.Resource): NOT COMPLIANT" -ForegroundColor Red
        foreach ($finding in $result.Findings) {
            Write-Host "     - $finding" -ForegroundColor Yellow
        }
    } else {
        $compliantResources++
        Write-Host "  ✅ $($result.Resource): COMPLIANT" -ForegroundColor Green
    }
}

# Summary
Write-Host ""
Write-Host "📊 Compliance Summary" -ForegroundColor Cyan
Write-Host "====================" -ForegroundColor Cyan
Write-Host "Total Resources Checked: $totalResources" -ForegroundColor White
Write-Host "Compliant: $compliantResources" -ForegroundColor Green
Write-Host "Non-Compliant: $($totalResources - $compliantResources)" -ForegroundColor Red
Write-Host "Compliance Rate: $([math]::Round(($compliantResources / $totalResources) * 100, 2))%" -ForegroundColor $(if ($compliantResources -eq $totalResources) {"Green"} else {"Yellow"})

Write-Host ""
if ($allCompliant) {
    Write-Host "✅ All resources are compliant with security baseline!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "❌ Compliance issues found. Review required." -ForegroundColor Red
    Write-Host "   Run auto-remediate.ps1 to fix common issues automatically." -ForegroundColor Yellow
    exit 1
}
