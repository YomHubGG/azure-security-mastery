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

### GitHub Container Registry (ghcr.io) - FREE Alternative
```bash
# Create Personal Access Token at: https://github.com/settings/tokens
# Scopes needed: write:packages, read:packages, delete:packages

# Login to GitHub Container Registry
echo $GITHUB_TOKEN | podman login ghcr.io -u YOUR_USERNAME --password-stdin

# Tag image for GitHub
podman tag app-name:version ghcr.io/username/app-name:version
podman tag app-name:version ghcr.io/username/app-name:latest

# Push to GitHub Container Registry
podman push ghcr.io/username/app-name:version
podman push ghcr.io/username/app-name:latest

# Pull from GitHub (public images)
podman pull ghcr.io/username/app-name:version

# Pull private images (authenticate first)
echo $GITHUB_TOKEN | podman login ghcr.io -u username --password-stdin
podman pull ghcr.io/username/app-name:version

# List your packages: https://github.com/username?tab=packages

# Logout
podman logout ghcr.io
```

## Day 37: Container Registry Security

### ACR Security Assessment
```bash
# Check ACR name availability
az acr check-name --name yourdesiredname

# List existing ACRs
az acr list --output table

# ACR pricing tiers (2025):
# Basic: €4.60/month, 10GB, 2 webhooks
# Standard: €18.40/month, 100GB, 10 webhooks
# Premium: €92/month, 500GB, 500 webhooks, geo-replication, content trust
```

### ACR Authentication Methods (Ranked by Security)
```bash
# 1. MANAGED IDENTITY (Best - no credentials!)
az aks create --attach-acr ACR_NAME --enable-managed-identity

# 2. AZURE AD USER (Good - personal MFA)
az acr login --name ACR_NAME

# 3. SERVICE PRINCIPAL (Good - scoped for CI/CD)
az ad sp create-for-rbac \
  --name "ci-cd-acr" \
  --role acrpush \
  --scopes /subscriptions/{sub}/resourceGroups/{rg}/providers/Microsoft.ContainerRegistry/registries/{acr}

# 4. ADMIN USER (⚠️ Emergency only - shared password)
az acr update --name ACR_NAME --admin-enabled true
az acr credential show --name ACR_NAME
```

### ACR Network Security
```bash
# Add IP allowlist rule
az acr network-rule add \
  --name ACR_NAME \
  --ip-address YOUR_IP/32

# List network rules
az acr network-rule list --name ACR_NAME --output table

# Remove network rule
az acr network-rule remove \
  --name ACR_NAME \
  --ip-address YOUR_IP/32

# Create private endpoint (Premium tier only)
az network private-endpoint create \
  --name acr-private-endpoint \
  --resource-group RG_NAME \
  --vnet-name VNET_NAME \
  --subnet SUBNET_NAME \
  --private-connection-resource-id /subscriptions/{sub}/resourceGroups/{rg}/providers/Microsoft.ContainerRegistry/registries/{acr} \
  --group-id registry \
  --connection-name acr-connection
```

### ACR Content Trust (Image Signing)
```bash
# Enable content trust (Premium tier only)
az acr config content-trust update \
  --name ACR_NAME \
  --status enabled

# Check content trust status
az acr config content-trust show --name ACR_NAME

# Sign and push with Docker
export DOCKER_CONTENT_TRUST=1
docker push myacr.azurecr.io/app:v1.0.0

# Sign and push with Podman (requires skopeo)
skopeo copy \
  --sign-by GPG_KEY_ID \
  containers-storage:app:v1.0.0 \
  docker://myacr.azurecr.io/app:v1.0.0
```

### ACR Vulnerability Scanning
```bash
# Enable Microsoft Defender for Containers
az security pricing create \
  --name ContainerRegistry \
  --tier Standard

# View vulnerability scan results
az security assessment list \
  --resource-group RG_NAME \
  --resource ACR_NAME \
  --resource-type Microsoft.ContainerRegistry/registries

# List quarantined images
az acr repository show-tags \
  --name ACR_NAME \
  --repository REPO_NAME \
  --query "[?quarantine=='true']"
```

