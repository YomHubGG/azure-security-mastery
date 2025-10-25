# 🔧 Integration Cookbook - WHERE DO I ACTUALLY DO THIS?

**Last Updated:** October 25, 2025  
**Purpose:** Bridge theory → practice gap  
**Your Frustration:** *"If you ask me I won't even know which file to open nor what to write and where"*

---

## 🎯 The Problem This Solves

**Theory says:** "Integrate Key Vault with AKS using CSI Secrets Store Driver"

**You think:** 😰 *"Cool... but WHERE? What file? What command? Azure Portal? Terminal? Bicep? YAML? Help!"*

**This guide:** Shows you EXACTLY where, what file, what format, what tool.

---

## 📋 Quick Reference Table

| Task | Tool | Where | File Type | Example |
|------|------|-------|-----------|---------|
| Create Azure resources | Azure CLI / Bicep | Terminal | `.bicep` or command | `az vm create ...` |
| Configure K8s resources | kubectl | Terminal | `.yaml` | `kubectl apply -f pod.yaml` |
| Build container | Docker | Terminal | `Dockerfile` | `docker build -t myapp .` |
| Deploy to ACI | Azure CLI | Terminal | Command | `az container create ...` |
| Set environment variables | Azure Portal / CLI | Portal or Terminal | N/A | `az webapp config appsettings set` |
| Store secrets | Azure Key Vault | Portal or Terminal | N/A | `az keyvault secret set` |
| CI/CD pipeline | GitHub Actions | `.github/workflows/` | `.yml` | `deploy.yml` |
| Local development | VS Code / Terminal | Your machine | Various | `npm install`, `npm start` |
| Configure app | Code editor | Project root | `.env`, `config.js` | `PORT=3000` |
| Database queries | MongoDB Compass / Code | App or GUI | `.js` | `User.find()` |

---

## 🗂️ File Structure & Where Things Go

### Typical Azure + MERN Project Structure

```
my-project/
├── .github/
│   └── workflows/
│       ├── deploy-frontend.yml        ← GitHub Actions (CI/CD)
│       └── deploy-backend.yml
│
├── infrastructure/                    ← Azure Bicep templates
│   ├── main.bicep
│   ├── storage.bicep
│   ├── keyvault.bicep
│   └── parameters.json
│
├── kubernetes/                        ← Kubernetes YAML manifests
│   ├── deployment.yaml
│   ├── service.yaml
│   ├── ingress.yaml
│   ├── configmap.yaml
│   └── secret.yaml
│
├── backend/                           ← Node.js + Express backend
│   ├── src/
│   │   ├── models/
│   │   │   └── User.js                ← MongoDB schemas
│   │   ├── routes/
│   │   │   └── users.js               ← API endpoints
│   │   ├── middleware/
│   │   │   └── auth.js                ← Authentication logic
│   │   └── index.js                   ← Main entry point
│   ├── .env                           ← Local environment variables (NEVER commit!)
│   ├── .env.example                   ← Template (safe to commit)
│   ├── Dockerfile                     ← Container build instructions
│   ├── package.json                   ← Dependencies
│   └── .dockerignore
│
├── frontend/                          ← React frontend
│   ├── src/
│   │   ├── components/
│   │   │   └── UserList.jsx           ← React components
│   │   ├── pages/
│   │   │   └── Home.jsx               ← React pages
│   │   ├── App.jsx                    ← Main React component
│   │   └── index.js                   ← Entry point
│   ├── public/
│   │   └── index.html                 ← HTML template
│   ├── Dockerfile                     ← Container build (if containerizing React)
│   ├── package.json
│   └── .env                           ← React environment variables
│
├── .gitignore                         ← Files to exclude from Git
├── README.md                          ← Project documentation
└── docker-compose.yml                 ← Local development (optional)
```

---

## 🔍 Scenario-Based Guide

### Scenario 1: "I Want to Deploy My Day 35 Container to Azure"

**Current State:** You have `secure-app` container working locally  
**Goal:** Deploy to Azure Container Instances  
**Tools Needed:** Terminal (Azure CLI), Docker

**Step-by-Step:**

1️⃣ **Build your container (Terminal, in project directory):**
```bash
# Location: Your project root (where Dockerfile is)
docker build -t secure-app:1.0 .
```

