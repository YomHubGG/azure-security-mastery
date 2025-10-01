<#
.SYNOPSIS
    Azure Security Auto-Remediation
.DESCRIPTION
    Automatically fixes common security issues
.PARAMETER WhatIf
    Simulates changes without applying them
.NOTES
    Author: Security Engineering Team
    Date: October 1, 2025
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
    Write-Host "⚠️  SIMULATION MODE - No changes will be applied" -ForegroundColor Yellow
    Write-Host ""
}

$remediationCount = 0
$successCount = 0
$failCount = 0

# ============================================
# 1. Fix Storage Account Security
# ============================================
Write-Host "📦 Remediating Storage Accounts..." -ForegroundColor Green
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName

foreach ($storage in $storageAccounts) {
    Write-Host "  Checking: $($storage.StorageAccountName)..." -ForegroundColor Cyan
    
    $needsUpdate = $false
    $updates = @{}
    
    # Check HTTPS
    if (-not $storage.EnableHttpsTrafficOnly) {
        Write-Host "    🔧 Enabling HTTPS-only traffic..." -ForegroundColor Yellow
        $updates['EnableHttpsTrafficOnly'] = $true
        $needsUpdate = $true
        $remediationCount++
    }
    
    # Check TLS version
    if ($storage.MinimumTlsVersion -ne "TLS1_2") {
        Write-Host "    🔧 Setting minimum TLS version to 1.2..." -ForegroundColor Yellow
        $updates['MinimumTlsVersion'] = "TLS1_2"
        $needsUpdate = $true
        $remediationCount++
    }
    
    # Check public access
    if ($storage.AllowBlobPublicAccess) {
        Write-Host "    🔧 Disabling public blob access..." -ForegroundColor Yellow
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
            Write-Host "    ✅ Remediation applied successfully" -ForegroundColor Green
            $successCount++
        } catch {
            Write-Host "    ❌ Remediation failed: $_" -ForegroundColor Red
            $failCount++
        }
    } elseif ($needsUpdate) {
        Write-Host "    ℹ️  Would apply remediation (simulation mode)" -ForegroundColor Blue
    } else {
        Write-Host "    ✅ No remediation needed" -ForegroundColor Green
    }
}

# ============================================
# 2. Fix Key Vault Security
# ============================================
Write-Host ""
Write-Host "🔐 Remediating Key Vaults..." -ForegroundColor Green
$keyVaults = Get-AzKeyVault -ResourceGroupName $ResourceGroupName

foreach ($vault in $keyVaults) {
    Write-Host "  Checking: $($vault.VaultName)..." -ForegroundColor Cyan
    
    $needsUpdate = $false
    
    # Check soft delete
    if (-not $vault.EnableSoftDelete) {
        Write-Host "    🔧 Enabling soft delete..." -ForegroundColor Yellow
        $needsUpdate = $true
        $remediationCount++
        
        if (-not $WhatIf) {
            try {
                Update-AzKeyVault -ResourceGroupName $ResourceGroupName `
                                  -VaultName $vault.VaultName `
                                  -EnableSoftDelete $true
                Write-Host "    ✅ Soft delete enabled" -ForegroundColor Green
                $successCount++
            } catch {
                Write-Host "    ❌ Failed to enable soft delete: $_" -ForegroundColor Red
                $failCount++
            }
        } else {
            Write-Host "    ℹ️  Would enable soft delete (simulation mode)" -ForegroundColor Blue
        }
    }
    
    # Check purge protection (requires soft delete)
    if (-not $vault.EnablePurgeProtection) {
        Write-Host "    🔧 Enabling purge protection..." -ForegroundColor Yellow
        $needsUpdate = $true
        $remediationCount++
        
        if (-not $WhatIf) {
            try {
                Update-AzKeyVault -ResourceGroupName $ResourceGroupName `
                                  -VaultName $vault.VaultName `
                                  -EnablePurgeProtection
                Write-Host "    ✅ Purge protection enabled" -ForegroundColor Green
                $successCount++
            } catch {
                Write-Host "    ❌ Failed to enable purge protection: $_" -ForegroundColor Red
                $failCount++
            }
        } else {
            Write-Host "    ℹ️  Would enable purge protection (simulation mode)" -ForegroundColor Blue
        }
    }
    
    if (-not $needsUpdate) {
        Write-Host "    ✅ No remediation needed" -ForegroundColor Green
    }
}

# ============================================
# Summary
# ============================================
Write-Host ""
Write-Host "📊 Remediation Summary" -ForegroundColor Cyan
Write-Host "======================" -ForegroundColor Cyan
Write-Host "Total issues identified: $remediationCount" -ForegroundColor White

if (-not $WhatIf) {
    Write-Host "Successfully fixed: $successCount" -ForegroundColor Green
    Write-Host "Failed to fix: $failCount" -ForegroundColor $(if ($failCount -gt 0) {"Red"} else {"Green"})
    
    if ($successCount -gt 0) {
        Write-Host ""
        Write-Host "✅ Auto-remediation complete!" -ForegroundColor Green
        Write-Host "   Run compliance-check.ps1 to verify fixes." -ForegroundColor Cyan
    }
} else {
    Write-Host ""
    Write-Host "ℹ️  Simulation complete. Run without -WhatIf to apply changes." -ForegroundColor Blue
}

Write-Host ""
Write-Host "💡 Note: Some issues require manual review (e.g., NSG rules)." -ForegroundColor Yellow
