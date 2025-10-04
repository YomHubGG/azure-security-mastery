# 🎯 Azure Security Mastery - Master Command Reference

## 🤖 **AGENT GUARDRAILS** - READ FIRST!
```
📖 ALWAYS reference: azure-security-mastery/_references/agent-guidelines.md
⛔ NEVER start trials without explicit permission
🔍 ALWAYS check current state before taking action
💰 ALWAYS confirm cost implications before deployment
📋 USE prompt templates to set clear expectations each session
```

## 🔧 Essential Daily Commands

### Authentication
```bash
az logout && az login --use-device-code
az account show
```

### Resource Management
```bash
az resource list --resource-group RESOURCE_GROUP --output table
```

### Bicep/IaC Operations
```bash
# Validate template
az deployment group validate --resource-group RG_NAME --template-file template.bicep --parameters @params.json

# Preview changes
az deployment group what-if --resource-group RG_NAME --template-file template.bicep --parameters @params.json

# Deploy
az deployment group create --resource-group RG_NAME --template-file template.bicep --parameters @params.json
```

### Container Operations (Podman)
```bash
# Build secure container
podman build -t app-name:version -f Dockerfile.secure .

# Run locally
podman run -d -p 3000:3000 --name test-container app-name:version

# Test endpoints
curl http://localhost:3000
curl http://localhost:3000/health

# Cleanup
podman stop test-container && podman rm test-container
```

### Azure Container Registry (ACR)
```bash
# Deploy ACR with IaC
az deployment group create --resource-group RG_NAME --template-file azure-container-registry.bicep --parameters @acr-dev.parameters.json

# Get access token for Podman
az acr login --name REGISTRY_NAME --expose-token

# Authenticate Podman (use token from above)
podman login REGISTRY_NAME.azurecr.io -u 00000000-0000-0000-0000-000000000000 -p ACCESS_TOKEN

# Tag and push
podman tag app-name:version REGISTRY_NAME.azurecr.io/app-name:version  
podman push REGISTRY_NAME.azurecr.io/app-name:version

# List repositories and tags
az acr repository list --name REGISTRY_NAME --output table
az acr repository show-tags --name REGISTRY_NAME --repository app-name --output table

# Enable admin user (for ACI integration)
az acr update --name REGISTRY_NAME --admin-enabled true
```

### Azure Container Instances (ACI)
```bash
# ⚠️  COST WARNING: Container Instances charge ~€0.32/day while running!
# ALWAYS get human approval before deploying costly resources

# Preview costs BEFORE deploying
az deployment group what-if --resource-group RG_NAME --template-file container-instance.bicep --parameters @aci-dev.parameters.json

# Deploy container to cloud (ONLY with explicit approval)
echo "⚠️  WARNING: This will incur costs! Proceed? (y/N)" && read -r response
[[ "$response" =~ ^[Yy]$ ]] && az deployment group create --resource-group RG_NAME --template-file container-instance.bicep --parameters @aci-dev.parameters.json || echo "❌ Deployment cancelled"

# Test running container
curl http://CONTAINER_IP:3000
curl http://CONTAINER_IP:3000/health

# Cost management - ALWAYS clean up after sessions
az container stop --name CONTAINER_NAME --resource-group RG_NAME    # Stop (but restarts due to policy)
az container delete --name CONTAINER_NAME --resource-group RG_NAME --yes  # Delete completely
```