2️⃣ **Tag for Azure Container Registry (Terminal):**
```bash
docker tag secure-app:1.0 myregistry.azurecr.io/secure-app:1.0
```

3️⃣ **Push to registry (Terminal):**
```bash
docker push myregistry.azurecr.io/secure-app:1.0
```

4️⃣ **Deploy to ACI (Terminal):**
```bash
az container create \
  --resource-group rg-aci-test \
  --name secure-app-container \
  --image myregistry.azurecr.io/secure-app:1.0 \
  --cpu 0.5 \
  --memory 0.5 \
  --ports 3000 \
  --dns-name-label yom-secure-app \
  --registry-username myregistry \
  --registry-password "your-password"
```

**WHERE:**
- Build: Terminal (project directory)
- Tag/Push: Terminal (any directory)
- Deploy: Terminal (any directory)
- **NO FILES TO EDIT** (all commands)

---

### Scenario 2: "I Want to Store Secrets in Key Vault Instead of Environment Variables"

**Current State:** Your `.env` file has secrets (bad!)  
**Goal:** Move secrets to Azure Key Vault  
**Tools Needed:** Azure Portal OR Terminal

**Option A: Azure Portal (GUI)**

1️⃣ **Create Key Vault:**
- Navigate to: Azure Portal → Search "Key Vault"
- Click: "+ Create"
- Fill form:
  - Resource Group: `rg-keyvault`
  - Name: `kv-yom-secrets`
  - Region: `West Europe`
- Click: "Review + Create" → "Create"

2️⃣ **Add Secret:**
- Navigate to: Your Key Vault → "Secrets" (left menu)
- Click: "+ Generate/Import"
- Fill form:
  - Name: `database-url`
  - Value: `mongodb://username:password@host:27017/db`
- Click: "Create"

3️⃣ **Grant App Access:**
- Navigate to: Your Key Vault → "Access policies" (left menu)
- Click: "+ Add Access Policy"
- Select:
  - Secret permissions: "Get", "List"
  - Select principal: Your App Service or Managed Identity
- Click: "Add" → "Save"

**Option B: Azure CLI (Terminal)**

```bash
# 1. Create Key Vault
az keyvault create \
  --name kv-yom-secrets \
  --resource-group rg-keyvault \
  --location westeurope

# 2. Add Secret
az keyvault secret set \
  --vault-name kv-yom-secrets \
  --name database-url \
  --value "mongodb://username:password@host:27017/db"

# 3. Grant App Access (if using App Service)
az webapp identity assign \
  --name my-app \
  --resource-group rg-app

# Get the identity's principal ID (output from above command)
PRINCIPAL_ID="copy-from-above-output"

az keyvault set-policy \
  --name kv-yom-secrets \
  --object-id $PRINCIPAL_ID \
  --secret-permissions get list
```

**WHERE:**
- Portal: Azure Portal → Key Vault section
- CLI: Terminal (any directory)
- **NO LOCAL FILES** (all in Azure)

---

### Scenario 3: "I Want to Connect Key Vault to My App Service"

**Current State:** Secret in Key Vault, app can't access  
**Goal:** App reads secret from Key Vault  
**Tools Needed:** Code Editor, Terminal

**Step 1: Enable Managed Identity (Terminal):**
```bash
az webapp identity assign \
  --name my-app \
  --resource-group rg-app
```

**Step 2: Grant Key Vault Access (Terminal):**
```bash
PRINCIPAL_ID=$(az webapp identity show \
  --name my-app \
  --resource-group rg-app \
  --query principalId \
  --output tsv)

az keyvault set-policy \
  --name kv-yom-secrets \
  --object-id $PRINCIPAL_ID \
  --secret-permissions get list
```

**Step 3: Configure App Settings to Reference Key Vault (Terminal):**
```bash
az webapp config appsettings set \
  --name my-app \
  --resource-group rg-app \
  --settings DATABASE_URL="@Microsoft.KeyVault(SecretUri=https://kv-yom-secrets.vault.azure.net/secrets/database-url/)"
```

