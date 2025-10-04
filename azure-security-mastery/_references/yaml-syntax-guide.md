# 📝 YAML Syntax Guide for Azure Pipelines

**Created**: October 4, 2025  
**Purpose**: Quick reference for Azure DevOps & GitHub Actions YAML syntax

---

## 🎯 YAML Basics

### What is YAML?
**YAML** = "YAML Ain't Markup Language"
- Human-readable data serialization format
- Used for configuration files (pipelines, Docker, Kubernetes)
- **Indentation matters** (like Python) - use spaces, NOT tabs!
- Case-sensitive

---

## 📐 Core YAML Syntax

### Comments
```yaml
# This is a comment
# Comments start with # and go to end of line
```

### Key-Value Pairs
```yaml
key: value
name: John
age: 30
location: Paris
```

### Nested Structures (Indentation)
```yaml
parent:
  child: value
  another_child: another_value
  
person:
  name: Yom
  location: France
  skills:
    - Azure
    - Security
    - DevOps
```

### Lists (Arrays)
```yaml
# Method 1: Dash notation
fruits:
  - apple
  - banana
  - orange

# Method 2: Inline
fruits: [apple, banana, orange]
```

### Multi-line Strings
```yaml
# Preserve line breaks with |
script: |
  echo "Line 1"
  echo "Line 2"
  echo "Line 3"

# Fold into single line with >
description: >
  This is a very long
  description that will
  be folded into one line
```

### Boolean Values
```yaml
enabled: true
disabled: false
# Also accepted: yes/no, on/off, True/False
```

### Numbers
```yaml
age: 30
price: 19.99
count: 0
```

---

## 🔧 Azure Pipelines YAML Structure

### Basic Pipeline Template
```yaml
# When to run the pipeline
trigger:
  branches:
    include:
    - main
    - develop
  paths:
    include:
    - 'src/**'
    exclude:
    - 'docs/**'

# What machine/OS to use
pool:
  vmImage: 'ubuntu-latest'  # Linux
  # vmImage: 'windows-latest'  # Windows
  # vmImage: 'macOS-latest'    # macOS

# What to execute
steps:
  - task: TaskName@Version
    displayName: 'Human-readable name'
    inputs:
      inputKey: value
  
  - script: echo "Hello World"
    displayName: 'Run bash command'
```

### Key Pipeline Components

#### 1. Triggers
```yaml
# Trigger on specific branches
trigger:
  branches:
    include:
    - main
    - release/*
    exclude:
    - experimental/*

# Trigger on specific paths
trigger:
  paths:
    include:
    - 'azure-security-mastery/**/*.bicep'

# Trigger on pull requests
pr:
  branches:
    include:
    - main

# Disable automatic triggers
trigger: none
```

#### 2. Pool (Build Agent)
```yaml
# Microsoft-hosted agents
pool:
  vmImage: 'ubuntu-latest'

# Available images:
# - ubuntu-latest
# - ubuntu-22.04
# - windows-latest
# - windows-2022
# - macOS-latest
# - macOS-12
```

#### 3. Variables
```yaml
variables:
  resourceGroup: 'rg-learning-day1'
  location: 'westeurope'
  buildConfiguration: 'Release'

# Using variables
steps:
  - script: echo "RG is $(resourceGroup)"
```

#### 4. Steps (Simple Pipeline)
```yaml
steps:
  # Built-in Azure task
  - task: AzureCLI@2
    displayName: 'Run Azure CLI'
    inputs:
      azureSubscription: 'connection-name'
      scriptType: 'bash'
      scriptLocation: 'inlineScript'
      inlineScript: |
        az --version
        az account show

  # Raw script
  - script: |
      echo "Running security check..."
      npm audit
    displayName: 'Security Audit'

  # Bash script
  - bash: |
      echo "Bash-specific script"
    displayName: 'Bash Task'

  # PowerShell script
  - powershell: |
      Write-Host "PowerShell script"
    displayName: 'PowerShell Task'
```

#### 5. Stages (Multi-stage Pipeline)
```yaml
stages:
- stage: Build
  displayName: 'Build Stage'
  jobs:
  - job: CompileCode
    steps:
    - script: echo "Building..."

- stage: Test
  displayName: 'Test Stage'
  dependsOn: Build  # Wait for Build to complete
  jobs:
  - job: RunTests
    steps:
    - script: echo "Testing..."

- stage: Deploy
  displayName: 'Deploy Stage'
  dependsOn: Test
  condition: succeeded()  # Only if Test succeeded
  jobs:
  - job: DeployToAzure
    steps:
    - script: echo "Deploying..."
```

