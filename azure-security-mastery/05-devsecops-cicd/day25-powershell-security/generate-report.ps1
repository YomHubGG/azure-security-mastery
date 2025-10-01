<#
.SYNOPSIS
    Generate Professional Security Report
.DESCRIPTION
    Creates HTML and JSON security reports for Azure resources
.NOTES
    Author: Security Engineering Team
    Date: October 1, 2025
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$ResourceGroupName = "rg-learning-day1"
)

Write-Host "📊 Generating Security Report..." -ForegroundColor Cyan
Write-Host ""

# Collect data
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$report = @{
    GeneratedAt = $timestamp
    ResourceGroup = $ResourceGroupName
    Summary = @{}
    Resources = @()
    SecurityFindings = @()
}

# Get all resources
Write-Host "🔍 Collecting resource inventory..." -ForegroundColor Green
$resources = Get-AzResource -ResourceGroupName $ResourceGroupName

$report.Summary.TotalResources = $resources.Count
$report.Summary.StorageAccounts = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.Storage/storageAccounts"}).Count
$report.Summary.KeyVaults = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.KeyVault/vaults"}).Count
$report.Summary.NetworkSecurityGroups = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.Network/networkSecurityGroups"}).Count
$report.Summary.VirtualNetworks = ($resources | Where-Object {$_.ResourceType -eq "Microsoft.Network/virtualNetworks"}).Count

# Audit storage accounts
Write-Host "📦 Auditing storage accounts..." -ForegroundColor Green
$storageAccounts = Get-AzStorageAccount -ResourceGroupName $ResourceGroupName

foreach ($storage in $storageAccounts) {
    $resourceInfo = @{
        Name = $storage.StorageAccountName
        Type = "Storage Account"
        Location = $storage.Location
        HTTPSOnly = $storage.EnableHttpsTrafficOnly
        TLSVersion = $storage.MinimumTlsVersion
        PublicAccess = $storage.AllowBlobPublicAccess
        Status = "Secure"
    }
    
    if (-not $storage.EnableHttpsTrafficOnly -or $storage.AllowBlobPublicAccess) {
        $resourceInfo.Status = "At Risk"
        $report.SecurityFindings += "Storage: $($storage.StorageAccountName) has security issues"
    }
    
    $report.Resources += $resourceInfo
}

# Audit Key Vaults
Write-Host "🔐 Auditing Key Vaults..." -ForegroundColor Green
$keyVaults = Get-AzKeyVault -ResourceGroupName $ResourceGroupName

foreach ($vault in $keyVaults) {
    $resourceInfo = @{
        Name = $vault.VaultName
        Type = "Key Vault"
        Location = $vault.Location
        SoftDelete = $vault.EnableSoftDelete
        PurgeProtection = $vault.EnablePurgeProtection
        Status = "Secure"
    }
    
    if (-not $vault.EnableSoftDelete -or -not $vault.EnablePurgeProtection) {
        $resourceInfo.Status = "Needs Attention"
        $report.SecurityFindings += "Key Vault: $($vault.VaultName) missing protection features"
    }
    
    $report.Resources += $resourceInfo
}

# Calculate security score
$totalChecks = $report.Resources.Count
$secureResources = ($report.Resources | Where-Object {$_.Status -eq "Secure"}).Count
$report.Summary.SecurityScore = if ($totalChecks -gt 0) { [math]::Round(($secureResources / $totalChecks) * 100, 1) } else { 0 }

# Generate HTML report
Write-Host "📄 Generating HTML report..." -ForegroundColor Green

$statusColor = if ($report.Summary.SecurityScore -ge 80) { "#10b981" } elseif ($report.Summary.SecurityScore -ge 60) { "#f59e0b" } else { "#ef4444" }

