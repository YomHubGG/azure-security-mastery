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

# 4. Virtual Network Security Check
Write-Host "🌐 Auditing Virtual Networks..." -ForegroundColor Green
$vnets = Get-AzVirtualNetwork -ResourceGroupName $ResourceGroupName

foreach ($vnet in $vnets) {
    $audit = @{
        ResourceName = $vnet.Name
        ResourceType = "Virtual Network"
        AddressSpace = ($vnet.AddressSpace.AddressPrefixes -join ", ")
        SubnetCount = $vnet.Subnets.Count
        Location = $vnet.Location
        Status = "PASS"
        Issues = @()
    }
    
    # Check if subnets have NSGs
    foreach ($subnet in $vnet.Subnets) {
        if ($null -eq $subnet.NetworkSecurityGroup) {
            $audit.Status = "WARNING"
            $audit.Issues += "Subnet '$($subnet.Name)' has no NSG attached"
        }
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
Write-Host "⚠️  WARNING: $warnCount" -ForegroundColor Yellow
Write-Host "❌ FAIL: $failCount" -ForegroundColor Red
Write-Host ""

# Display detailed results
Write-Host "Detailed Findings:" -ForegroundColor Cyan
$results | Format-Table ResourceName, ResourceType, Status, @{Name="Issues";Expression={$_.Issues -join ", "}} -AutoSize

# Export to JSON
$reportPath = "./security-audit-report-$(Get-Date -Format 'yyyyMMdd-HHmmss').json"
$results | ConvertTo-Json -Depth 10 | Out-File $reportPath
Write-Host "📄 Full report saved to: $reportPath" -ForegroundColor Green

# Return results
return $results