### ACR Lifecycle Management
```bash
# Enable retention policy (Premium tier - delete untagged manifests)
az acr config retention update \
  --name ACR_NAME \
  --status enabled \
  --days 7 \
  --type UntaggedManifests

# Lock production image (prevent deletion/overwrite)
az acr repository update \
  --name ACR_NAME \
  --image app:v1.0.0 \
  --write-enabled false \
  --delete-enabled false

# Unlock image
az acr repository update \
  --name ACR_NAME \
  --image app:v1.0.0 \
  --write-enabled true \
  --delete-enabled true

# Check storage usage
az acr show-usage --name ACR_NAME --output table
```

### ACR Geo-Replication (Premium Only)
```bash
# Create replica in another region
az acr replication create \
  --registry ACR_NAME \
  --location northeurope

# List replicas
az acr replication list \
  --registry ACR_NAME \
  --output table

# Delete replica
az acr replication delete \
  --registry ACR_NAME \
  --location northeurope
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

## Day 43: Local Kubernetes Practice

### k3s Installation & Management
```bash
# Install k3s (lightweight Kubernetes)
curl -sfL https://get.k3s.io | sh -

# Check k3s status
sudo systemctl status k3s

# Check k3s version
k3s --version  # v1.33.5+k3s1

# Get kubeconfig
sudo cat /etc/rancher/k3s/k3s.yaml > ~/.kube/config
sudo chown $USER:$USER ~/.kube/config
chmod 600 ~/.kube/config

# Verify kubectl access
kubectl get nodes
kubectl get pods --all-namespaces
```

### Essential kubectl Commands
```bash
# Get cluster info
kubectl cluster-info
kubectl get nodes -o wide

# Work with namespaces
kubectl get namespaces
kubectl create namespace secure-app

# Deploy application
kubectl apply -f 01-namespace.yaml
kubectl apply -f 02-deployment.yaml
kubectl apply -f 03-service.yaml
kubectl apply -f 04-network-policy.yaml

# Check deployments
kubectl get deployments -n secure-app
kubectl get pods -n secure-app -o wide
kubectl get services -n secure-app

# Describe resources (detailed info)
kubectl describe deployment secure-app -n secure-app
kubectl describe pod <pod-name> -n secure-app
kubectl describe service secure-app -n secure-app

# View logs
kubectl logs <pod-name> -n secure-app
kubectl logs <pod-name> -n secure-app --tail=50
kubectl logs -f <pod-name> -n secure-app  # Follow logs

# Execute commands in pod
kubectl exec -it <pod-name> -n secure-app -- sh
kubectl exec -it <pod-name> -n secure-app -- env
kubectl exec -it <pod-name> -n secure-app -- wget -O- http://localhost:3000

# Scaling operations
kubectl scale deployment secure-app --replicas=5 -n secure-app
kubectl scale deployment secure-app --replicas=2 -n secure-app

# Test self-healing (delete pod, K8s recreates)
kubectl delete pod <pod-name> -n secure-app
kubectl get pods -n secure-app -w  # Watch pods recreate

# Test service load balancing
curl http://localhost:30080  # Hits different pods

# Cleanup
kubectl delete namespace secure-app
```

### Kubernetes Manifest Structure (YAML)
```yaml
# 01-namespace.yaml - Logical isolation
apiVersion: v1
kind: Namespace
metadata:
  name: secure-app

# 02-deployment.yaml - Pod management with replicas
apiVersion: apps/v1
kind: Deployment
metadata:
  name: secure-app
  namespace: secure-app
spec:
  replicas: 3  # Desired state (self-healing maintains this)
  selector:
    matchLabels:
      app: secure-app
  template:
    spec:
      securityContext:
        runAsNonRoot: true
        fsGroup: 1001
      containers:
      - name: secure-app
        image: ghcr.io/yomhubgg/secure-app:1.0.0
        securityContext:
          runAsUser: 1001
          allowPrivilegeEscalation: false
          capabilities:
            drop: ["ALL"]
        resources:
          limits:
            cpu: "250m"      # Anti-cryptomining
            memory: "256Mi"
        livenessProbe:       # Restart if unhealthy
          httpGet:
            path: /
            port: 3000
        readinessProbe:      # Remove from service if not ready
          httpGet:
            path: /
            port: 3000