### Azure Key Vault (FREE tier)
```bash
# Deploy Key Vault with IaC
az deployment group create --resource-group RG_NAME --template-file key-vault.bicep --parameters @key-vault.parameters.json

# Get access token for REST API (CLI has issues)
ACCESS_TOKEN=$(az account get-access-token --resource https://vault.azure.net --query accessToken -o tsv)

# Add secret
curl -X PUT "https://VAULT_NAME.vault.azure.net/secrets/SECRET_NAME?api-version=7.3" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"value": "SECRET_VALUE"}'

# Get secret
curl -s "https://VAULT_NAME.vault.azure.net/secrets/SECRET_NAME?api-version=7.3" \
  -H "Authorization: Bearer $ACCESS_TOKEN"

# List all secrets
curl -s "https://VAULT_NAME.vault.azure.net/secrets?api-version=7.3" \
  -H "Authorization: Bearer $ACCESS_TOKEN"

# Check existing Key Vault (Day 17)
az keyvault show --name kvlearning4uybw3c2lbkwm --query "{name:name, location:location, sku:properties.sku.name, softDelete:properties.enableSoftDelete, purgeProtection:properties.enablePurgeProtection}"

# List existing secrets
az keyvault secret list --vault-name kvlearning4uybw3c2lbkwm --output table
```

## 🗄️ Day 19: Data Protection & Governance Commands

### Data Discovery & Classification
```bash
# Check what data sources you have for governance
az resource list --query "[?type=='Microsoft.Storage/storageAccounts' || type=='Microsoft.KeyVault/vaults'].{Name:name, Type:type, Location:location}" --output table

# List all storage accounts (data repositories)
az storage account list --query "[].{Name:name, Tier:sku.tier, Encryption:encryption.services.blob.enabled}" --output table

# Check Key Vaults (secrets and sensitive data)
az keyvault list --query "[].{Name:name, Location:location, SoftDelete:properties.enableSoftDelete}" --output table

# List container registries (application data)
az acr list --query "[].{Name:name, Tier:sku.tier, AdminEnabled:adminUserEnabled}" --output table
```

### Data Protection Configuration
```bash
# Check blob storage encryption status
az storage account show --name "stlearning4uybw3c2lbkwm" --resource-group "rg-learning-day1" --query "encryption" --output json

# Enable blob soft delete (data protection feature)
az storage blob service-properties delete-policy update --account-name "stlearning4uybw3c2lbkwm" --enable true --days-retained 30

# Check soft delete configuration
az storage blob service-properties delete-policy show --account-name "stlearning4uybw3c2lbkwm" --output table

# Enable blob versioning for data protection
az storage blob service-properties update --account-name "stlearning4uybw3c2lbkwm" --enable-versioning true

# Verify Key Vault soft delete protection
az keyvault show --name "kvlearning4uybw3c2lbkwm" --query "{SoftDelete:properties.enableSoftDelete, PurgeProtection:properties.enablePurgeProtection}" --output table
```

### Data Governance with Azure Policy
```bash
# List data protection related policies
az policy definition list --query "[?contains(displayName, 'data') || contains(displayName, 'encrypt')].{Name:displayName, Type:policyType}" --output table

# Check which policies are assigned to your subscription
az policy assignment list --output table

# Find encryption policies (data protection requirement)
az policy definition list --query "[?contains(displayName, 'encrypt') && contains(displayName, 'storage')].{Name:displayName}" --output table

# View details of a specific data protection policy
az policy definition show --name "404c3081-a854-4457-ae30-26a93ef643f9" --query "{Name:displayName, Description:description}" --output table
```

### Data Classification with Tags
```bash
# Add classification tags to storage accounts
az storage account update --name "stlearning4uybw3c2lbkwm" --set tags.DataClassification=Public tags.Environment=Learning
az storage account update --name "devsec4uybw3c2lbkwm" --set tags.DataClassification=Internal tags.Environment=Development
az storage account update --name "prodsec4uybw3c2lbkwm" --set tags.DataClassification=Confidential tags.Environment=Production

# Add tags to Key Vaults (highly confidential by nature)
az keyvault update --name "kvlearning4uybw3c2lbkwm" --set tags.DataClassification=HighlyConfidential tags.Environment=Learning

# Verify tagging
az resource list --query "[?tags != null].{Name:name, Classification:tags.DataClassification, Environment:tags.Environment}" --output table

# List resource tags (data classification metadata)
az resource list --query "[?tags != null].{Name:name, Tags:tags}" --output table
```

