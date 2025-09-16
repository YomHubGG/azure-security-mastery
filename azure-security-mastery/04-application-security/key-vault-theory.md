# 🔐 Azure Key Vault & Application Security - Complete Theory Guide

## 🎯 What is Azure Key Vault?

Azure Key Vault is your **centralized digital safe** in the cloud. Think of it as a highly secure vault where you store:
- 🔑 **Secrets**: Passwords, API keys, connection strings
- 🔐 **Keys**: Encryption keys for protecting data
- 📜 **Certificates**: SSL certificates for secure websites

## 🏗️ The Problem Key Vault Solves

### ❌ The Old Way (Dangerous):
```javascript
// app.js - DON'T DO THIS!
const config = {
    dbPassword: "MySuperSecretPassword123!",
    apiKey: "sk-1234567890abcdef",
    jwtSecret: "myapp-secret-key"
};

// This is bad because:
// 1. Secrets are in your code (anyone can see them)
// 2. Hard to change without redeploying
// 3. Accidentally committed to Git
// 4. Different secrets for dev/prod is painful
```

### ✅ The Key Vault Way (Secure):
```javascript
// app.js - SECURE APPROACH
const { SecretClient } = require("@azure/keyvault-secrets");

async function getConfig() {
    const keyVaultUrl = "https://myapp-vault.vault.azure.net/";
    const client = new SecretClient(keyVaultUrl, credential);
    
    return {
        dbPassword: await client.getSecret("database-password"),
        apiKey: await client.getSecret("openai-api-key"),
        jwtSecret: await client.getSecret("jwt-signing-key")
    };
}

// Benefits:
// 1. No secrets in code
// 2. Centralized management
// 3. Audit trail of who accessed what
// 4. Easy rotation and updates
```

## 🔒 Three Types of Key Vault Objects

### 1. 🔑 Secrets (Most Common)
**What**: Sensitive text values  
**Examples**: 
- Database connection strings
- API keys (OpenAI, Stripe, etc.)
- Passwords
- JWT signing secrets

```bash
# Store a secret
az keyvault secret set --vault-name MyVault --name "db-password" --value "SuperSecretPass123!"

# Retrieve a secret
az keyvault secret show --vault-name MyVault --name "db-password" --query "value" -o tsv
```

### 2. 🔐 Keys (For Encryption)
**What**: Cryptographic keys for encryption/decryption  
**Examples**:
- Encrypting sensitive data in database
- Signing tokens
- File encryption

```bash
# Create an encryption key
az keyvault key create --vault-name MyVault --name "data-encryption-key" --kty RSA --size 2048

# Use for encryption (programmatically)
const encrypted = await cryptoClient.encrypt("RSA-OAEP", Buffer.from("sensitive data"));
```

### 3. 📜 Certificates (For HTTPS)
**What**: SSL/TLS certificates for secure connections  
**Examples**:
- HTTPS certificates for websites
- Client authentication certificates
- Code signing certificates

```bash
# Import a certificate
az keyvault certificate import --vault-name MyVault --name "app-ssl-cert" --file cert.pfx
```

## 🎯 Real-World Scenarios for 42 Students

### Scenario 1: Chat Application (ft_transcendence)
```
Your final project needs:
├── 🗄️ PostgreSQL database → password in Key Vault
├── 🔐 JWT tokens → signing key in Key Vault  
├── 🎮 42 OAuth → client secret in Key Vault
├── 📧 Email service → API key in Key Vault
└── 📱 Push notifications → service keys in Key Vault
```

### Scenario 2: Microservices Project
```
Service A (Users) ←→ Service B (Chat) ←→ Service C (Game)
     ↓                    ↓                    ↓
  Key Vault          Key Vault           Key Vault
  - DB password      - API key          - Game configs
  - JWT secret       - Service auth      - External APIs
```

### Scenario 3: Web Portfolio with Backend
```
Frontend (React) → Backend (Node.js) → Database (PostgreSQL)
                        ↓
                   Key Vault:
                   - Database URL
                   - Session secret
                   - API keys
```

## 🛡️ Security Benefits

### 1. **Centralized Management**
```
Before: Secrets scattered everywhere
├── .env files (different per developer)
├── Docker compose files
├── Kubernetes secrets
└── Hardcoded in applications

After: One source of truth
└── Azure Key Vault
    ├── All secrets in one place
    ├── Consistent across environments
    ├── Easy to update globally
    └── Full audit trail
```

### 2. **Access Control**
```bash
# Only specific people/apps can access secrets
az role assignment create \
  --role "Key Vault Secrets User" \
  --assignee "developer@42.fr" \
  --scope "/subscriptions/.../vaults/MyVault"

# Your app gets its own identity (no passwords!)
az identity create --name "my-app-identity"
```