# 03-service.yaml - Stable network endpoint + load balancing
apiVersion: v1
kind: Service
metadata:
  name: secure-app
  namespace: secure-app
spec:
  type: NodePort
  ports:
  - port: 80
    targetPort: 3000
    nodePort: 30080  # Access via localhost:30080
  selector:
    app: secure-app  # Routes to pods with this label

# 04-network-policy.yaml - Pod firewall
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: secure-app-netpol
  namespace: secure-app
spec:
  podSelector:
    matchLabels:
      app: secure-app
  policyTypes:
  - Ingress
  - Egress
  egress:
  - to:
    - namespaceSelector: {}
    ports:
    - protocol: UDP
      port: 53  # Allow DNS
  - to:
    - podSelector:
        matchLabels:
          app: secure-app
    ports:
    - protocol: TCP
      port: 3000  # Allow pod-to-pod
```

### Kubernetes Troubleshooting
```bash
# Check pod status and events
kubectl get pods -n secure-app
kubectl describe pod <pod-name> -n secure-app

# Check logs for errors
kubectl logs <pod-name> -n secure-app --previous  # Previous crashed container

# Check resource usage
kubectl top nodes
kubectl top pods -n secure-app

# Debug network connectivity
kubectl exec -it <pod-name> -n secure-app -- sh
# Inside pod:
ping 10.42.0.1  # Check network
wget -O- http://10.42.0.9:3000  # Test pod-to-pod
nslookup kubernetes.default  # Test DNS

# Check service endpoints
kubectl get endpoints -n secure-app

# Validate YAML before applying
kubectl apply -f deployment.yaml --dry-run=client
```

### Real-World Learnings
```
✅ Self-healing: Kubernetes maintains desired state (replicas: 3 = always 3 pods)
✅ Scaling: kubectl scale --replicas=N (declarative, automatic)
✅ Load balancing: Service distributes traffic across pod IPs
✅ Network Policies: Pod-level firewall (cryptomining prevention)
✅ Resource limits: CPU 250m max prevents abuse
✅ Health probes: liveness (restart) vs readiness (traffic routing)
✅ k3s = 95% of AKS capabilities for €0
✅ Local practice = production-ready skills without €238/month
```

---

## Day 45: GitHub Actions Advanced Security with OIDC

### Azure OIDC App Registration & Federated Credentials
```bash
# Create app registration for OIDC
az ad app create \
  --display-name "github-oidc-cybersecurity-journey"

# Output: appId: 1eba6d9f-ceb6-4101-adc6-d86d2142fd43

# Create Service Principal
az ad sp create --id 1eba6d9f-ceb6-4101-adc6-d86d2142fd43

# Output: objectId: 5f37f027-63a7-49f2-958f-50932f4aef38

# Create federated credential for GitHub Actions
az ad app federated-credential create \
  --id 1eba6d9f-ceb6-4101-adc6-d86d2142fd43 \
  --parameters '{
    "name": "github-actions-main",
    "issuer": "https://token.actions.githubusercontent.com",
    "subject": "repo:YomHubGG/azure-security-mastery:ref:refs/heads/main",
    "audiences": ["api://AzureADTokenExchange"]
  }'

# Assign Contributor role (use Portal due to CLI bug)
# Portal → rg-learning-day1 → IAM → Add role assignment → Contributor
# → Select "github-oidc-cybersecurity-journey"

# Get tenant and subscription IDs
az account show --query "{tenantId:tenantId, subscriptionId:id}"
# Tenant: 286ee762-df63-4515-be88-d6a2032dfe6f
# Subscription: a174b81f-93c2-4089-afa0-6f82a5165c86
```

### GitHub Secrets Configuration
```bash
# Add to: https://github.com/YomHubGG/azure-security-mastery/settings/secrets/actions

AZURE_CLIENT_ID: 1eba6d9f-ceb6-4101-adc6-d86d2142fd43
AZURE_TENANT_ID: 286ee762-df63-4515-be88-d6a2032dfe6f
AZURE_SUBSCRIPTION_ID: a174b81f-93c2-4089-afa0-6f82a5165c86
```

### GitHub CLI - Pipeline Monitoring
```bash
# Install gh CLI (if needed)
sudo apt install gh -y

# Authenticate
gh auth login

