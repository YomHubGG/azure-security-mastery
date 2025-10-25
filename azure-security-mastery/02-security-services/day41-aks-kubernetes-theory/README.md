# 📘 Day 41: Azure Kubernetes Service (AKS) - Theory & Fundamentals

**Session #21** | October 25, 2025 | Theory-First Approach  
**Goal:** Demystify Kubernetes and AKS without spending €80-150/month  
**Approach:** Comprehensive theory → Next session: Local practice  
**Cost:** €0 (Theory only, hands-on Day 43 with local K8s)

---

## 🎯 Why This Theory-First Approach?

**The Problem:**
- AKS requires €80-150/month minimum (worker nodes + load balancer + storage)
- That breaks our €0 learning budget
- But Kubernetes knowledge is CRITICAL for cloud careers

**The Solution:**
1. **Today (Day 41):** Learn ALL the theory and concepts
2. **Next session (Day 43):** Practice locally with k3s/minikube (€0)
3. **Result:** Full Kubernetes knowledge without Azure costs

**What You'll Gain:**
- ✅ Interview-ready Kubernetes knowledge
- ✅ Understand when to use AKS vs ACI vs VMs
- ✅ Security concepts (RBAC, Network Policies, Pod Security)
- ✅ Architecture understanding (control plane, nodes, networking)
- ✅ Cost analysis (why it's expensive but worth it for enterprises)

---

## 📚 Table of Contents

1. [What is Kubernetes?](#what-is-kubernetes)
2. [Container Orchestration Explained](#container-orchestration-explained)
3. [AKS Architecture](#aks-architecture)
4. [Kubernetes Core Concepts](#kubernetes-core-concepts)
5. [AKS Security Features](#aks-security-features)
6. [When to Use AKS vs ACI vs VMs](#when-to-use-what)
7. [Cost Analysis](#cost-analysis)
8. [Interview Prep: Key Questions](#interview-prep)

---

## 1. What is Kubernetes?

### The Elevator Pitch

**Kubernetes (K8s)** is a **container orchestration platform** that automates:
- **Deployment:** Rolling out containers across multiple machines
- **Scaling:** Adding/removing containers based on load
- **Management:** Health checks, restarts, load balancing
- **Networking:** Service discovery and communication
- **Storage:** Persistent data management

### The Real-World Analogy

Think of **containers** as **shipping containers** (standardized boxes with your code):

| Without Kubernetes | With Kubernetes |
|-------------------|-----------------|
| You manually place each container on trucks (servers) | Kubernetes is the **logistics company** that figures out which truck each container goes on |
| You manually restart failed containers | Kubernetes **automatically restarts** them |
| You manually distribute traffic | Kubernetes **load balances** automatically |
| You manually scale during Black Friday | Kubernetes **auto-scales** based on CPU/memory |

### Why It Exists

**Problem Kubernetes Solves:**

```
2010s: You have 100 containers to run
├── Which servers should host them?
├── What if a container crashes?
├── How do containers find each other?
├── How do you update without downtime?
├── How do you scale from 10 to 1000 containers?
└── How do you manage this across dev/staging/prod?

Answer: You need an orchestrator → Kubernetes!
```

### Kubernetes vs Docker

**Common Confusion:**

| Tool | What It Does | Relationship |
|------|-------------|--------------|
| **Docker/Podman** | **Builds** and **runs** single containers | The engine that runs containers |
| **Kubernetes** | **Orchestrates** many containers across many servers | Uses Docker/Podman to run containers |

**Analogy:**
- Docker = Car engine
- Kubernetes = Traffic control system for thousands of cars

**You've already used Docker!**
- Day 35: Built container with Podman
- Day 37: Published to registry
- Day 39: Deployed to ACI (single container)
- **Today:** Learn how to manage 100s of containers with K8s

---

## 2. Container Orchestration Explained

### What Problems Does Orchestration Solve?

#### **Problem 1: Manual Placement**
```
Without Orchestration:
├── You have 50 containers
├── You have 10 servers
├── You manually SSH to each server and run containers
├── Server 3 is overloaded, server 7 is idle
└── Nightmare to manage!

With Kubernetes:
├── You say: "Run these 50 containers"
├── Kubernetes automatically distributes across 10 servers
├── Balances load based on CPU/memory
└── Done in seconds!
```

#### **Problem 2: Failures**
```
Without Orchestration:
├── Container crashes at 3am
├── App is down until you wake up and restart it
└── Customers can't access your site

With Kubernetes:
├── Container crashes
├── Kubernetes detects it in seconds
├── Automatically starts a new container
└── Zero downtime, you sleep peacefully
```

#### **Problem 3: Scaling**
```
Without Orchestration:
├── Black Friday traffic spike
├── You manually start 50 more containers
├── Takes 30 minutes
├── By then, site has crashed

With Kubernetes:
├── CPU usage hits 80%
├── Kubernetes auto-scales from 10 → 50 containers
├── Takes 60 seconds
└── Site handles traffic smoothly
```

#### **Problem 4: Updates**
```
Without Orchestration:
├── Need to update app to v2.0
├── Stop all containers (DOWNTIME!)
├── Start new version
├── If it fails, manually rollback
└── Stressful deployment!

With Kubernetes:
├── Rolling update: Stop 1 container, start new one
├── Repeat until all updated (ZERO DOWNTIME)
├── If v2.0 crashes, auto-rollback to v1.0
└── Safe, automated deployment
```

### Orchestration Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     CONTROL PLANE                           │
│  (The Brain - Decides what should happen)                   │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │
│  │ API Server  │  │  Scheduler  │  │  Controller │       │
│  │ (Commands)  │  │  (Placement)│  │  (Healing)  │       │
│  └─────────────┘  └─────────────┘  └─────────────┘       │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                    WORKER NODES                              │
│  (The Muscles - Actually run containers)                     │
│                                                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │   Node 1    │  │   Node 2    │  │   Node 3    │        │
│  │             │  │             │  │             │        │
│  │ [🐳][🐳]    │  │ [🐳][🐳]    │  │ [🐳][🐳]    │        │
│  │ [🐳][🐳]    │  │ [🐳][🐳]    │  │ [🐳][🐳]    │        │
│  └─────────────┘  └─────────────┘  └─────────────┘        │
└──────────────────────────────────────────────────────────────┘
```

---

## 3. AKS Architecture

### What is AKS (Azure Kubernetes Service)?

**AKS = Managed Kubernetes on Azure**

**What "Managed" Means:**
- ✅ **Microsoft runs the control plane** (you don't pay for it!)
- ✅ **Microsoft patches and updates K8s** (security handled)
- ✅ **Microsoft ensures control plane HA** (99.95% uptime SLA)
- ❌ **You pay for worker nodes** (the VMs running containers)
- ❌ **You manage worker node updates** (but Microsoft helps)

### AKS vs "Regular" Kubernetes

| Component | Regular K8s | AKS (Managed) |
|-----------|-------------|---------------|
| **Control Plane** | You host on 3+ VMs | Microsoft hosts (FREE!) |
| **Control Plane Cost** | ~€75/month (3 VMs) | €0 (Microsoft pays) |
| **Worker Nodes** | You provision VMs | You provision VMs |
| **Worker Node Cost** | You pay | You pay |
| **K8s Updates** | Manual (scary!) | Microsoft patches control plane |
| **Monitoring** | You set up | Azure Monitor included |
| **Security** | DIY | Azure AD integration, security policies |

**Why AKS is Worth It:**
- Control plane management = HUGE time saver
- Security patches automatic
- Azure integration (Azure AD, Key Vault, ACR)
- Still, you pay for worker nodes (that's the catch!)

### AKS Architecture Diagram

```
┌──────────────────────────────────────────────────────────────────┐
│                    AZURE SUBSCRIPTION                            │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              AKS CONTROL PLANE (FREE!)                      │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │ API Server | Scheduler | Controller Manager | etcd   │  │ │
│  │  │ (Managed by Microsoft, you never see this)           │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  └────────────────────────────────────────────────────────────┘ │
│                           │                                      │
│                           ▼                                      │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │         AZURE VIRTUAL NETWORK (Your Control)                │ │
│  │                                                             │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │ │
│  │  │  Worker VM 1 │  │  Worker VM 2 │  │  Worker VM 3 │    │ │
│  │  │  (D2s_v3)    │  │  (D2s_v3)    │  │  (D2s_v3)    │    │ │
│  │  │  2 vCPU/8GB  │  │  2 vCPU/8GB  │  │  2 vCPU/8GB  │    │ │
│  │  │  €70/month   │  │  €70/month   │  │  €70/month   │    │ │
│  │  │              │  │              │  │              │    │ │
│  │  │  [Pod][Pod]  │  │  [Pod][Pod]  │  │  [Pod][Pod]  │    │ │
│  │  │  [Pod][Pod]  │  │  [Pod][Pod]  │  │  [Pod][Pod]  │    │ │
│  │  └──────────────┘  └──────────────┘  └──────────────┘    │ │
│  │                                                             │ │
│  └────────────────────────────────────────────────────────────┘ │
│                           │                                      │
│  ┌────────────────────────▼───────────────────────────────────┐ │
│  │  Azure Load Balancer (Public IP)      | €15/month          │ │
│  │  ├─ Routes traffic to worker nodes                         │ │
│  └────────────────────────────────────────────────────────────┘ │
│                           │                                      │
│  ┌────────────────────────▼───────────────────────────────────┐ │
│  │  Azure Managed Disks (Persistent Storage) | €5-15/month    │ │
│  │  ├─ Volumes for database data, etc.                        │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  TOTAL MONTHLY COST: €215-225 (3-node cluster)                  │
└──────────────────────────────────────────────────────────────────┘
```

**Cost Breakdown for Small AKS Cluster:**
- Control plane: €0 (Microsoft pays!)
- 3x Worker VMs (D2s_v3): €210/month
- Load Balancer: €15/month
- Storage: €5-10/month
- **Total: ~€230/month** (that's why we're doing theory + local practice!)

---

## 4. Kubernetes Core Concepts

### The Building Blocks

#### **1. Pod** 🫘

**What:** The smallest deployable unit in Kubernetes

**Think of it as:** A wrapper around one (or more) containers

**Key Concepts:**
- Usually 1 container per Pod (your secure-app)
- Pods share network namespace (can talk via localhost)
- Pods are **ephemeral** (can be killed/recreated anytime)
- Each Pod gets its own IP address

**Example:**
```yaml
# Your secure-app as a Pod
apiVersion: v1
kind: Pod
metadata:
  name: secure-app
spec:
  containers:
  - name: secure-app
    image: ghcr.io/yomhubgg/secure-app:1.0.0
    ports:
    - containerPort: 3000
```

**Real-World Analogy:**
- Container = App process
- Pod = The VM/computer the process runs on
- Multiple Pods = Multiple computers

#### **2. Deployment** 🚀

**What:** Manages a set of identical Pods

**Think of it as:** Auto-scaling group for containers

**Key Concepts:**
- You say: "I want 5 replicas of secure-app"
- Deployment ensures 5 Pods are always running
- If 1 Pod crashes, Deployment starts a new one
- Handles rolling updates (update app without downtime)

**Example:**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: secure-app-deployment
spec:
  replicas: 3  # Always keep 3 Pods running
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
        image: ghcr.io/yomhubgg/secure-app:1.0.0
        resources:
          requests:
            cpu: 100m       # 0.1 CPU
            memory: 128Mi
          limits:
            cpu: 500m       # Max 0.5 CPU
            memory: 512Mi
```

**What This Does:**
1. Creates 3 Pods running secure-app
2. If 1 Pod crashes → Starts a new one (self-healing)
3. Can scale: `kubectl scale deployment secure-app-deployment --replicas=10`
4. Rolling update: Updates Pods one at a time (zero downtime)

#### **3. Service** 🌐

**What:** Stable network endpoint for Pods

**Problem It Solves:**
- Pods have dynamic IPs (change when restarted)
- How do other apps find your secure-app if IP changes?
- Answer: Service = stable DNS name + IP

**Types:**

| Type | Use Case | Example |
|------|----------|---------|
| **ClusterIP** | Internal only | Database (only app can access) |
| **NodePort** | External access (dev/test) | Expose on port 30000-32767 |
| **LoadBalancer** | External access (production) | Azure creates public IP + LB (€15/month) |

**Example:**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: secure-app-service
spec:
  type: LoadBalancer  # Azure creates public IP
  selector:
    app: secure-app   # Target Pods with this label
  ports:
  - port: 80          # External port
    targetPort: 3000  # Container port
```

**What This Does:**
1. Creates a stable endpoint: `secure-app-service:80`
2. Load balances traffic across 3 Pods
3. If using LoadBalancer type → Azure creates public IP
4. External users access: `http://<public-ip>` → Routes to Pods

#### **4. Namespace** 📦

**What:** Virtual cluster within a cluster

**Think of it as:** Folders for organizing resources

**Use Cases:**
- Separate dev/staging/prod environments
- Team isolation (team-a, team-b namespaces)
- Resource quotas per namespace

**Example:**
```bash
kubectl create namespace production
kubectl create namespace development

# Deploy to specific namespace
kubectl apply -f deployment.yaml -n production
```

#### **5. ConfigMap & Secret** 🔐

**What:** Store configuration and secrets

**ConfigMap:** Non-sensitive config (URLs, feature flags)
**Secret:** Sensitive data (passwords, API keys) - base64 encoded

**Example:**
```yaml
# ConfigMap
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  API_URL: "https://api.example.com"
  LOG_LEVEL: "info"

---
# Secret
apiVersion: v1
kind: Secret
metadata:
  name: app-secrets
type: Opaque
data:
  DB_PASSWORD: cGFzc3dvcmQxMjM=  # base64 encoded
```

**Using in Pod:**
```yaml
spec:
  containers:
  - name: secure-app
    env:
    - name: API_URL
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: API_URL
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: app-secrets
          key: DB_PASSWORD
```

**Better Approach (Azure):** Use Azure Key Vault with CSI Secrets Store driver (covered in Day 17!)

---

## 5. AKS Security Features

### Security Layers in AKS

#### **Layer 1: Cluster-Level Security**

##### **1. Azure AD Integration**

**What It Does:** Use your Azure AD accounts to access cluster

**Without Azure AD:**
```bash
# Anyone with kubeconfig file = full access (scary!)
kubectl get pods --kubeconfig=./stolen-config.yaml
```

**With Azure AD:**
```bash
# Must authenticate with Azure AD
az aks get-credentials --resource-group rg --name aks-cluster
# Prompts for Azure AD login
# Access controlled by Azure RBAC
```

**Benefits:**
- MFA enforcement
- Conditional Access policies
- Audit logs (who accessed what)
- No static credentials

##### **2. RBAC (Role-Based Access Control)**

**What It Does:** Control who can do what

**Example Roles:**

| Role | Permissions | Use Case |
|------|------------|----------|
| **Cluster Admin** | Everything | Ops team |
| **Namespace Admin** | Full access to 1 namespace | Dev team lead |
| **Pod Reader** | Read-only Pods | Monitoring tool |
| **Secret Admin** | Manage secrets only | Security team |

**Example RBAC:**
```yaml
# Role: Can only view Pods in "production" namespace
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  namespace: production
  name: pod-reader
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list", "watch"]

---
# Bind role to user
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: read-pods
  namespace: production
subjects:
- kind: User
  name: "user@company.com"
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: Role
  name: pod-reader
  apiGroup: rbac.authorization.k8s.io
```

**Result:** `user@company.com` can ONLY view Pods in production, nothing else!

##### **3. Network Policies**

**What They Do:** Firewall rules for Pods

**Problem:**
- By default, all Pods can talk to all Pods
- Database Pod shouldn't talk to Internet
- Frontend should only talk to Backend

**Example Network Policy:**
```yaml
# Only allow traffic from app-frontend to app-backend
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-policy
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend
    ports:
    - protocol: TCP
      port: 8080
```

**Result:**
- ✅ Frontend → Backend: Allowed
- ❌ Random Pod → Backend: Blocked
- ❌ Backend → Internet: Blocked

**Real-World Analogy:**
- Network Policy = Firewall rules
- Default = All doors open
- With policies = Only specific doors open to specific people

#### **Layer 2: Pod-Level Security**

##### **1. Pod Security Standards**

**Three Levels:**

| Level | Description | Restrictions |
|-------|-------------|--------------|
| **Privileged** | No restrictions | Dangerous! Avoid! |
| **Baseline** | Minimal restrictions | Block known privilege escalations |
| **Restricted** | Heavily restricted | Non-root, drop capabilities, read-only filesystem |

**Example (Restricted Pod):**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: secure-app
spec:
  securityContext:
    runAsNonRoot: true      # Must run as non-root user
    runAsUser: 1001         # UID 1001 (like your nodejs user!)
    fsGroup: 1001
  containers:
  - name: secure-app
    image: ghcr.io/yomhubgg/secure-app:1.0.0
    securityContext:
      allowPrivilegeEscalation: false
      readOnlyRootFilesystem: true
      capabilities:
        drop:
        - ALL              # Drop all Linux capabilities
    resources:
      limits:
        cpu: 500m
        memory: 512Mi
```

**What This Does:**
- ✅ Runs as user 1001 (non-root)
- ✅ Can't escalate to root
- ✅ Filesystem is read-only
- ✅ No special Linux capabilities
- ✅ Resource limits enforced

**This is the SAME security you implemented on Day 35!** 🎯

##### **2. Admission Controllers**

**What They Do:** Gatekeepers that validate/mutate resources before creation

**Example: OPA Gatekeeper**
```yaml
# Policy: Block images from Docker Hub (only allow ACR/GHCR)
apiVersion: constraints.gatekeeper.sh/v1beta1
kind: K8sAllowedRepos
metadata:
  name: allow-only-ghcr
spec:
  match:
    kinds:
    - apiGroups: [""]
      kinds: ["Pod"]
  parameters:
    repos:
    - "ghcr.io/"
    - "yourcompany.azurecr.io/"
```

**Result:**
```bash
kubectl run test --image=nginx:latest
# ERROR: Image 'nginx:latest' not from allowed repo!

kubectl run test --image=ghcr.io/yomhubgg/secure-app:1.0.0
# ✅ SUCCESS: Image from allowed repo
```

**Use Cases:**
- Enforce image registries
- Block privileged containers
- Require resource limits
- Enforce naming conventions

#### **Layer 3: Azure Integration Security**

##### **1. Azure Key Vault Integration**

**Problem:**
- Kubernetes Secrets are base64 encoded (not encrypted!)
- Anyone with cluster access can decode secrets

**Solution: CSI Secrets Store Driver**
```yaml
# Pod reads secret from Azure Key Vault (not K8s Secret)
apiVersion: v1
kind: Pod
metadata:
  name: secure-app
spec:
  containers:
  - name: secure-app
    volumeMounts:
    - name: secrets-store
      mountPath: "/mnt/secrets"
      readOnly: true
  volumes:
  - name: secrets-store
    csi:
      driver: secrets-store.csi.k8s.io
      volumeAttributes:
        secretProviderClass: "azure-keyvault"
```

**Benefits:**
- ✅ Secrets stored in Azure Key Vault (encrypted, audited)
- ✅ Pods read secrets at runtime (not stored in K8s)
- ✅ Key Vault access logs (who accessed what secret)
- ✅ Automatic secret rotation

**You already learned Key Vault on Day 17!** Now you know how it integrates with AKS. 🎯

##### **2. Managed Identity**

**What:** Pods get Azure AD identity (no passwords!)

**Without Managed Identity:**
```bash
# Pod needs to access Azure Storage
# Must store credentials in Secret (risky!)
AZURE_STORAGE_KEY=<secret>
```

**With Managed Identity:**
```bash
# Pod gets Azure AD identity
# Accesses Storage without credentials
# Azure handles authentication
```

**How It Works:**
1. Enable Managed Identity on AKS
2. Assign identity to Pod
3. Grant identity permissions to Azure resources
4. Pod authenticates automatically

**Example:**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: secure-app
  labels:
    aadpodidbinding: azure-identity
spec:
  containers:
  - name: secure-app
    image: ghcr.io/yomhubgg/secure-app:1.0.0
    # No credentials needed!
    # Azure SDK automatically uses managed identity
```

##### **3. Private Clusters**

**What:** Control plane not exposed to Internet

**Public AKS (default):**
```
Internet → AKS API Server → Your laptop kubectl commands
                            (Anyone can try to access)
```

**Private AKS:**
```
Internet  ❌  AKS API Server
            ↕
VPN/Bastion → AKS API Server → kubectl commands
              (Only accessible from your network)
```

**Use Cases:**
- Enterprises with strict security requirements
- Compliance needs (GDPR, HIPAA)
- Prevent API server attacks

**Trade-off:** More complex setup (need VPN/Bastion to access cluster)

#### **Layer 4: Monitoring & Compliance**

##### **1. Azure Defender for Kubernetes**

**What:** Threat detection for AKS

**Detects:**
- ❌ Cryptocurrency mining containers
- ❌ Privilege escalation attempts
- ❌ Suspicious network traffic
- ❌ Malicious container images
- ❌ Access from malicious IPs

**Cost:** ~€12/node/month (we're skipping this, but good to know)

##### **2. Azure Policy for AKS**

**What:** Enforce governance at scale

**Examples:**
- Require all Pods to have resource limits
- Block privileged containers
- Enforce image scanning
- Require labels on all resources

**How It Works:**
```yaml
# Azure Policy: Require CPU/memory limits on all Pods
apiVersion: policy.azure.com/v1
kind: AzurePolicy
metadata:
  name: require-resource-limits
spec:
  policyDefinition: "Container CPU and memory resource limits"
  enforcementMode: "Default"
```

**Result:**
```bash
kubectl apply -f pod-without-limits.yaml
# ERROR: Denied by Azure Policy: Missing resource limits!
```

##### **3. Audit Logging**

**What:** Track all API server actions

**Example Audit Log:**
```json
{
  "timestamp": "2025-10-25T10:30:00Z",
  "user": "user@company.com",
  "action": "create",
  "resource": "pods",
  "namespace": "production",
  "name": "secure-app-abc123",
  "result": "success"
}
```

**Use Cases:**
- Security investigations ("Who deleted the production database?")
- Compliance audits (GDPR requires access logs)
- Troubleshooting ("Why did my Pod fail to start?")

---

## 6. When to Use AKS vs ACI vs VMs

### The Decision Matrix

| Requirement | Use AKS | Use ACI | Use VM | Use Functions |
|-------------|---------|---------|--------|---------------|
| **Running 1-5 containers** | ❌ Overkill | ✅ Perfect | ✅ Also good | Consider |
| **Running 10-100 containers** | ✅ Perfect | ❌ Too many | ❌ Too manual | ❌ Not suited |
| **Need auto-scaling** | ✅ Built-in | ❌ Manual | ❌ Complex | ✅ Built-in |
| **Need zero downtime updates** | ✅ Rolling updates | ❌ Recreate | ❌ Complex | ✅ Built-in |
| **Need service mesh (mTLS)** | ✅ Istio/Linkerd | ❌ Not supported | ❌ Manual setup | ❌ Not applicable |
| **Short-lived tasks (<10 min)** | ❌ Overkill | ✅ Perfect | ❌ Overkill | ✅ Perfect |
| **24/7 services** | ✅ Cost-effective | ❌ Same as VM | ✅ Good | ❌ Expensive |
| **Need Kubernetes skills** | ✅ Great learning | ❌ Different model | ❌ Different model | ❌ Serverless |
| **Budget: €0-20/month** | ❌ Too expensive | ✅ Within budget | ✅ Within budget | ✅ Free tier |
| **Budget: €100+/month** | ✅ Makes sense | ❌ Limited | ✅ Also good | Depends |

### Real-World Scenarios

#### **Scenario 1: Startup with 3 Microservices**

**Your App:**
- Frontend (React)
- Backend API (Node.js)
- Database (PostgreSQL)

**Best Choice: ACI or VMs**

**Why NOT AKS:**
- Only 3 containers = AKS overkill
- €230/month for AKS vs €50/month for 3 ACI containers
- No need for Kubernetes complexity

**Use ACI if:**
- Stateless services (Frontend, Backend)
- Can tolerate occasional restarts

**Use VMs if:**
- Need persistent storage (Database)
- Need 24/7 uptime guarantees

#### **Scenario 2: E-commerce Site with 20 Microservices**

**Your App:**
- 5 Frontend services
- 10 Backend APIs
- 3 Databases
- 2 Background workers

**Best Choice: AKS**

**Why:**
- 20 containers = Managing individually is nightmare
- Need auto-scaling (Black Friday traffic spikes)
- Need rolling updates (deploy without downtime)
- Service discovery (services need to find each other)
- €230/month for AKS vs €500+/month for 20 ACI containers

**AKS Wins:**
- Centralized management
- Auto-scaling across all services
- Service mesh for security (mTLS between services)
- Cost-effective at this scale

#### **Scenario 3: Batch Processing Jobs**

**Your App:**
- Process images uploaded by users
- Each job runs 5 minutes
- 100 jobs per day

**Best Choice: ACI or Azure Functions**

**Why NOT AKS:**
- Jobs are short-lived
- No need for 24/7 cluster running
- Pay only for 500 minutes/day of processing

**Cost Comparison:**
- AKS: €230/month (cluster always running)
- ACI: ~€5/month (only pay for 500 min × 30 days)
- Functions: ~€2/month (serverless, pay per execution)

**Winner: Azure Functions or ACI**

#### **Scenario 4: Machine Learning Platform**

**Your App:**
- 50 ML models running as containers
- Need GPU nodes
- Auto-scale based on inference requests

**Best Choice: AKS**

**Why:**
- Complex orchestration needs
- GPU node pools
- Model versioning and A/B testing
- Scale from 10 → 100 → 10 containers based on load

**AKS Features Used:**
- GPU node pools
- Horizontal Pod Autoscaler (scale based on requests)
- Rolling updates (deploy new model versions)
- Service mesh (route traffic for A/B testing)

### Summary Decision Tree

```
How many containers do you need?
├── 1-5 containers
│   ├── Short-lived (minutes) → Azure Functions or ACI
│   └── Long-running (24/7) → ACI or VMs
│
├── 6-20 containers
│   ├── Simple architecture → ACI + Load Balancer
│   └── Need orchestration → Consider AKS (if budget allows)
│
└── 21+ containers
    ├── Budget <€200/month → Multiple VMs + Docker Compose
    └── Budget >€200/month → AKS (worth the investment)

Do you need these features?
├── Auto-scaling → AKS or Functions
├── Zero downtime updates → AKS or Functions
├── Service mesh (mTLS) → AKS only
├── GPU workloads → AKS only
└── Kubernetes skills → AKS (great learning)
```

---

## 7. Cost Analysis

### Why AKS is Expensive

#### **Minimum Production AKS Costs**

| Component | Size | Monthly Cost | Annual Cost |
|-----------|------|--------------|-------------|
| **Control Plane** | Managed by Microsoft | €0 ✅ | €0 |
| **Worker Node 1** | D2s_v3 (2 vCPU, 8GB) | €70 | €840 |
| **Worker Node 2** | D2s_v3 (2 vCPU, 8GB) | €70 | €840 |
| **Worker Node 3** | D2s_v3 (2 vCPU, 8GB) | €70 | €840 |
| **Load Balancer** | Standard | €15 | €180 |
| **Public IP** | Static | €3 | €36 |
| **Managed Disks** | 3x 128GB Premium SSD | €15 | €180 |
| **Egress Traffic** | First 100GB free | ~€5 | ~€60 |
| **Azure Monitor** | Logs + Metrics | ~€10 | ~€120 |
| **Defender for K8s** | Optional | ~€36 | ~€432 |
| **Total (minimum)** | | **€238/month** | **€2,856/year** |
| **Total (with Defender)** | | **€274/month** | **€3,288/year** |

#### **Cost Comparison: Running secure-app for 1 Year**

| Service | Setup | Monthly Cost | Annual Cost | Notes |
|---------|-------|--------------|-------------|-------|
| **ACI (0.5 CPU, 24/7)** | Free | €17 | €204 | Simple, no management |
| **VM B1s (1 CPU, 1GB)** | Free | €15 | €180 | More control, persistence |
| **AKS (3-node cluster)** | Free | €238 | €2,856 | Orchestration, auto-scale |
| **GitHub CR + Local** | Free | €0 | €0 | Learning/testing |

**For YOUR secure-app alone:** AKS is 14x more expensive than ACI! 😱

**AKS makes sense when:**
- Running 20+ containers (management complexity saved)
- Need orchestration features (auto-scale, rolling updates)
- Enterprise requirements (HA, compliance, audit logs)

#### **Optimization Strategies (if you HAD to use AKS)**

##### **1. Use Spot Instances**
```
Regular D2s_v3: €70/month
Spot D2s_v3: €15-25/month (70% discount!)

Risk: Can be evicted with 30 seconds notice
Use for: Non-critical workloads, batch jobs
```

##### **2. Autoscaling Node Pools**
```yaml
# Scale from 1 node (idle) to 10 nodes (peak)
nodepool:
  minCount: 1    # Minimum 1 node (€70/month)
  maxCount: 10   # Scale up to 10 during peaks
```

**Cost:**
- Off-peak (1 node): €70/month
- Peak (10 nodes): €700/month
- Average (3 nodes): €210/month
- Savings: Pay for what you use!

##### **3. Use B-Series Burstable VMs**
```
Regular D2s_v3: €70/month (dedicated CPU)
B2s (burstable): €30/month (shared CPU with burst)

Trade-off: Performance varies, but good for dev/test
```

##### **4. Reserved Instances (1-year commit)**
```
D2s_v3 pay-as-you-go: €70/month
D2s_v3 1-year reserved: €45/month (35% discount)

Commitment: Must pay for 12 months even if not used
Good for: Stable production workloads
```

##### **5. Dev/Test Subscriptions**
```
Some Azure enterprise agreements offer:
- Dev/Test pricing (up to 40% discount)
- Must be non-production use only
```

#### **Free/Cheap Alternatives for Learning**

| Approach | Cost | Pros | Cons |
|----------|------|------|------|
| **Local k3s/minikube** | €0 | ✅ Full K8s experience | ❌ Not "real" Azure |
| **AKS Free Tier (if exists)** | €0 control + €70 nodes | ✅ Real AKS | ❌ Still pay for nodes |
| **Azure Dev/Test Labs** | Varies | ✅ Educational discount | ❌ Limited availability |
| **GitHub Codespaces + k3s** | €0 (60 hours/month) | ✅ Cloud-based, free | ❌ 60-hour limit |
| **AKS Deploy-and-Delete** | ~€0.10/hour | ✅ Quick tests OK | ❌ Must remember to delete! |
| **Theory + Documentation** | €0 | ✅ Interview-ready | ❌ No hands-on |

**OUR CHOICE (BCA Plan):**
1. **Today:** Theory (€0) ← We are here!
2. **Next session:** Local k3s practice (€0)
3. **Skip:** Real AKS deployment (save €238/month)
4. **Result:** Full Kubernetes knowledge, zero cost! 🎯

---

## 8. Interview Prep: Key Questions

### Question 1: "What is Kubernetes?"

**Bad Answer:**
> "It's a container orchestration tool."

**Good Answer:**
> "Kubernetes is a container orchestration platform that automates deployment, scaling, and management of containerized applications. It solves problems like:
> - Placement: Which server should each container run on?
> - Self-healing: Automatically restart failed containers
> - Scaling: Add/remove containers based on load
> - Updates: Rolling updates without downtime
> - Service discovery: How containers find each other
>
> In production, you might have 100 containers across 10 servers. Kubernetes makes this manageable through declarative configuration – you describe the desired state, and K8s ensures reality matches that state."

### Question 2: "Why use AKS instead of self-managed Kubernetes?"

**Good Answer:**
> "AKS reduces operational overhead significantly:
> 1. **Control plane is free and fully managed** – Microsoft handles patching, updates, and HA. With self-managed K8s, you'd need 3+ VMs for control plane (€75/month) and manage upgrades yourself.
> 2. **Azure integration** – Native Azure AD auth, Key Vault secrets, ACR integration, Azure Monitor
> 3. **Security** – Microsoft patches control plane CVEs within days, you don't have to track Kubernetes security advisories
> 4. **Time savings** – Focus on apps, not cluster management
>
> Trade-off: You still pay for worker nodes, but that's unavoidable even with self-managed."

### Question 3: "Explain the difference between a Pod, Deployment, and Service."

**Good Answer:**
> "These are 3 layers of abstraction:
>
> **Pod:** The smallest unit – a wrapper around 1+ containers with shared network/storage. Think of it as a single VM running your app. Pods are ephemeral (can be killed/recreated).
>
> **Deployment:** Manages a set of identical Pods. You declare 'I want 5 replicas' and Deployment ensures 5 Pods are always running. Handles self-healing (restarts failed Pods) and rolling updates (update app without downtime).
>
> **Service:** Provides stable networking for Pods. Since Pod IPs change when restarted, Service gives a stable DNS name and IP. LoadBalancer type creates an Azure Load Balancer with public IP for external access.
>
> Analogy: Pod = computer, Deployment = ensure 5 computers always running, Service = stable address to reach those computers."

### Question 4: "How do you secure a Kubernetes cluster?"

**Good Answer:**
> "I use defense in depth with multiple layers:
>
> **Cluster Level:**
> - Azure AD integration with MFA
> - RBAC to limit who can do what
> - Private cluster (API server not exposed to internet)
> - Network policies (firewall rules between Pods)
>
> **Pod Level:**
> - Run as non-root user (same as my Day 35 container security)
> - Read-only root filesystem
> - Drop all Linux capabilities
> - Pod Security Standards (enforce 'Restricted' level)
> - Resource limits to prevent DoS
>
> **Data Level:**
> - Azure Key Vault for secrets (not Kubernetes Secrets)
> - CSI Secrets Store driver to inject secrets at runtime
> - Managed Identity (no passwords in code)
>
> **Admission Control:**
> - OPA Gatekeeper policies (block Docker Hub images, require resource limits)
> - Azure Policy for Kubernetes (governance at scale)
>
> **Monitoring:**
> - Azure Defender for threat detection
> - Audit logs for all API server actions
> - Azure Monitor for metrics and logs"

### Question 5: "When would you use ACI instead of AKS?"

**Good Answer:**
> "I'd choose based on scale and complexity:
>
> **Use ACI when:**
> - Running 1-10 containers (AKS overkill)
> - Short-lived workloads (batch jobs, CI/CD tasks)
> - Simple architecture (no service mesh needed)
> - Budget-conscious (€17/month for 0.5 CPU vs €238/month for AKS)
> - Don't need Kubernetes features
>
> **Use AKS when:**
> - Running 20+ containers (management complexity saved)
> - Need orchestration (auto-scaling, rolling updates, service discovery)
> - Multi-environment (dev/staging/prod with identical configs)
> - Need service mesh (mTLS between services)
> - Enterprise requirements (HA, compliance, audit)
> - At scale, AKS becomes more cost-effective per container
>
> Example: I deployed my secure-app to ACI (Day 39) because it's one container. If I had 20 microservices, I'd use AKS."

### Question 6: "What's the difference between Horizontal and Vertical scaling in Kubernetes?"

**Good Answer:**
> "**Horizontal Pod Autoscaler (HPA):** Add more Pods
> - Example: 3 Pods → 10 Pods when CPU hits 80%
> - Scales within existing nodes
> - Fast (seconds to add Pods)
> - Most common approach
>
> **Vertical Pod Autoscaler (VPA):** Increase Pod resources
> - Example: Change from 0.5 CPU → 1 CPU per Pod
> - Requires Pod restart
> - Slower (minutes)
> - Used when Pods are too small
>
> **Cluster Autoscaler:** Add more worker nodes
> - Example: 3 nodes → 5 nodes when no space for Pods
> - Works with HPA (add Pods first, then nodes if needed)
> - Slowest (minutes to provision VMs)
>
> Best practice: Use HPA for most workloads, Cluster Autoscaler as backup."

### Question 7: "How do you handle secrets in AKS?"

**Bad Answer:**
> "Store them in Kubernetes Secrets."

**Good Answer:**
> "Kubernetes Secrets are only base64 encoded, not encrypted at rest by default. Better approaches:
>
> **Azure Key Vault + CSI Secrets Store:**
> 1. Store secrets in Azure Key Vault (encrypted, audited)
> 2. Install CSI Secrets Store driver in AKS
> 3. Pods mount secrets as volumes from Key Vault
> 4. Secrets never stored in Kubernetes etcd
> 5. Automatic rotation supported
>
> **Managed Identity:**
> - Pods get Azure AD identity
> - Access Key Vault without passwords
> - Principle of least privilege (each Pod has own identity)
>
> **External Secrets Operator:**
> - Syncs secrets from Key Vault to K8s Secrets
> - Good for GitOps workflows
>
> I used Key Vault extensively on Day 17, so integrating with AKS is a natural extension."

### Question 8: "Explain a scenario where AKS cost you money unexpectedly."

**Good Answer (shows you understand the gotchas):**
> "Common cost surprises:
>
> **1. Forgot to delete cluster overnight:**
> - €238/month = €0.33/hour
> - Left running for 12 hours = €4 wasted
> - Lesson: Use auto-shutdown or cleanup scripts
>
> **2. Load Balancer not deleted:**
> - Even after deleting AKS cluster
> - Load Balancer kept running = €15/month
> - Lesson: Check resource group for orphaned resources
>
> **3. Cluster autoscaler went wild:**
> - Set maxCount: 100 nodes by mistake
> - Traffic spike → Autoscaler added 50 nodes
> - €3,500 bill for that day!
> - Lesson: Set reasonable maxCount limits
>
> **4. Public IP egress costs:**
> - First 100GB free, then €0.08/GB
> - Misconfigured backup job downloaded 5TB
> - €400 egress charge
> - Lesson: Use Azure Storage for backups (no egress)
>
> This is why I'm doing theory + local practice first – learn without burning money!"

---

## 📝 Summary: What You've Learned

### **Key Concepts**
✅ Kubernetes is a container orchestrator (automates deployment, scaling, management)
✅ AKS = Managed Kubernetes (Microsoft runs control plane for free, you pay for worker nodes)
✅ Core building blocks: Pod, Deployment, Service, Namespace, ConfigMap, Secret
✅ Security layers: Cluster (RBAC, Network Policies), Pod (Security Context), Azure (Key Vault, Managed Identity)
✅ Cost reality: AKS minimum €238/month (not suitable for single-container learning)

### **When to Use What**
✅ **ACI:** 1-10 containers, simple apps, budget-conscious
✅ **AKS:** 20+ containers, need orchestration, enterprise requirements
✅ **VMs:** Traditional workloads, need full OS control, persistent storage
✅ **Functions:** Event-driven, short-lived, serverless

### **Why We're Skipping AKS Deployment**
✅ **Cost:** €238/month minimum (breaks €0 budget)
✅ **Overkill:** Your secure-app is 1 container (ACI perfect for this)
✅ **Better approach:** Theory (today) + Local practice (next session) = Full knowledge, zero cost

### **Interview-Ready**
✅ You can explain Kubernetes architecture
✅ You understand security features (RBAC, Network Policies, Pod Security)
✅ You know cost trade-offs and when to use each service
✅ You can discuss Azure-specific features (Key Vault, Managed Identity, Azure AD)

---

## 🎯 Next Session: Day 43 - Local Kubernetes Practice

**What We'll Do:**
1. Install k3s (lightweight Kubernetes) on your Parrot machine
2. Deploy your secure-app to local K8s cluster
3. Practice kubectl commands
4. Implement security policies locally
5. Learn YAML manifests hands-on

**Cost:** €0 (local only)  
**Outcome:** Practical Kubernetes experience without Azure bills

---

## 📚 Additional Resources

**Official Docs:**
- [Kubernetes Docs](https://kubernetes.io/docs/)
- [AKS Documentation](https://learn.microsoft.com/en-us/azure/aks/)
- [AKS Best Practices](https://learn.microsoft.com/en-us/azure/aks/best-practices)

**Learning:**
- [Kubernetes The Hard Way](https://github.com/kelseyhightower/kubernetes-the-hard-way) - DIY K8s (advanced)
- [Play with Kubernetes](https://labs.play-with-k8s.com/) - Free browser-based lab

**Cost Calculators:**
- [Azure Pricing Calculator](https://azure.microsoft.com/en-us/pricing/calculator/)
- [AKS Cost Estimator](https://azure.github.io/aks-calculator/)

---

**Created:** October 25, 2025 (Session #21)  
**Author:** YomHub (Azure Security Journey)  
**Purpose:** Understand AKS and Kubernetes without spending €238/month  
**Next:** Day 43 - Local Kubernetes hands-on practice 🚀