### 3. **Audit Trail**
```
Who accessed what secret when:
2024-09-16 10:30 - alice@42.fr accessed "database-password"
2024-09-16 10:31 - my-app-identity accessed "jwt-secret"
2024-09-16 10:32 - bob@42.fr listed all secrets
```

## 🔄 How Applications Connect to Key Vault

### Method 1: Service Principal (Username/Password for Apps)
```javascript
const credential = new ClientSecretCredential(
    "tenant-id",
    "client-id", 
    "client-secret"  // This is still a secret, but only one!
);
```

### Method 2: Managed Identity (Passwordless! 🎉)
```javascript
// When running in Azure (Container Instances, App Service, etc.)
const credential = new DefaultAzureCredential();
// Azure automatically handles authentication - no secrets needed!
```

### Method 3: Development (Your Local Machine)
```bash
# Login once
az login

# Your app uses your credentials
const credential = new DefaultAzureCredential();
```

## 🔄 Secret Lifecycle Management

### 1. **Creation**
```bash
az keyvault secret set --vault-name MyVault --name "new-api-key" --value "abc123"
```

### 2. **Versioning** (Automatic)
```bash
# Update the same secret name
az keyvault secret set --vault-name MyVault --name "new-api-key" --value "xyz789"

# Old version still exists
az keyvault secret show --vault-name MyVault --name "new-api-key" --version "previous-version-id"
```

### 3. **Rotation**
```bash
# Set expiration date
az keyvault secret set --vault-name MyVault --name "temp-key" --value "abc123" --expires "2024-12-31"

# Get notification when it's about to expire
# Update your application to use new value
```

### 4. **Deletion**
```bash
# Soft delete (can be recovered)
az keyvault secret delete --vault-name MyVault --name "old-secret"

# Permanent deletion
az keyvault secret purge --vault-name MyVault --name "old-secret"
```

## 💰 Cost Structure (Great for Students!)

### Free Tier Benefits:
- ✅ **10,000 operations/month FREE**
- ✅ **Unlimited secrets storage**
- ✅ **All basic features included**

### What Counts as an "Operation":
- Getting a secret (1 operation)
- Setting a secret (1 operation)
- Listing secrets (1 operation)
- Deleting a secret (1 operation)

### Typical Usage for Learning:
```
Daily development: ~50 operations
Monthly total: ~1,500 operations
Cost: FREE (well under 10k limit)
```

## 🏗️ Integration Architecture Patterns

### Pattern 1: Simple Web App
```
User → Web App → Key Vault → Database
       ↓
    Gets DB password from Key Vault
    Connects to database
```

### Pattern 2: Containerized App
```
Container Instance
├── Your application code
├── Azure SDK for Key Vault
└── Managed Identity (automatic auth)
     ↓
Key Vault
├── database-connection-string
├── external-api-key
└── jwt-secret
```

### Pattern 3: Multi-Environment
```
Development Environment:
├── App → Dev Key Vault → Dev Database

Production Environment:
├── App → Prod Key Vault → Prod Database

Same code, different Key Vault URLs!
```

## 🛠️ Development Workflow

### 1. **Local Development**
```bash
# Login to Azure
az login

# Set your Key Vault URL
export KEYVAULT_URL="https://dev-vault.vault.azure.net/"

# Your app automatically uses your credentials
node app.js
```

### 2. **Testing Environment**
```bash
# Deploy to Azure Container Instances with Managed Identity
az container create \
  --name test-app \
  --image myapp:latest \
  --assign-identity \
  --environment-variables KEYVAULT_URL="https://test-vault.vault.azure.net/"
```

### 3. **Production Environment**
```bash
# Same code, different Key Vault
az container create \
  --name prod-app \
  --image myapp:latest \
  --assign-identity \
  --environment-variables KEYVAULT_URL="https://prod-vault.vault.azure.net/"
```

## 🎓 Learning Path Summary

Now that you understand the theory, here's what we'll build hands-on:

1. **🏗️ Create Key Vault** (using Bicep IaC)
2. **🔑 Store secrets** (database passwords, API keys)
3. **📝 Build sample app** (Node.js app that uses Key Vault)
4. **🐳 Containerize** (secure container with Key Vault integration)
5. **🚀 Deploy** (Container Instance with Managed Identity)
6. **🔍 Monitor** (see who accessed what secrets)

## ✅ Knowledge Check

Before we continue, make sure you understand:
- ❓ What's the difference between secrets, keys, and certificates?
- ❓ Why is Key Vault better than environment variables?
- ❓ How does Managed Identity eliminate passwords?
- ❓ What are the cost implications for student projects?

Ready to get hands-on? 🚀