$html = @"
<!DOCTYPE html>
<html>
<head>
    <title>Azure Security Report - $(Get-Date -Format 'yyyy-MM-dd')</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
            background: #f8fafc;
            padding: 40px 20px;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 { 
            color: #0f172a;
            font-size: 32px;
            margin-bottom: 10px;
        }
        .timestamp { color: #64748b; margin-bottom: 30px; }
        .summary-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .card {
            background: white;
            border-radius: 8px;
            padding: 24px;
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }
        .metric-value {
            font-size: 42px;
            font-weight: bold;
            color: #0078d4;
            margin-bottom: 8px;
        }
        .metric-label {
            color: #64748b;
            font-size: 14px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        .security-score {
            font-size: 56px;
            font-weight: bold;
            color: $statusColor;
        }
        .score-label { color: #64748b; font-size: 16px; }
        table {
            width: 100%;
            border-collapse: collapse;
            background: white;
            border-radius: 8px;
            overflow: hidden;
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }
        th {
            background: #0078d4;
            color: white;
            padding: 16px;
            text-align: left;
            font-weight: 600;
        }
        td {
            padding: 16px;
            border-bottom: 1px solid #e2e8f0;
        }
        tr:last-child td { border-bottom: none; }
        .status-badge {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 12px;
            font-size: 12px;
            font-weight: 600;
        }
        .status-secure { background: #d1fae5; color: #065f46; }
        .status-risk { background: #fee2e2; color: #991b1b; }
        .status-attention { background: #fed7aa; color: #92400e; }
        .findings {
            background: #fef3c7;
            border-left: 4px solid #f59e0b;
            padding: 16px;
            margin-top: 20px;
            border-radius: 4px;
        }
        .findings h3 { color: #92400e; margin-bottom: 10px; }
        .findings ul { margin-left: 20px; color: #78350f; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔒 Azure Security Report</h1>
        <p class="timestamp">Generated: $timestamp | Resource Group: $ResourceGroupName</p>
        
        <div class="summary-grid">
            <div class="card">
                <div class="security-score">$($report.Summary.SecurityScore)%</div>
                <div class="score-label">Security Score</div>
            </div>
            <div class="card">
                <div class="metric-value">$($report.Summary.TotalResources)</div>
                <div class="metric-label">Total Resources</div>
            </div>
            <div class="card">
                <div class="metric-value">$($report.Summary.StorageAccounts)</div>
                <div class="metric-label">Storage Accounts</div>
            </div>
            <div class="card">
                <div class="metric-value">$($report.Summary.KeyVaults)</div>
                <div class="metric-label">Key Vaults</div>
            </div>
            <div class="card">
                <div class="metric-value">$($report.Summary.NetworkSecurityGroups)</div>
                <div class="metric-label">Network Security Groups</div>
            </div>
            <div class="card">
                <div class="metric-value">$($report.Summary.VirtualNetworks)</div>
                <div class="metric-label">Virtual Networks</div>
            </div>
        </div>
        
        <h2 style="margin-bottom: 20px; color: #0f172a;">Resource Inventory</h2>
        <table>
            <tr>
                <th>Resource Name</th>
                <th>Type</th>
                <th>Location</th>
                <th>Status</th>
            </tr>
"@

foreach ($resource in $report.Resources) {
    $statusClass = switch ($resource.Status) {
        "Secure" { "status-secure" }
        "At Risk" { "status-risk" }
        "Needs Attention" { "status-attention" }
        default { "status-secure" }
    }
    
    $html += @"
            <tr>
                <td><strong>$($resource.Name)</strong></td>
                <td>$($resource.Type)</td>
                <td>$($resource.Location)</td>
                <td><span class="status-badge $statusClass">$($resource.Status)</span></td>
            </tr>
"@
}

$html += "</table>"

# Add findings if any
if ($report.SecurityFindings.Count -gt 0) {
    $html += @"
        <div class="findings">
            <h3>⚠️ Security Findings</h3>
            <ul>
"@
    foreach ($finding in $report.SecurityFindings) {
        $html += "<li>$finding</li>"
    }
    $html += @"
            </ul>
        </div>
"@
}

$html += @"
    </div>
</body>
</html>
"@

# Save HTML report
$htmlPath = "./security-report-$(Get-Date -Format 'yyyyMMdd').html"
$html | Out-File $htmlPath -Encoding UTF8
Write-Host "✅ HTML report saved: $htmlPath" -ForegroundColor Green

# Save JSON report
$jsonPath = "./security-report-$(Get-Date -Format 'yyyyMMdd').json"
$report | ConvertTo-Json -Depth 10 | Out-File $jsonPath -Encoding UTF8
Write-Host "✅ JSON report saved: $jsonPath" -ForegroundColor Green

Write-Host ""
Write-Host "📊 Report Summary" -ForegroundColor Cyan
Write-Host "Security Score: $($report.Summary.SecurityScore)%" -ForegroundColor $(if ($report.Summary.SecurityScore -ge 80) {"Green"} elseif ($report.Summary.SecurityScore -ge 60) {"Yellow"} else {"Red"})
Write-Host "Total Resources: $($report.Summary.TotalResources)" -ForegroundColor Cyan
Write-Host "Security Findings: $($report.SecurityFindings.Count)" -ForegroundColor $(if ($report.SecurityFindings.Count -eq 0) {"Green"} else {"Yellow"})
Write-Host ""
Write-Host "💡 Open $htmlPath in your browser to view the full report" -ForegroundColor Cyan