# List workflow runs
gh run list --limit 10
gh run list --workflow="day45-secure-pipeline.yml" --limit 5

# Watch run in real-time
gh run watch <run-id> --interval 5
gh run watch --interval 5  # Interactive selection

# View run details
gh run view <run-id>
gh run view <run-id> --log
gh run view <run-id> --log-failed

# View specific job logs
gh run view <run-id> --job <job-id>

# Download artifacts
gh run download <run-id>
gh run download <run-id> --name sbom-<sha>

# Rerun failed workflow
gh run rerun <run-id>

# Cancel running workflow
gh run cancel <run-id>
```

### Security Scanning Tools (Integrated in Pipeline)
```bash
# Gitleaks - Secret scanning
docker run --rm -v $(pwd):/src zricethezav/gitleaks:latest detect --source /src

# Trivy - Container vulnerability scanning
trivy image secure-app:latest
trivy image --severity CRITICAL,HIGH secure-app:latest
trivy image --format sarif --output trivy-results.sarif secure-app:latest

# Syft - SBOM generation
syft packages secure-app:latest -o spdx-json > sbom.spdx.json

# npm audit - Dependency vulnerabilities
npm audit --audit-level=moderate
npm audit --json > npm-audit.json

# CodeQL - SAST analysis (automated in GitHub Actions)
# Analyzes JavaScript code for security vulnerabilities
```

### Docker Image Building & Scanning
```bash
# Build container with embedded app
docker build -t secure-app:latest -f Dockerfile.day45 .

# Load image for scanning (buildx)
docker buildx build --load -t secure-app:latest -f Dockerfile.day45 .

# List local images
docker images

# Scan with Trivy
trivy image secure-app:latest

# Generate SBOM with Syft
syft secure-app:latest -o spdx-json > sbom.json

# Run container locally
docker run -d -p 3000:3000 --name test-app secure-app:latest
curl http://localhost:3000

# Cleanup
docker stop test-app && docker rm test-app
```

### Pipeline Workflow Structure (.github/workflows/day45-secure-pipeline.yml)
```yaml
name: Secure DevSecOps Pipeline - Day 45 Advanced

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

permissions:
  id-token: write        # Required for OIDC authentication
  contents: read         # Required for checkout
  security-events: write # Required for CodeQL
  issues: write          # Required for security summary

jobs:
  security-scanning:
    name: 🔒 Security Scanning
    runs-on: ubuntu-latest
    steps:
      - name: 🔐 Scan for Leaked Secrets (Gitleaks)
        uses: gitleaks/gitleaks-action@v2
      
      - name: 🛡️ Initialize CodeQL Analysis
        uses: github/codeql-action/init@v3
      
      - name: 🔍 Audit Dependencies
        run: npm audit --audit-level=moderate
      
      - name: 🔬 Perform CodeQL Analysis
        uses: github/codeql-action/analyze@v3

  build-and-scan:
    name: 🐳 Build & Scan Container
    runs-on: ubuntu-latest
    needs: security-scanning
    steps:
      - name: 🐳 Build Container Image
        uses: docker/build-push-action@v5
        with:
          load: true  # Load into Docker daemon for scanning
          
      - name: 🔍 Scan Image with Trivy
        uses: aquasecurity/trivy-action@master
        
      - name: 📋 Generate SBOM
        uses: anchore/sbom-action@v0

  deploy:
    name: 🚀 Deploy to Azure
    runs-on: ubuntu-latest
    needs: build-and-scan
    if: github.ref == 'refs/heads/main'
    steps:
      - name: 🔐 Azure Login via OIDC
        uses: azure/login@v2
        with:
          client-id: ${{ secrets.AZURE_CLIENT_ID }}
          tenant-id: ${{ secrets.AZURE_TENANT_ID }}
          subscription-id: ${{ secrets.AZURE_SUBSCRIPTION_ID }}
      
      - name: 🚀 Deploy to ACI
        run: az container create ...
```

### Debugging Pipeline Issues
```bash
# Check recent runs
gh run list --limit 5

# View failed logs with context
gh run view <run-id> --log-failed | grep -B 5 -A 5 "ERROR"

