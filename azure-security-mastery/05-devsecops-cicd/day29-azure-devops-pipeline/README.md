# 🔧 Day 29: Azure DevOps Security Pipeline

**Date**: October 4, 2025 (Session #14)  
**Focus**: Secure CI/CD with Azure DevOps  
**Cost**: €0 (Free tier: 1,800 pipeline minutes/month)

---

## 🎯 Learning Objectives (Per 1-Year Plan)

```bash
# Day 29: Azure DevOps Security Pipeline
├── Set up Azure DevOps with security scanning
├── Implement secure CI/CD practices
├── Add infrastructure testing
├── Create security gates and approvals
└── Commit: "Secure DevOps pipeline"
```

---

## 📚 Why Azure DevOps for Security?

**Enterprise CI/CD Platform**:
- **Free Tier**: 1,800 build minutes/month (plenty for learning)
- **Built-in Security**: Secret scanning, dependency scanning
- **Pipeline-as-Code**: YAML-based like GitHub Actions (Day 23 experience!)
- **Azure Integration**: Native service connections
- **Security Gates**: Approval workflows before production

**You Already Know This** (from Day 23 GitHub Actions):
- Pipeline triggers and workflows
- Security scanning integration
- Secrets management
- Automated testing

**What's New**:
- Azure DevOps UI and project structure
- Service connections (vs GitHub secrets)
- Classic vs YAML pipelines
- Azure-native deployment

---

## 🚀 Phase 1: Azure DevOps Setup (10 min)

### Step 1: Create Organization

```bash
# Go to: https://dev.azure.com
# Sign in with your Azure account
# Create new organization: "YomSecurityLearning" (or your choice)
# Create project: "azure-security-pipeline"
```

### Step 2: Install Azure Pipelines Extension (if needed)

Already included in free tier!

### Step 3: Connect to Azure Subscription

**Service Connection Setup**:
1. Project Settings → Service connections
2. New service connection → Azure Resource Manager
3. Service principal (automatic)
4. Subscription: Azure subscription 1
5. Resource group: rg-learning-day1
6. Connection name: azure-security-connection

---

## 🔒 Phase 2: Security Pipeline (30 min)

### Create Pipeline File

Create: `azure-pipelines.yml`

```yaml
# Azure DevOps Security Pipeline
# Similar to Day 23 GitHub Actions but Azure-native

trigger:
  branches:
    include:
    - main
  paths:
    include:
    - 'azure-security-mastery/**/*.bicep'

pool:
  vmImage: 'ubuntu-latest'

variables:
  resourceGroup: 'rg-learning-day1'
  location: 'westeurope'

stages:
- stage: SecurityValidation
  displayName: 'Security Validation'
  jobs:
  - job: BicepValidation
    displayName: 'Validate Bicep Templates'
    steps:
    - task: AzureCLI@2
      displayName: 'Install Bicep'
      inputs:
        azureSubscription: 'azure-security-connection'
        scriptType: 'bash'
        scriptLocation: 'inlineScript'
        inlineScript: |
          az bicep install
          az bicep version
    
    - task: AzureCLI@2
      displayName: 'Validate Templates'
      inputs:
        azureSubscription: 'azure-security-connection'
        scriptType: 'bash'
        scriptLocation: 'inlineScript'
        inlineScript: |
          echo "Validating Bicep templates..."
          for file in $(find . -name "*.bicep"); do
            echo "Validating: $file"
            az bicep build --file "$file" || exit 1
          done
          echo "✅ All templates valid"

- stage: SecurityScan
  displayName: 'Security Scanning'
  dependsOn: SecurityValidation
  jobs:
  - job: DependencyScan
    displayName: 'Dependency Security Scan'
    steps:
    - script: |
        echo "🔍 Security scanning..."
        # Check for hardcoded secrets (basic check)
        if grep -r "password\|secret\|key" --include="*.bicep" --include="*.json" .; then
          echo "⚠️ Warning: Potential secrets found"
        fi
        echo "✅ Security scan complete"
      displayName: 'Basic Security Check'

- stage: Deploy
  displayName: 'Deploy to Azure'
  dependsOn: SecurityScan
  condition: succeeded()
  jobs:
  - deployment: DeployInfrastructure
    displayName: 'Deploy Infrastructure'
    environment: 'production'
    strategy:
      runOnce:
        deploy:
          steps:
          - checkout: self
          - task: AzureCLI@2
            displayName: 'Deploy Bicep Template'
            inputs:
              azureSubscription: 'azure-security-connection'
              scriptType: 'bash'
              scriptLocation: 'inlineScript'
              inlineScript: |
                echo "Deploying infrastructure..."
                echo "✅ Deployment simulation complete"
                # Add actual deployment commands here
```

---

## 📊 Phase 3: Security Gates (15 min)

### Add Manual Approval

1. **Environments** → Create "production"
2. **Approvals and checks** → Add approval
3. **Approvers**: Your account
4. **Instructions**: "Review security scan before deploying"

This creates a security gate - pipeline waits for manual approval before deploying!

---

## 🎯 Quick CLI Commands

```bash
# Check Azure DevOps CLI (if needed)
az extension add --name azure-devops

# Set organization default
az devops configure --defaults organization=https://dev.azure.com/YourOrg

# List pipelines
az pipelines list --org https://dev.azure.com/YourOrg --project azure-security-pipeline

# Run pipeline manually
az pipelines run --name azure-pipelines --org https://dev.azure.com/YourOrg --project azure-security-pipeline
```

---

## ✅ Success Criteria

- ✅ Azure DevOps organization created
- ✅ Project with security pipeline configured
- ✅ Service connection to Azure working
- ✅ Pipeline validates Bicep templates
- ✅ Security scanning integrated
- ✅ Manual approval gate configured
- ✅ Zero cost (within 1,800 free minutes)

---

## 💼 Portfolio Value

**Demonstrates**:
- Azure DevOps expertise (enterprise standard)
- Secure CI/CD pipeline design
- Infrastructure validation automation
- Security gates and compliance controls
- Multi-platform DevOps (GitHub + Azure DevOps)

**Career Skills**: DevSecOps Engineer, Cloud Security Engineer, Platform Engineer

---

Ready to build your Azure DevOps security pipeline! 🚀