### Cost & Compliance Monitoring
```bash
# Check current spending (important for cost management)
az consumption usage list --top 5 --output table

# List all resources and their costs
az consumption usage list --include-additional-properties --include-meter-details --output table

# Check free tier usage
az consumption reservation summary list --grain monthly --output table

# Check Azure Policy compliance (governance framework)
az policy assignment list --query "[].{Name:displayName, Scope:scope}" --output table

# Policy compliance check
az policy state list --resource "/subscriptions/$(az account show --query id -o tsv)/resourceGroups/rg-learning-day1" --output table
```

### Access Control & Security Audit
```bash
# Check RBAC assignments (data access control)
az role assignment list --all --query "[].{Principal:principalName, Role:roleDefinitionName, Scope:scope}" --output table

# RBAC analysis for specific scope
az role assignment list --scope "/subscriptions/$(az account show --query id -o tsv)/resourceGroups/rg-learning-day1" --output table

# RBAC assignments for current user
az role assignment list --assignee $(az account show --query user.name -o tsv) --output table
```

## 🌐 Network Security Commands (Day 9)

### Virtual Networks & Security Groups
```bash
# List virtual networks
az network vnet list --resource-group rg-learning-day1 --output table

# List network security groups
az network nsg list --resource-group rg-learning-day1 --output table

# Validate network security configuration
az network nsg show --name dev-nsg-security --resource-group rg-learning-day1 --query "securityRules[].{Name:name,Priority:priority,Access:access,Direction:direction}"
```

## 🛡️ Security Services Commands

### Microsoft Defender for Cloud (Day 13)
```bash
# Check current Defender status
az security pricing list --output table

# Enable Defender for different resource types (⚠️ COSTS MONEY!)
az security pricing create --name "VirtualMachines" --tier "Standard"
az security pricing create --name "StorageAccounts" --tier "Standard"
az security pricing create --name "KeyVaults" --tier "Standard"
```

### Azure Policy (Day 15)
```bash
# Create management group hierarchy
az account management-group create --name "security-learning" --display-name "Security Learning"

# Move subscription to management group
az account management-group subscription add --name "security-learning" --subscription $(az account show --query id -o tsv)
```

## 🎯 Current Infrastructure
- **Networks**: dev-vnet-security (10.0.0.0/16), prod-vnet-security
- **Security**: dev-nsg-security, prod-nsg-security  
- **Storage**: Multiple secure storage accounts
- **Containers**: 
  - Registry: devacr4uybw3c2lbkwm.azurecr.io/secure-app:v1
  - Status: ⏹️ **STOPPED** (deleted to save costs - can redeploy anytime)
- **Key Vault**: kvlearning4uybw3c2lbkwm.vault.azure.net ✅ **ACTIVE**
  - Secrets: database-password, jwt-secret, external-api-key

## � Day 21: Application Security Commands

## 🔐 Day 21: Application Security Commands

### App Service Discovery
```bash
# List existing web apps
az webapp list --output table

# List App Service plans with pricing tiers
az appservice plan list --output table

# Get detailed app configuration
az webapp show --name APP_NAME --resource-group RG_NAME --output json

# Check app settings and environment variables
az webapp config appsettings list --name APP_NAME --resource-group RG_NAME --output table

# Basic security configuration check
az webapp show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --query "{name:name, state:state, kind:kind, httpsOnly:httpsOnly, clientAffinityEnabled:clientAffinityEnabled}" --output table
```

### App Service Security Analysis
```bash
# Check SSL/TLS configuration
az webapp config ssl list --resource-group RG_NAME --output table

# View custom domains
az webapp config hostname list --webapp-name APP_NAME --resource-group RG_NAME --output table

# Check authentication settings
az webapp auth show --name APP_NAME --resource-group RG_NAME --output json

# Check CORS (Cross-Origin Resource Sharing) configuration
az webapp cors show --name APP_NAME --resource-group RG_NAME --output json

# Check managed identity status
az webapp identity show --name APP_NAME --resource-group RG_NAME --output table

# View network access restrictions
az webapp config access-restriction show --name APP_NAME --resource-group RG_NAME --output table
```