# Common issues and fixes:
# 1. Image not found: Add load: true to docker build
# 2. Registry name: Must be lowercase (yomhubgg not YomHubGG)
# 3. Trivy scan failure: Use local image tag (secure-app:sha not ghcr.io/...)
# 4. SBOM failure: Match image reference (same as Trivy)
# 5. No subscriptions: Add GitHub secrets (CLIENT_ID, TENANT_ID, SUBSCRIPTION_ID)

# Check artifacts
gh run view <run-id>
gh run download <run-id>  # Downloads all artifacts
```

### View Security Findings
```bash
# GitHub Security Tab
# https://github.com/YomHubGG/azure-security-mastery/security/code-scanning

# Download artifacts locally
gh run download <run-id> --name gitleaks-results.sarif
gh run download <run-id> --name npm-audit-results
gh run download <run-id> --name sbom-<sha>.spdx.json

# View SBOM
cat sbom-<sha>.spdx.json | jq '.packages[] | {name: .name, version: .versionInfo}'

# View npm audit
cat npm-audit-results/npm-audit.json | jq '.vulnerabilities'
```

### Cost Monitoring
```bash
# GitHub Actions usage
# https://github.com/settings/billing/summary

# Pipeline run time
gh run list --limit 10  # Check "ELAPSED" column

# Average: ~3 minutes per run
# Free tier: 2,000 minutes/month
# Capacity: 666 runs per month (more than enough)
```

### Real-World Learnings
```
✅ OIDC tokens expire after 1 hour (vs 2-year client secrets = 87,600 hours)
✅ No stored secrets = reduced attack surface
✅ GitHub requests tokens on-demand from Azure
✅ Federated credentials = modern authentication (AWS, GCP, Azure all support)
✅ SARIF format enables GitHub Security tab integration
✅ SBOM = Software Bill of Materials (supply chain security)
✅ Artifacts survive 90 days (compliance/auditing)
✅ Docker load: true required for local scanning with buildx
✅ Registry names must be lowercase in GHCR
✅ Pipeline debugging: iterative, systematic problem-solving
✅ Free tier = production-grade learning (2,000 minutes/month)
```

### Interview Talking Points
```
"I built a production-grade DevSecOps pipeline with zero stored secrets using 
OpenID Connect (OIDC). The pipeline includes 5 security layers:

1. Secret scanning with Gitleaks - catches leaked API keys
2. SAST with CodeQL - finds code vulnerabilities
3. Dependency scanning with npm audit - detects vulnerable packages  
4. Container scanning with Trivy - OS and library CVEs
5. SBOM generation with Syft - complete software bill of materials

The authentication uses OIDC tokens that expire after 1 hour instead of 
traditional 2-year client secrets, significantly reducing attack surface. 
Everything runs on GitHub Actions free tier, deploying to Azure Container 
Instances for under €0.01 per test. Results are visible in GitHub Security 
tab for centralized vulnerability management."
```

---

## Day 47: Infrastructure Security Scanning (Checkov)

**Date:** November 2, 2025  
**Tool:** Checkov 3.2.489  
**Focus:** Policy-as-Code and IaC Security Validation

### Checkov Installation
```bash
# Create virtual environment (recommended)
python3 -m venv ~/.venv/checkov
source ~/.venv/checkov/bin/activate

# Install Checkov
pip install checkov

# Verify installation
checkov --version
# Output: 3.2.489
```

### Basic Scanning
```bash
# Scan single Bicep file
checkov -f path/to/file.bicep --framework bicep

# Scan all Bicep files in directory
checkov -d . --framework bicep

# Compact output (less verbose)
checkov -d . --framework bicep --compact --quiet

# Scan specific frameworks
checkov -d . --framework terraform
checkov -d . --framework kubernetes
checkov -d . --framework dockerfile
```

### Output Formats
```bash
# JSON output (machine-readable)
checkov -d . --framework bicep --output json > results.json

# SARIF output (GitHub Security tab)
checkov -d . --framework bicep --output sarif > results.sarif

# Multiple outputs simultaneously
checkov -d . \
  --framework bicep \
  --output cli \
  --output json \
  --output sarif \
  --output-file-path results/
```

### Analysis Commands
```bash
# View summary statistics
checkov -d . --framework bicep --output json | jq '.summary'

# List all failed checks
checkov -d . --framework bicep --output json | \
  jq '.results.failed_checks[] | {check_id, check_name, file_path}'