#### 6. Jobs (Parallel Execution)
```yaml
jobs:
- job: Job1
  steps:
  - script: echo "Job 1"

- job: Job2
  steps:
  - script: echo "Job 2"
  
# Jobs run in parallel by default
```

#### 7. Conditions
```yaml
steps:
  - script: echo "Always runs"
  
  - script: echo "Only on main branch"
    condition: eq(variables['Build.SourceBranch'], 'refs/heads/main')
  
  - script: echo "Only if previous succeeded"
    condition: succeeded()
  
  - script: echo "Always run, even on failure"
    condition: always()
```

---

## 🐙 GitHub Actions YAML (Comparison)

### Basic Workflow
```yaml
name: CI Pipeline

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Run a command
      run: echo "Hello World"
    
    - name: Multi-line script
      run: |
        echo "Line 1"
        echo "Line 2"
```

### Azure Pipelines vs GitHub Actions

| Azure Pipelines | GitHub Actions | Purpose |
|----------------|----------------|---------|
| `trigger:` | `on:` | When to run |
| `pool:` | `runs-on:` | What machine |
| `steps:` | `steps:` | What to do |
| `task:` | `uses:` | Pre-built action |
| `script:` | `run:` | Command to execute |
| `displayName:` | `name:` | Human label |
| `stages:` | `jobs:` | Grouping |

---

## 💡 Best Practices

### Indentation
```yaml
# ✅ CORRECT - 2 spaces per level
parent:
  child:
    grandchild: value

# ❌ WRONG - tabs or inconsistent spaces
parent:
	child:  # Tab used!
    grandchild: value  # 4 spaces
```

### Quotes
```yaml
# Generally optional for simple strings
name: azure-pipeline

# Required for special characters
name: "pipeline: with-colon"

# Required for strings starting with special chars
value: "@variable"

# Multi-word values
description: This works fine without quotes
description: "But quotes are safer"
```

### Comments
```yaml
# ✅ GOOD - Explain why, not what
# Using ubuntu-latest for Docker compatibility
pool:
  vmImage: 'ubuntu-latest'

# ❌ BAD - Obvious comment
# This sets the VM image
pool:
  vmImage: 'ubuntu-latest'
```

---

## 🔍 Common YAML Errors

### 1. Indentation Error
```yaml
# ❌ WRONG
steps:
- script: echo "test"
  displayName: 'Test'
 condition: succeeded()  # Wrong indentation!

# ✅ CORRECT
steps:
- script: echo "test"
  displayName: 'Test'
  condition: succeeded()  # Aligned with displayName
```

### 2. List Item Error
```yaml
# ❌ WRONG
steps:
  task: AzureCLI@2  # Missing dash!

# ✅ CORRECT
steps:
  - task: AzureCLI@2  # Dash for list item
```

### 3. String with Special Characters
```yaml
# ❌ MIGHT BREAK
script: echo $PATH

# ✅ SAFER
script: 'echo $PATH'
script: |
  echo $PATH
```

---

## 🎯 Quick Reference Table

| Syntax | Meaning | Example |
|--------|---------|---------|
| `key: value` | Key-value pair | `name: pipeline` |
| `- item` | List item | `- main` |
| `parent:` | Nested structure | `trigger:` |
| `  child: value` | Indented child | `  branches:` |
| `\|` | Multi-line preserved | `script: \|` |
| `>` | Multi-line folded | `description: >` |
| `#` | Comment | `# This is a note` |
| `$(var)` | Variable reference | `$(resourceGroup)` |
| `@` | Task version | `AzureCLI@2` |

---

## 📚 Learning Resources

### Official Documentation
- **Azure Pipelines YAML**: https://learn.microsoft.com/en-us/azure/devops/pipelines/yaml-schema
- **GitHub Actions**: https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions
- **YAML Spec**: https://yaml.org/spec/

### Validation Tools
- **YAML Lint**: http://www.yamllint.com/
- **Azure DevOps YAML Validator**: Built into portal editor
- **VS Code Extension**: YAML by Red Hat

### Quick Tips
1. **Use VS Code** with YAML extension for syntax highlighting
2. **Validate often** - most portals have built-in validators
3. **Start simple** - add complexity gradually
4. **Copy working examples** - then customize
5. **Mind your spaces** - 2 spaces per indentation level

---

**Last Updated**: October 4, 2025  
**Used In**: Day 23 (GitHub Actions), Day 29 (Azure DevOps)  
**Portfolio Value**: YAML is universal in DevOps (Docker, Kubernetes, CI/CD)