**Step 4: Access in Your Code (Code Editor: `backend/src/index.js`):**
```javascript
// Your app code (backend/src/index.js)
const dbUrl = process.env.DATABASE_URL; // Automatically fetched from Key Vault!

mongoose.connect(dbUrl, {
  useNewUrlParser: true,
  useUnifiedTopology: true
});
```

**WHERE:**
- Commands: Terminal (any directory)
- Code: `backend/src/index.js` (or wherever you connect to DB)
- **NO MANUAL KEY VAULT SDK** (App Service handles it automatically!)

---

### Scenario 4: "I Want to Deploy to Kubernetes (AKS)"

**Current State:** Your app works locally  
**Goal:** Deploy to AKS with Kubernetes manifests  
**Tools Needed:** Code Editor (for YAML), Terminal (for kubectl)

**Step 1: Create Kubernetes Manifests (Code Editor):**

**File: `kubernetes/deployment.yaml`**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: secure-app
  namespace: default
spec:
  replicas: 3
  selector:
    matchLabels:
      app: secure-app
  template:
    metadata:
      labels:
        app: secure-app
    spec:
      containers:
      - name: secure-app
        image: myregistry.azurecr.io/secure-app:1.0
        ports:
        - containerPort: 3000
        env:
        - name: PORT
          value: "3000"
        - name: DATABASE_URL
          valueFrom:
            secretKeyRef:
              name: app-secrets
              key: database-url
        resources:
          requests:
            memory: "128Mi"
            cpu: "250m"
          limits:
            memory: "256Mi"
            cpu: "500m"
        securityContext:
          runAsUser: 1001
          runAsNonRoot: true
          allowPrivilegeEscalation: false
```

**File: `kubernetes/service.yaml`**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: secure-app-service
spec:
  type: LoadBalancer
  selector:
    app: secure-app
  ports:
  - protocol: TCP
    port: 80
    targetPort: 3000
```

**File: `kubernetes/secret.yaml` (for local testing only!)**
```yaml
apiVersion: v1
kind: Secret
metadata:
  name: app-secrets
type: Opaque
stringData:
  database-url: "mongodb://localhost:27017/myapp"
```

**Step 2: Apply to AKS (Terminal):**
```bash
# Connect to your AKS cluster
az aks get-credentials \
  --resource-group rg-aks \
  --name aks-yom-cluster

# Apply manifests
kubectl apply -f kubernetes/secret.yaml
kubectl apply -f kubernetes/deployment.yaml
kubectl apply -f kubernetes/service.yaml

# Check status
kubectl get pods
kubectl get services
```

**WHERE:**
- YAML files: `kubernetes/` directory in your project
- Apply commands: Terminal (in project root or kubernetes/ directory)
- **FILES ARE LOCAL, KUBECTL SENDS TO AZURE**

---

### Scenario 5: "I Want to Integrate Key Vault with AKS (CSI Driver)"

**Current State:** AKS deployed, Key Vault has secrets  
**Goal:** Pods read secrets from Key Vault (not YAML files)  
**Tools Needed:** Terminal (Azure CLI + kubectl), Code Editor (YAML)

**Step 1: Enable CSI Driver on AKS (Terminal):**
```bash
az aks enable-addons \
  --addons azure-keyvault-secrets-provider \
  --name aks-yom-cluster \
  --resource-group rg-aks
```

**Step 2: Create Managed Identity (Terminal):**
```bash
az identity create \
  --name aks-keyvault-identity \
  --resource-group rg-aks

# Get client ID
CLIENT_ID=$(az identity show \
  --name aks-keyvault-identity \
  --resource-group rg-aks \
  --query clientId \
  --output tsv)

# Get identity resource ID
IDENTITY_ID=$(az identity show \
  --name aks-keyvault-identity \
  --resource-group rg-aks \
  --query id \
  --output tsv)
```

**Step 3: Grant Key Vault Access (Terminal):**
```bash
az keyvault set-policy \
  --name kv-yom-secrets \
  --object-id $CLIENT_ID \
  --secret-permissions get list
```

**Step 4: Associate Identity with AKS (Terminal):**
```bash
az aks pod-identity add \
  --resource-group rg-aks \
  --cluster-name aks-yom-cluster \
  --namespace default \
  --name aks-keyvault-identity \
  --identity-resource-id $IDENTITY_ID
```