# Count failures by check ID
checkov -d . --framework bicep --output json | \
  jq '.results.failed_checks | group_by(.check_id) | .[] | {check: .[0].check_id, count: length}'

# Top 10 most common failures
checkov -d . --framework bicep --output json | \
  jq '.results.failed_checks | group_by(.check_id) | sort_by(-length) | .[:10] | .[] | {check: .[0].check_id, count: length}'
```

### Filtering
```bash
# Run specific checks only
checkov -d . --check CKV_AZURE_35,CKV_AZURE_41

# Skip specific checks
checkov -d . --skip-check CKV_AZURE_206

# Skip pattern
checkov -d . --skip-check CKV_AZURE_*_206
```

### Inline Suppression (Bicep)
```bicep
// Suppress specific check with justification
#checkov:skip=CKV_AZURE_35:Dev environment - public access acceptable
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: 'devstoragetest'
  properties: {
    // ... configuration
  }
}
```

### CI/CD Integration
```bash
# GitHub Actions workflow step
- name: Run Checkov
  run: |
    pip install checkov
    checkov -d . \
      --framework bicep \
      --output sarif \
      --output-file-path results/

- name: Upload SARIF
  uses: github/codeql-action/upload-sarif@v3
  with:
    sarif_file: results/results_sarif.sarif
```

### Real Scan Results (Azure Security Journey)
```bash
# Full infrastructure scan
cd /home/yom/cybersecurity-journey/azure-security-mastery
source ~/.venv/checkov/bin/activate
checkov -d . --framework bicep --compact --quiet

# Output:
# bicep scan results:
# Passed checks: 68, Failed checks: 47, Skipped checks: 0
# Security Score: 59/100
```

### Common Azure Issues Found
```bash
# 1. Missing secret expiration (CKV_AZURE_41) - 12 occurrences
# Fix: Add attributes.exp to Key Vault secrets

# 2. Storage public access (CKV_AZURE_35) - 6 occurrences
# Fix: Add networkAcls: { defaultAction: 'Deny' }

# 3. HTTP traffic allowed (CKV_AZURE_14) - 3 occurrences
# Fix: Add httpsOnly: true to App Services

# 4. No geo-replication (CKV_AZURE_206) - 6 occurrences
# Fix: Change SKU from Standard_LRS to Standard_GRS

# 5. SSH from internet (CKV_AZURE_10) - 2 occurrences (CRITICAL)
# Fix: Restrict sourceAddressPrefix to specific IPs
```

### Remediation Workflow
```bash
# 1. Scan and identify issues
checkov -d . --framework bicep --compact > scan-results.txt

# 2. Prioritize by severity
# CRITICAL → HIGH → MEDIUM → LOW

# 3. Fix code (add network restrictions, expirations, etc.)

# 4. Re-scan to verify fixes
checkov -d . --framework bicep --compact

# 5. Compare before/after
# Before: 68 passed, 47 failed (59%)
# After: 115 passed, 0 failed (100%)
```

### Best Practices
```bash
# Start with warnings, not blocking (establish baseline)
checkov -d . --soft-fail

# After baseline, enforce in CI/CD
checkov -d . --compact || exit 1

# Scan only changed files in PR
git diff --name-only origin/main...HEAD | grep '.bicep$' | while read file; do
  checkov -f "$file" --framework bicep
done

# Use configuration file for consistent settings
cat > .checkov.yml << EOF
skip-check:
  - CKV_AZURE_206  # Dev environment - LRS acceptable
framework:
  - bicep
output: cli
compact: true
EOF

checkov -d . --config-file .checkov.yml
```

### Portfolio Soundbite
```
"I implemented Infrastructure-as-Code security scanning using Checkov, scanning 
9 Bicep templates across my Azure Security Journey. The scan identified 47 
security issues with a 59% security score, including critical findings like 
SSH access from the internet and storage accounts exposed to public access. 

I created comprehensive remediation guidance that would improve the security 
score from 59% to 100%, covering network restrictions, secret expiration 
policies, HTTPS enforcement, and geo-replication configuration. The scanning 
process takes under 10 seconds and integrates into GitHub Actions with SARIF 
output for the Security tab - true shift-left security in practice."
```

---

```