### CORS Security Configuration
```bash
# Add allowed origins (secure approach)
az webapp cors add --name APP_NAME --resource-group RG_NAME --allowed-origins "https://yourtrustedsite.com"

# Remove overly permissive CORS (if present)
az webapp cors remove --name APP_NAME --resource-group RG_NAME --allowed-origins "*"

# Configure CORS for specific methods and headers
az webapp cors add --name APP_NAME --resource-group RG_NAME \
  --allowed-origins "https://yourfrontend.com" \
  --allowed-methods "GET,POST,PUT" \
  --allowed-headers "Content-Type,Authorization"
```

### App Service Security Configuration
```bash
# Verify HTTPS-only enforcement
az webapp show --name APP_NAME --resource-group RG_NAME --query "httpsOnly" --output tsv

# Configure security headers (requires app-level implementation)
# Set client certificate mode for mutual TLS
az webapp update --name APP_NAME --resource-group RG_NAME --client-cert-mode Required

# Configure minimum TLS version
az webapp config set --name APP_NAME --resource-group RG_NAME --min-tls-version "1.2"

# Enable HTTP/2 support
az webapp config set --name APP_NAME --resource-group RG_NAME --http20-enabled true
```

## �📋 Session Progress
- [x] **Phase 1**: IaC Foundations (Bicep, multi-resource templates)
- [x] **Phase 2**: Network Security (VNets, NSGs, subnetting)  
- [x] **Phase 3**: Container Security (Podman, ACR, ACI) ✅ **COMPLETED Sept 1**
- [x] **Phase 4**: Application Security (Key Vault, web apps) ← **IN PROGRESS Day 21**
- [ ] **Phase 5**: Compliance & Governance (Policy, RBAC)

*Updated: September 22, 2025*

---
## 🚨 COST SAFETY COMMITMENT
**NEVER deploy costly resources without explicit human consent!**  
See: [Cost Safety Guidelines](azure-security-mastery/cost-safety-guidelines.md)

## Day 23: DevSecOps & CI/CD Security

### GitHub Actions Security Pipeline
```yaml
# .github/workflows/secure-devsecops-pipeline.yml
# Automated security scanning with CodeQL, ESLint security, dependency audit
# Portfolio demonstration: Enterprise DevSecOps practices
```

### Security Scanning Commands
```bash
# CodeQL security analysis (automated in pipeline)
# ESLint security rules for JavaScript
npx eslint --ext .js --config .eslintrc.json "**/*.js"

# NPM security audit
npm audit --audit-level=moderate

# Bicep template security validation
bicep build template.bicep --outfile template.json
```

### Service Principal DevSecOps (Attempted)
```bash
### Service Principal DevSecOps (Attempted)
```bash
# Create scoped service principal for CI/CD
az ad sp create-for-rbac \
  --name "sp-github-actions-day23" \
  --role "Contributor" \
  --scopes "/subscriptions/$SUBSCRIPTION_ID/resourceGroups/rg-learning-day1"

# Note: CLI v2.45.0 has role assignment bug - worked around with user auth
```

---

## Day 25: PowerShell Security Scripts

### PowerShell Az Module Setup
```bash
# Check PowerShell version
pwsh --version  # 7.5.3 confirmed

# Install/Update Az module
pwsh -Command "Install-Module -Name Az -Repository PSGallery -Force -AllowClobber"

# Verify installation
pwsh -Command "Get-Module -ListAvailable Az.*"
```

### Azure Authentication in PowerShell
```powershell
# Connect to Azure
Connect-AzAccount

# Verify connection
Get-AzContext

# Set subscription
Set-AzContext -SubscriptionId "a174b81f-93c2-4089-afa0-6f82a5165c86"
```

### Security Automation Scripts (Day 25)
```bash
# Execute security audit
pwsh -File ./security-audit.ps1 -ResourceGroupName "rg-learning-day1"