**Step 5: Create SecretProviderClass (Code Editor: `kubernetes/secretprovider.yaml`):**
```yaml
apiVersion: secrets-store.csi.x-k8s.io/v1
kind: SecretProviderClass
metadata:
  name: azure-keyvault-secrets
  namespace: default
spec:
  provider: azure
  parameters:
    usePodIdentity: "true"
    keyvaultName: "kv-yom-secrets"
    objects: |
      array:
        - |
          objectName: database-url
          objectType: secret
        - |
          objectName: api-key
          objectType: secret
    tenantId: "your-tenant-id"
```

**Step 6: Update Deployment to Use CSI (Code Editor: `kubernetes/deployment.yaml`):**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: secure-app
spec:
  template:
    spec:
      containers:
      - name: secure-app
        image: myregistry.azurecr.io/secure-app:1.0
        volumeMounts:
        - name: secrets-store
          mountPath: "/mnt/secrets"
          readOnly: true
        env:
        - name: DATABASE_URL
          value: "/mnt/secrets/database-url"
      volumes:
      - name: secrets-store
        csi:
          driver: secrets-store.csi.k8s.io
          readOnly: true
          volumeAttributes:
            secretProviderClass: "azure-keyvault-secrets"
```

**Step 7: Apply Changes (Terminal):**
```bash
kubectl apply -f kubernetes/secretprovider.yaml
kubectl apply -f kubernetes/deployment.yaml
```

**Step 8: Verify (Terminal):**
```bash
# Check pod logs
kubectl logs -l app=secure-app

# Exec into pod and check mounted secrets
kubectl exec -it <pod-name> -- ls /mnt/secrets
kubectl exec -it <pod-name> -- cat /mnt/secrets/database-url
```

**WHERE:**
- Enable addon: Terminal (Azure CLI)
- Create identity: Terminal (Azure CLI)
- YAML files: `kubernetes/` directory
- Apply manifests: Terminal (kubectl)
- Verify: Terminal (kubectl)

---

### Scenario 6: "I Want to Set Up CI/CD Pipeline"

**Current State:** You push code to GitHub manually  
**Goal:** Auto-deploy to Azure on every push  
**Tools Needed:** Code Editor (for workflow file), GitHub

**Step 1: Create Workflow File (Code Editor: `.github/workflows/deploy.yml`):**
```yaml
name: Deploy to Azure

on:
  push:
    branches: [ main ]

env:
  AZURE_WEBAPP_NAME: my-app
  NODE_VERSION: '18.x'

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v3
    
    - name: Setup Node.js
      uses: actions/setup-node@v3
      with:
        node-version: ${{ env.NODE_VERSION }}
    
    - name: Install dependencies
      run: npm ci
      working-directory: ./backend
    
    - name: Run tests
      run: npm test
      working-directory: ./backend
    
    - name: Build app
      run: npm run build
      working-directory: ./backend
    
    - name: Deploy to Azure Web App
      uses: azure/webapps-deploy@v2
      with:
        app-name: ${{ env.AZURE_WEBAPP_NAME }}
        publish-profile: ${{ secrets.AZURE_WEBAPP_PUBLISH_PROFILE }}
        package: ./backend
```

**Step 2: Get Publish Profile (Azure Portal):**
- Navigate to: Azure Portal → Your App Service
- Click: "Get publish profile" (top toolbar)
- Save downloaded file

**Step 3: Add Secret to GitHub (GitHub Website):**
- Navigate to: Your GitHub repo → Settings → Secrets and variables → Actions
- Click: "New repository secret"
- Name: `AZURE_WEBAPP_PUBLISH_PROFILE`
- Value: Paste contents of downloaded publish profile
- Click: "Add secret"

**Step 4: Commit Workflow File (Terminal):**
```bash
git add .github/workflows/deploy.yml
git commit -m "Add CI/CD pipeline"
git push origin main
```

**Step 5: Watch It Run (GitHub Website):**
- Navigate to: Your GitHub repo → Actions tab
- Click on your workflow run
- Watch logs in real-time

**WHERE:**
- Workflow file: `.github/workflows/deploy.yml` in your repo
- Publish profile: Azure Portal → App Service
- GitHub secret: GitHub repo → Settings → Secrets
- Commit: Terminal (in project root)
- Monitor: GitHub Actions tab

---

## 🧩 Common Integration Patterns

### Pattern 1: Local Development → Docker → Azure

```
1. Write code (VS Code, local files)
   ├─ backend/src/index.js
   └─ frontend/src/App.jsx

