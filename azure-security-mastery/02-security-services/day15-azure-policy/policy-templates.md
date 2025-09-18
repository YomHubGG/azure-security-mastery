# Azure Policy Templates for Security Learning

This directory contains policy definitions and assignments for implementing governance in the learning environment.

## Policy Categories

### 1. Security Policies
- HTTPS enforcement for storage accounts
- Key Vault security configurations
- Network security requirements

### 2. Governance Policies  
- Required resource tagging
- Allowed resource locations (GDPR compliance)
- Resource naming conventions

### 3. Cost Management Policies
- VM SKU restrictions
- Resource lifecycle management
- Budget enforcement

## Usage

Run the setup script to apply all governance policies:

```bash
chmod +x setup-governance.sh
./setup-governance.sh
```

## Standard Tags Applied

```json
{
  "Environment": "Learning",
  "Project": "Azure-Security", 
  "CostCenter": "Education",
  "Owner": "Student",
  "CreatedDate": "YYYY-MM-DD"
}
```

Service-specific tags are also applied based on resource type.