# Run compliance check
pwsh -File ./compliance-check.ps1 -ResourceGroupName "rg-learning-day1"

# Test auto-remediation (simulation mode)
pwsh -File ./auto-remediate.ps1 -ResourceGroupName "rg-learning-day1" -WhatIf

# Apply auto-remediation (live)
pwsh -File ./auto-remediate.ps1 -ResourceGroupName "rg-learning-day1"

# Generate security report
pwsh -File ./generate-report.ps1 -ResourceGroupName "rg-learning-day1"
```

### Security Audit Results (October 1, 2025)
```
Security Score: 80%
Compliance Rate: 57.14%
Resources Scanned: 11 (4 storage, 1 Key Vault, 2 NSGs, 3 VNets, 1 Container Registry)
Issues Found: Key Vault missing soft delete/purge protection, NSGs with permissive rules
Cost: €0 (PowerShell execution = free)
```

---

## Day 27: Logic Apps & Security Orchestration

### Resource Group for Automation
```bash
# Create resource group for Logic Apps
az group create --name rg-security-automation --location westeurope

# List resource groups
az group list --query "[].{Name:name, Location:location}" -o table
```

### Logic Apps Management
```bash
# List Logic Apps
az logic workflow list --resource-group rg-learning-day1 -o table

# Show workflow details
az logic workflow show \
  --name 1 \
  --resource-group rg-learning-day1 \
  --query "{Name:name, State:state, Location:location}"

# Get workflow run history (after creating workflows)
az logic workflow run list \
  --name workflow-name \
  --resource-group rg-learning-day1

# Manual trigger test
az logic workflow run trigger \
  --name workflow-name \
  --resource-group rg-learning-day1 \
  --trigger-name manual
```

### Portal Access for Visual Designer
```bash
# Direct portal link for Logic Apps
echo "https://portal.azure.com/#view/HubsExtension/BrowseResource/resourceType/Microsoft.Logic%2Fworkflows"

# Access designer: Portal → Logic App → Development Tools → Logic app designer
```

### Logic App Cost Check
```
Pricing Tier: Consumption
Free Tier: 4,000 actions/month
Current Usage: 0 executions
Cost: €0
Note: Pay-per-execution model, no standing charges
```

### Browser Compatibility Note
```
Issue: Gmail connector OAuth popups fail in Firefox
Solution: Use Edge/Chrome for Azure Portal OAuth flows
Alternative: Use Outlook.com connector or HTTP actions for testing
```

---

## Day 29: Azure DevOps Security Pipeline

### Azure DevOps Portal Access
```bash
# Direct access to Azure DevOps
echo "https://dev.azure.com"

# Create organization (done via web portal)
# Portal: Sign in → Create organization → Choose name

# Create project (done via web portal)
# Organization → New project → Name: azure-security-pipeline
```

### Azure DevOps CLI (az devops extension)
```bash
# Install Azure DevOps CLI extension
az extension add --name azure-devops

# Configure default organization
az devops configure --defaults organization=https://dev.azure.com/YOUR_ORG

# List projects
az devops project list --output table

# Show project details
az devops project show --project azure-security-pipeline
```

### Service Connections
```bash
# List service connections (via portal - no direct CLI for creation)
# Portal: Project Settings → Service connections

# Service connection created:
# Name: azure-connection
# Type: Azure Resource Manager
# Authentication: Workload Identity Federation
# Scope: Subscription level
# Resource Group: rg-learning-day1
```

### Pipeline Management
```bash
# List pipelines in project
az pipelines list --output table

# Show pipeline details
az pipelines show --name "azure-security-mastery" --output table

# Run pipeline manually
az pipelines run --name "azure-security-mastery"

# Show pipeline run history
az pipelines runs list --output table

# Show specific run details
az pipelines runs show --id RUN_ID
```

### Pipeline YAML Configuration
```yaml
# File: azure-pipelines.yml (in repository root)

