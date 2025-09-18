# Day 17: Key Vault & Managed Identity Demo

Today we'll implement enterprise-grade Key Vault management with Managed Identity.

## Step 1: Create some test secrets

Since we have issues with the Azure CLI Key Vault commands, let's use ARM/Bicep to manage our secrets:

```bash
# Create a deployment with test secrets
az deployment group create \
  --resource-group rg-learning-day1 \
  --template-file secrets-setup.bicep \
  --parameters keyVaultName=kvlearning4uybw3c2lbkwm
```

## Step 2: Set up Managed Identity Integration

We'll create a simple App Service with Managed Identity that can access our Key Vault securely.

## Step 3: Test Key Rotation Policies

We'll set up automated key rotation and monitoring.

Let's start! 🚀