2. Test locally (Terminal, project directory)
   ├─ npm install
   ├─ npm start
   └─ Visit http://localhost:3000

3. Containerize (Terminal, project directory)
   ├─ docker build -t myapp .
   └─ docker run -p 3000:3000 myapp

4. Push to registry (Terminal, any directory)
   ├─ docker tag myapp myregistry.azurecr.io/myapp:1.0
   └─ docker push myregistry.azurecr.io/myapp:1.0

5. Deploy to Azure (Terminal, any directory)
   └─ az container create --image myregistry.azurecr.io/myapp:1.0 ...
```

---

### Pattern 2: GitHub → GitHub Actions → Azure

```
1. Write code (VS Code, local files)
   └─ backend/src/index.js

2. Commit to Git (Terminal, project root)
   ├─ git add .
   ├─ git commit -m "Add feature"
   └─ git push origin main

3. GitHub Actions triggers (Automatic, no action needed)
   └─ Defined in .github/workflows/deploy.yml

4. Pipeline runs (GitHub website, Actions tab)
   ├─ Checkout code
   ├─ Install dependencies
   ├─ Run tests
   ├─ Build Docker image
   └─ Deploy to Azure

5. App updated (Azure Portal or app URL)
   └─ Visit https://myapp.azurewebsites.net
```

---

### Pattern 3: Infrastructure as Code (Bicep)

```
1. Write Bicep (VS Code, infrastructure/ directory)
   └─ infrastructure/main.bicep

2. Validate (Terminal, infrastructure/ directory)
   └─ az bicep build --file main.bicep

3. Deploy (Terminal, any directory)
   └─ az deployment group create \
        --resource-group rg-myapp \
        --template-file infrastructure/main.bicep \
        --parameters infrastructure/parameters.json

4. Verify (Azure Portal)
   └─ Check Resource Group for created resources
```

---

## 🆘 Troubleshooting: "WHERE DO I FIX THIS?"

### Error: "Cannot find module 'express'"

**Fix Location:** Terminal (backend/ directory)
```bash
cd backend
npm install express
```

---

### Error: "Container port 3000 not accessible"

**Fix Location:** Azure Portal OR Terminal

**Option 1: Azure Portal**
- Navigate to: Your Container Instance → "Containers" → "Ports"
- Add port: 3000
- Update and restart

**Option 2: Azure CLI (Terminal)**
```bash
az container create \
  --ports 3000 \  # ← Add this line
  ...
```

---

### Error: "Secret not found in Key Vault"

**Fix Location:** Azure Portal OR Terminal

**Option 1: Portal**
- Navigate to: Key Vault → Secrets
- Click: "+ Generate/Import"
- Add your secret

**Option 2: CLI (Terminal)**
```bash
az keyvault secret set \
  --vault-name kv-yom-secrets \
  --name database-url \
  --value "your-connection-string"
```

---

### Error: "kubectl: command not found"

**Fix Location:** Terminal (any directory)
```bash
# Install kubectl
az aks install-cli
```

---

### Error: "Pod stuck in 'ImagePullBackOff'"

**Fix Location:** Terminal (check logs), then fix in YAML file

**Step 1: Diagnose (Terminal)**
```bash
kubectl describe pod <pod-name>
# Look for error message
```

**Step 2: Fix Image Name (Code Editor: kubernetes/deployment.yaml)**
```yaml
# Change this:
image: myregistry.azurecr.io/wrong-name:1.0