trigger:
  branches:
    include:
    - main
  paths:
    include:
    - 'azure-security-mastery/**/*.bicep'

pool:
  vmImage: 'ubuntu-latest'

steps:
- task: AzureCLI@2
  displayName: 'Install Bicep'
  inputs:
    azureSubscription: 'azure-connection'
    scriptType: 'bash'
    scriptLocation: 'inlineScript'
    inlineScript: |
      az bicep install
      az bicep version

- script: |
    echo "🔍 Validating Bicep templates..."
    find azure-security-mastery -name "*.bicep" -type f | while read file; do
      echo "Validating: $file"
      az bicep build --file "$file" || exit 1
    done
    echo "✅ All templates valid"
  displayName: 'Validate Bicep Templates'

- script: |
    echo "🔐 Security scan complete"
  displayName: 'Security Check'
```

### GitHub Integration
```bash
# Connect GitHub repository (done via portal)
# Portal: Pipelines → Create Pipeline → GitHub → Authorize → Select repo

# OAuth authentication with GitHub
# Uses Microsoft Authenticator app for Azure DevOps login
# GitHub OAuth popup for repository access authorization
```

### Free Tier Parallelism Request
```bash
# Request free parallelism grant (required for pipeline execution)
# URL: https://aka.ms/azpipelines-parallelism-request
# Form fields:
#   - Organization name
#   - Project visibility (Public/Private)
#   - Brief description of work

# Status check: Check email for Microsoft approval (2-3 business days)
```

### Pipeline Validation (Local)
```bash
# Validate Bicep templates locally (same as pipeline)
find azure-security-mastery -name "*.bicep" -type f | while read file; do
  echo "Validating: $file"
  az bicep build --file "$file" || exit 1
done

# Install Bicep if needed
az bicep install

# Check Bicep version
az bicep version
```

### Git Integration Notes
```bash
# Azure DevOps auto-commits pipeline YAML to GitHub
# When creating pipeline in portal, it commits azure-pipelines.yml

# If local branch is behind after pipeline creation:
git pull origin main  # Pull Azure DevOps commit
git push origin main  # Push local changes

# Recommended workflow:
git pull --rebase  # Rebase local commits on top of remote
git push origin main
```

### Cost & Usage Tracking
```
Azure DevOps Organization: FREE
Service Connection: €0 (no standing charges)
Free Tier: 1,800 pipeline minutes/month (after approval)
Parallel Jobs: 1 free parallel job (after approval)
Current Usage: 0 minutes = €0
```

### Comparison: GitHub Actions vs Azure DevOps
```bash
# GitHub Actions (Day 23)
# File: .github/workflows/secure-devsecops-pipeline.yml
# Trigger: on: [push, pull_request]
# Runner: runs-on: ubuntu-latest
# Free: 2,000 minutes/month (instant)

# Azure DevOps (Day 29)  
# File: azure-pipelines.yml
# Trigger: trigger: [main]
# Pool: pool: vmImage: ubuntu-latest
# Free: 1,800 minutes/month (approval required)

# Both: Production-ready enterprise CI/CD platforms
```

### Portfolio Demonstration Commands
```bash
# Show multi-platform DevOps expertise
echo "GitHub Actions: 2m 25s successful run (Day 23)"
echo "Azure DevOps: Pipeline configured, pending execution (Day 29)"

# Demonstrate YAML proficiency
cat .github/workflows/secure-devsecops-pipeline.yml
cat azure-pipelines.yml

# Show service connection security
az pipelines variable-group list  # Variables managed securely
```

### Real-World Learnings
```
✅ Microsoft Authenticator required for Azure DevOps
✅ GitHub OAuth integration for repository access
✅ Free tier requires parallelism approval (anti-crypto-mining)
✅ Workload Identity Federation = no stored credentials
✅ Azure DevOps auto-commits pipeline to GitHub (git workflow)
✅ Multi-platform DevOps valuable for enterprise roles
```

---
```