# To this:
image: myregistry.azurecr.io/correct-name:1.0
```

**Step 3: Reapply (Terminal)**
```bash
kubectl apply -f kubernetes/deployment.yaml
```

---

## 🎓 Mental Model: "Where Do I Do This?"

### Azure Portal (GUI):
- Creating resources initially (exploring)
- Checking status/monitoring
- One-off configuration changes
- Downloading publish profiles
- **NOT for automation** (use CLI instead)

### Terminal (Azure CLI):
- Creating resources (repeatable)
- Deploying infrastructure (Bicep)
- Automation scripts
- CI/CD pipelines
- **Preferred for production**

### Terminal (kubectl):
- Managing Kubernetes resources
- Deploying to AKS
- Debugging pods
- Checking logs
- **Only for Kubernetes**

### Terminal (Docker):
- Building container images
- Testing containers locally
- Pushing to registries
- **Local development + CI/CD**

### Code Editor (YAML files):
- Kubernetes manifests (`kubernetes/` directory)
- GitHub Actions workflows (`.github/workflows/`)
- Docker Compose (`docker-compose.yml`)
- **Store in Git, apply with kubectl/docker**

### Code Editor (Bicep files):
- Infrastructure as Code (`infrastructure/` directory)
- **Store in Git, deploy with Azure CLI**

### Code Editor (Application code):
- Your actual app logic (`backend/src/`, `frontend/src/`)
- Configuration files (`.env.example`, `config.js`)
- **Store in Git, runs on Azure**

### GitHub Website:
- Managing secrets (Settings → Secrets)
- Monitoring workflows (Actions tab)
- Code reviews (Pull Requests)
- **Repository management**

---

## 📚 Quick Reference: File Extensions

| Extension | What It Is | Where | Tool to Edit | Tool to Run |
|-----------|------------|-------|--------------|-------------|
| `.bicep` | Azure Infrastructure | `infrastructure/` | VS Code | `az deployment group create` |
| `.yaml` / `.yml` | Kubernetes manifests | `kubernetes/` | VS Code | `kubectl apply -f` |
| `.yml` (workflows) | GitHub Actions | `.github/workflows/` | VS Code | Automatic (on push) |
| `.js` / `.jsx` | JavaScript code | `backend/src/`, `frontend/src/` | VS Code | `node` / `npm start` |
| `.json` | Configuration | Project root | VS Code | Various (depending on context) |
| `.env` | Environment variables | Project root | VS Code | Loaded by app (DO NOT COMMIT!) |
| `.md` | Documentation | Anywhere | VS Code | View on GitHub |
| `Dockerfile` | Container build | Project root, subdirectories | VS Code | `docker build` |
| `.dockerignore` | Docker ignore | Project root | VS Code | Automatic (during build) |
| `.gitignore` | Git ignore | Project root | VS Code | Automatic (on commit) |

---

## 🎯 Your Next Steps

1. ✅ **Day 43: Local Kubernetes Practice**
   - Install k3s: Terminal (`curl -sfL https://get.k3s.io | sh -`)
   - Create YAML: Code Editor (`kubernetes/deployment.yaml`)
   - Deploy: Terminal (`kubectl apply -f kubernetes/`)
   - Check: Terminal (`kubectl get pods`)

2. 🔄 **Week After Day 43: Build First MERN API**
   - Create project: Terminal (`mkdir my-api && cd my-api`)
   - Initialize: Terminal (`npm init -y`)
   - Install Express: Terminal (`npm install express`)
   - Write code: Code Editor (`src/index.js`)
   - Test: Terminal (`npm start`)
   - Dockerize: Code Editor (`Dockerfile`), then Terminal (`docker build -t my-api .`)
   - Deploy: Terminal (`az container create ...`)

3. 🔄 **Month 2: Add Key Vault Integration**
   - Create Key Vault: Azure Portal OR Terminal (`az keyvault create`)
   - Add secrets: Portal OR Terminal (`az keyvault secret set`)
   - Configure app: Portal OR Terminal (`az webapp config appsettings set`)
   - Update code: Code Editor (use `process.env.SECRET_NAME`)

4. 🔄 **Month 3: Set Up CI/CD**
   - Create workflow: Code Editor (`.github/workflows/deploy.yml`)
   - Add secrets: GitHub website (Settings → Secrets)
   - Commit: Terminal (`git add .github/workflows/deploy.yml && git commit && git push`)
   - Monitor: GitHub website (Actions tab)

---

**Created:** October 25, 2025  
**Author:** YomHub  
**Purpose:** Bridge theory → practice gap  
**Target Audience:** Developers frustrated by "where do I actually do this?"  
**Key Insight:** Every task has a specific tool, location, and file type. Know this, and you're unstoppable! 🚀

