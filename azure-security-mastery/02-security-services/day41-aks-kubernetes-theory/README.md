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

## 🎮 Hands-On: Play with Kubernetes Exploration

**Date:** October 26, 2025  
**Environment:** [Play with Kubernetes](https://labs.play-with-k8s.com/)  
**Discovery:** Running as root on CentOS with RPM package manager!

### 🔍 What I Discovered

**System Details:**
```bash
# I found out I'm running on CentOS!
cat /etc/os-release
# Package manager: RPM (Red Hat Package Manager)
# Kernel warning: "kernel needs to be removed by rpm, because of grubby"

# Security finding: I'm root! 😱
whoami
# Output: root
```

**Security Implications:**
- ✅ **Good for learning:** No permission issues blocking experimentation
- ✅ **Realistic:** K8s cluster setup requires root/elevated privileges
- ⚠️ **In production:** NEVER run as root - use RBAC and least privilege
- 🛡️ **Playground safety:** Isolated environment, sessions expire after 4 hours

---

### 🚀 Fun Commands to Try

#### **1. System Reconnaissance** 🕵️

```bash
# Detailed OS information
cat /etc/os-release
uname -a                          # Kernel version (important for container runtime)
cat /proc/version                 # More detailed kernel info
hostname                          # Your node name
uptime                            # How long the system has been running

# Package manager exploration
rpm -qa | wc -l                   # Count installed packages
rpm -qa | grep -i kube            # Find Kubernetes-related packages
rpm -qa | grep -i docker          # Find Docker-related packages
rpm -qi kubeadm                   # Detailed info about kubeadm package

# Hardware info (it's virtual, but still interesting!)
cat /proc/cpuinfo | grep "model name" | head -1
free -h                           # Memory usage
df -h                             # Disk space
lsblk                             # Block devices
```

#### **2. Network Ninja Moves** 🌐

```bash
# Network configuration
ip addr show                      # All network interfaces
ip route show                     # Routing table
ss -tulnp                         # Open ports and listening services (netstat not on CentOS!)
cat /etc/hosts                    # Host mappings
cat /etc/resolv.conf              # DNS configuration

# Check what's listening (before K8s cluster is initialized)
ss -tulnp | grep LISTEN           # Note: netstat not available on RPM-based systems!

# After initializing K8s cluster, see K8s ports:
# - 6443: Kubernetes API server
# - 2379-2380: etcd server client API
# - 10250: Kubelet API
# - 10251: kube-scheduler
# - 10252: kube-controller-manager
```

**🔍 Real Output Analysis:**

```bash
# What I found with 'ss -tulnp':
tcp    LISTEN     0      128    127.0.0.1:42008      *:*       users:(("containerd",pid=314,fd=12))
tcp    LISTEN     0      128    127.0.0.1:10248      *:*       users:(("kubelet",pid=3640,fd=17))
tcp    LISTEN     0      128    127.0.0.1:10249      *:*       users:(("kube-proxy",pid=9779,fd=14))
tcp    LISTEN     0      128    127.0.0.11:33033     *:*                  
tcp    LISTEN     0      128    172.18.0.18:2379     *:*       users:(("etcd",pid=9201,fd=9))
tcp    LISTEN     0      128    127.0.0.1:2379       *:*       users:(("etcd",pid=9201,fd=8))
tcp    LISTEN     0      128    172.18.0.18:2380     *:*       users:(("etcd",pid=9201,fd=7))
tcp    LISTEN     0      128    127.0.0.1:2381       *:*       users:(("etcd",pid=9201,fd=14))
tcp    LISTEN     0      128    127.0.0.1:10257      *:*       users:(("kube-controller",pid=9224,fd=3))
tcp    LISTEN     0      128    127.0.0.1:10259      *:*       users:(("kube-scheduler",pid=8945,fd=3))
tcp    LISTEN     0      128    [::]:2375            [::]:*    users:(("dockerd",pid=134,fd=3))      # 🚨
tcp    LISTEN     0      128    [::]:10250           [::]:*    users:(("kubelet",pid=3640,fd=18))
tcp    LISTEN     0      128    [::]:6443            [::]:*    users:(("kube-apiserver",pid=9354,fd=3))
tcp    LISTEN     0      128    [::]:10255           [::]:*    users:(("kubelet",pid=3640,fd=14))
tcp    LISTEN     0      128    [::]:10256           [::]:*    users:(("kube-proxy",pid=9779,fd=12))
```

**🔐 Security Analysis of Bindings:**

| Binding | Meaning | Security Level | Services |
|---------|---------|----------------|----------|
| `127.0.0.1:PORT` | **IPv4 loopback** - Local only | ✅ **SECURE** | containerd, kubelet health, kube-proxy metrics, etcd, controller, scheduler |
| `172.18.0.18:PORT` | **Specific IPv4** - Container network | 🔐 **INTERNAL** | etcd cluster communication |
| `[::]:PORT` | **All IPv6 addresses** - Exposed! | ⚠️ **EXPOSED** | Docker API, kubelet, K8s API, kube-proxy |

**💡 IPv6 Notation Explained:**

```bash
[::]:2375
│  │  └─ Port number
│  └──── IPv6 "all zeros" shorthand (0000:0000:0000:0000:0000:0000:0000:0000)
└─────── Square brackets (required for IPv6 in URLs)

# IPv6 equivalents:
[::]        = 0.0.0.0        (all interfaces)
[::1]       = 127.0.0.1      (loopback)
```

**🚨 Port 2375 Warning:**

```bash
[::]:2375  →  Docker daemon API WITHOUT TLS encryption!

# This is DANGEROUS in production because:
# - No authentication required
# - No encryption
# - Full access to Docker = full access to host (as root!)
# - Anyone can: docker -H tcp://your-ip:2375 run --privileged ...

# Production should use:
[::]:2376  →  Docker daemon WITH TLS (encrypted + client certificates)

# Why it's here:
✅ Play with Kubernetes = isolated sandbox (safe)
✅ Session expires in 4 hours (temporary)
❌ NEVER expose 2375 in production!
```

**🎯 Key Kubernetes Ports Discovered:**

| Port | Service | Purpose | Security |
|------|---------|---------|----------|
| **2375** | Docker daemon | Container management | ⚠️ NO TLS (playground only) |
| **6443** | kube-apiserver | Main K8s API | ✅ TLS + RBAC required |
| **2379-2380** | etcd | K8s cluster database | 🔐 Internal + client certs |
| **10250** | kubelet | Node management | 🔐 Webhook auth |
| **10255** | kubelet | Read-only metrics | ⚠️ No auth (deprecated) |
| **10248** | kubelet | Health endpoint | ✅ Local only |
| **10249** | kube-proxy | Metrics | ✅ Local only |
| **10256** | kube-proxy | Health endpoint | ℹ️ Exposed but harmless |
| **10257** | kube-controller | Controller metrics | ✅ Local only |
| **10259** | kube-scheduler | Scheduler metrics | ✅ Local only |

**🧪 Test Docker Exposure (Playground Only!):**

```bash
# From another terminal/node in PWK:
docker -H tcp://node1:2375 ps        # List containers on node1
docker -H tcp://node1:2375 images    # List images on node1

# This works because 2375 is exposed without auth!
# In production, this would be a CRITICAL security vulnerability!
```

#### **3. Docker Deep Dive** 🐳

```bash
# Check Docker setup
docker version                    # Docker client and server versions
docker info                       # Detailed Docker configuration
docker images                     # List downloaded container images
docker ps -a                      # List all containers (running and stopped)

# Fun: Run a quick container
docker run hello-world            # Test Docker is working
docker run -it alpine sh          # Interactive Alpine Linux shell
# (type 'exit' to leave the container)

# See Docker's storage
ls -la /var/lib/docker/           # Docker's data directory
du -sh /var/lib/docker/           # How much space Docker is using
```

#### **4. Kubernetes Cluster Play** ☸️

```bash
# Check K8s tools
kubectl version --client          # Kubectl version
kubeadm version                   # Kubeadm version
kubelet --version                 # Kubelet version

# Initialize a cluster (on master node)
kubeadm init --apiserver-advertise-address $(hostname -i) --pod-network-cidr 10.5.0.0/16

# After init, set up kubectl access
mkdir -p $HOME/.kube
cp -i /etc/kubernetes/admin.conf $HOME/.kube/config

# Install a network plugin
# Note: Tried Weave Net (failed), switched to Flannel (worked)
# PWK officially recommends kube-router - discovered after the fact
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml

# Wait for network plugin to be ready
kubectl get pods -n kube-system -w  # Watch until all pods are Running

# Explore the cluster
kubectl get nodes                 # List nodes in cluster
kubectl get pods -A               # All pods in all namespaces
kubectl get services -A           # All services
kubectl cluster-info              # Cluster information
kubectl config view               # View kubeconfig

# Check system pods
kubectl get pods -n kube-system   # K8s system components
kubectl describe node node1       # Detailed node information
```

#### **5. Create Your First Pod** 🎯

```bash
# Simple nginx pod
kubectl run mynginx --image=nginx --port=80

# Watch it come up
kubectl get pods -w               # -w for watch mode (Ctrl+C to exit)

# Get details
kubectl describe pod mynginx      # Detailed pod information
kubectl logs mynginx              # Pod logs

# Execute commands in the pod
kubectl exec mynginx -- nginx -v  # Run command in container
kubectl exec -it mynginx -- sh    # Interactive shell in container
# (type 'exit' to leave)

# Expose the pod as a service
kubectl expose pod mynginx --port=80 --type=NodePort

# Get the service details
kubectl get svc mynginx           # Note the NodePort (30000-32767)

# Test it (from the node)
curl localhost:<NodePort>         # Should see nginx welcome page
```

**🔐 Security Discovery: Service Account Token Injection**

When you run `kubectl describe pod mynginx`, you'll see:

```yaml
Mounts:
  /var/run/secrets/kubernetes.io/serviceaccount from kube-api-access-h7mzb (ro)
```

**What is this?** Kubernetes **automatically** injects credentials into EVERY pod!

**Why?** So pods can talk to the Kubernetes API server (for monitoring tools, operators, etc.)

**What's inside that directory?**

```bash
# Enter the pod and explore
kubectl exec -it mynginx -- sh

# Inside the pod:
ls -la /var/run/secrets/kubernetes.io/serviceaccount/
# ca.crt       → Certificate Authority (verify API server identity)
# namespace    → Which namespace this pod is in (default)
# token        → JWT authentication token

# View the token
cat /var/run/secrets/kubernetes.io/serviceaccount/token
# Long JWT token: eyJhbGciOiJSUzI1NiIs...

# This token allows the pod to authenticate to Kubernetes API!
```

**🚨 Security Implication: Every Pod Can Talk to Kubernetes!**

```bash
# From inside the pod (after installing curl):
apt-get update && apt-get install -y curl

TOKEN=$(cat /var/run/secrets/kubernetes.io/serviceaccount/token)
APISERVER=https://kubernetes.default.svc

# Your nginx pod can query Kubernetes API!
curl -k $APISERVER/api/v1/namespaces/default/pods \
  --header "Authorization: Bearer $TOKEN"

# Output: List of all pods in default namespace!
# 😱 If nginx gets hacked, attacker can explore your cluster!
```

**🛡️ Best Practice: Disable Token Mounting (If Not Needed)**

```bash
# Create pod WITHOUT service account token
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: secure-nginx
spec:
  automountServiceAccountToken: false  # 🔒 No API access!
  containers:
  - name: nginx
    image: nginx
EOF

# Verify no token is mounted
kubectl describe pod secure-nginx | grep -A 5 "Mounts:"
# Output: No serviceaccount mount! ✅

# Try to access from inside
kubectl exec -it secure-nginx -- sh
ls /var/run/secrets/kubernetes.io/
# Output: Directory doesn't exist! Pod is isolated. 🔒
```

**📚 When to Disable Service Account Tokens:**

| Pod Type | Token Needed? | Reasoning |
|----------|---------------|-----------|
| **Nginx/web server** | ❌ NO | Just serves HTTP, no K8s interaction |
| **Application backend** | ❌ NO | Talks to database, not K8s API |
| **Monitoring tool** (Prometheus) | ✅ YES | Needs to query pod metrics |
| **CI/CD runner** (Jenkins) | ✅ YES | Deploys apps by creating pods |
| **Service mesh** (Istio) | ✅ YES | Discovers services dynamically |

**🎯 Security Principle: Least Privilege**
- If pod doesn't need API access → `automountServiceAccountToken: false`
- If pod needs API access → Create custom ServiceAccount with RBAC (not default SA!)

**Real-World Attack Scenario:**
```
1. Attacker exploits nginx CVE → Gains shell inside mynginx pod
2. Attacker finds token → cat /var/run/secrets/.../token
3. Attacker queries API → Lists all secrets in namespace
4. Attacker steals DB password → curl $APISERVER/api/v1/.../secrets
5. Attacker pivots to database → Full cluster compromise! 💥

Defense: automountServiceAccountToken: false
→ Attacker stuck inside container, can't explore cluster! 🛡️
```

#### **6. YAML Adventures** 📝

```bash
# Create a deployment YAML
cat <<EOF > my-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-nginx
  labels:
    app: nginx
spec:
  replicas: 3
  selector:
    matchLabels:
      app: nginx
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:latest
        ports:
        - containerPort: 80
EOF

# Apply it
kubectl apply -f my-deployment.yaml

# Watch the magic happen
kubectl get deployments
kubectl get pods -l app=nginx     # Pods with label app=nginx
kubectl get pods -o wide          # See which nodes they're on

# Scale it up!
kubectl scale deployment my-nginx --replicas=5

# Scale it down
kubectl scale deployment my-nginx --replicas=2

# Rolling update (change image)
kubectl set image deployment/my-nginx nginx=nginx:alpine

# Watch the rollout
kubectl rollout status deployment/my-nginx

# Rollback if needed
kubectl rollout undo deployment/my-nginx
```

#### **7. Security Exploration** 🔐

```bash
# Check RBAC (Role-Based Access Control)
kubectl get clusterroles          # Cluster-wide roles
kubectl get clusterrolebindings   # Who has which roles
kubectl auth can-i create pods    # Can I create pods? (as current user)
kubectl auth can-i --list         # What can I do?

# Service accounts
kubectl get serviceaccounts -A    # All service accounts
kubectl get secrets -A            # K8s secrets (tokens, etc.)

# Security contexts
kubectl explain pod.spec.securityContext
kubectl explain pod.spec.containers.securityContext

# Create a pod with security context
cat <<EOF > secure-pod.yaml
apiVersion: v1
kind: Pod
metadata:
  name: security-test
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 1000
    fsGroup: 2000
  containers:
  - name: sec-test
    image: alpine
    command: ["sleep", "3600"]
    securityContext:
      allowPrivilegeEscalation: false
      readOnlyRootFilesystem: true
      capabilities:
        drop:
          - ALL
EOF

kubectl apply -f secure-pod.yaml
kubectl exec security-test -- id  # Check user ID
```

#### **8. Troubleshooting & Debugging** 🐛

```bash
# Pod troubleshooting
kubectl get events                # Recent cluster events
kubectl get pods --show-labels    # See pod labels
kubectl describe pod <pod-name>   # Why is my pod not working?

# Resource usage
kubectl top nodes                 # Node CPU/memory (needs metrics-server)
kubectl top pods                  # Pod resource usage

# Logs
kubectl logs <pod-name>           # Pod logs
kubectl logs <pod-name> -f        # Follow logs (like tail -f)
kubectl logs <pod-name> --previous # Logs from previous container instance

# Port forwarding (access pod from your browser)
kubectl port-forward pod/mynginx 8080:80
# Then visit localhost:8080 (in PWK, use the port in their UI)

# Copy files to/from pods
kubectl cp mynginx:/etc/nginx/nginx.conf ./nginx.conf
kubectl cp ./myfile.txt mynginx:/tmp/
```

#### **9. Clean Up & Experimenting** 🧹

```bash
# Delete resources
kubectl delete pod mynginx
kubectl delete deployment my-nginx
kubectl delete -f my-deployment.yaml

# Delete everything with a label
kubectl delete pods -l app=nginx

# Nuclear option (delete everything in default namespace)
kubectl delete all --all

# Check what's left
kubectl get all
```

#### **10. Advanced: Kubernetes Internals** 🔬

```bash
# etcd exploration (K8s database)
ETCDCTL_API=3 etcdctl --endpoints=https://127.0.0.1:2379 \
  --cacert=/etc/kubernetes/pki/etcd/ca.crt \
  --cert=/etc/kubernetes/pki/etcd/server.crt \
  --key=/etc/kubernetes/pki/etcd/server.key \
  get / --prefix --keys-only

# View cluster certificates
ls -la /etc/kubernetes/pki/       # All PKI certificates
openssl x509 -in /etc/kubernetes/pki/apiserver.crt -text -noout

# Check kubelet configuration
systemctl status kubelet          # Is kubelet running?
journalctl -u kubelet -f          # Kubelet logs

# API server direct access
curl -k https://localhost:6443/   # Without auth (will fail)
curl -k https://localhost:6443/healthz  # Health check

# Container runtime (containerd or Docker)
crictl ps                         # List containers (if using containerd)
crictl images                     # List images
docker ps                         # If using Docker
```

---

### 🎯 Learning Challenges

**Beginner:**
1. ✅ Create a pod running nginx
2. ✅ Expose it as a service
3. ✅ Access the nginx welcome page
4. ✅ Check the pod logs

**Intermediate:**
1. 🎯 Create a deployment with 3 replicas
2. 🎯 Scale it up and down
3. 🎯 Perform a rolling update
4. 🎯 Create a pod with security contexts (non-root user)

**Advanced:**
1. 🚀 Set up a multi-node cluster (add worker nodes)
2. 🚀 Deploy a stateful application with persistent volumes
3. 🚀 Create network policies to restrict pod communication
4. 🚀 Set up ingress controller for HTTP routing

---

### 💡 What I Learned

**Technical Skills:**
- CentOS/RPM system administration
- Running as root (privileged operations)
- Kubernetes cluster initialization
- Docker container basics
- K8s networking and services
- kubectl command mastery

**Security Insights:**
- 🔴 Root access = powerful but dangerous
- 🔐 K8s uses RBAC for fine-grained permissions
- 🌐 Network policies control pod communication
- 🛡️ Security contexts limit container privileges
- 📜 Secrets management for sensitive data

**Key Takeaways:**
1. **Play with Kubernetes is powerful** - Real K8s cluster in browser!
2. **4-hour time limit** - Sessions expire, so document findings
3. **No persistence** - Great for learning, not for long-term projects
4. **Root access teaching moment** - Shows why RBAC matters in production
5. **Hands-on beats theory** - Even simple explorations teach a lot!

---

## 🎓 Session Outcome: Day 41 Complete! ✅

**Date Completed:** October 26, 2025 (Session #21)  
**Learning Mode:** Theory + PWK Exploration  
**Cost Spent:** €0 (Perfect!)  

### What I Accomplished:
✅ **Comprehensive Kubernetes Theory** - Orchestration, architecture, security layers
✅ **AKS Understanding** - Managed K8s, cost analysis, when to use vs ACI/VMs
✅ **Security Deep Dive** - RBAC, Network Policies, Pod Security, Azure integrations
✅ **PWK Hands-On Exploration** - System reconnaissance, network analysis, security discoveries
✅ **Service Account Token Injection** - Discovered automatic credential mounting, learned mitigation
✅ **Network Security Analysis** - IPv6 notation, port 2375 danger, Kubernetes port mapping
✅ **Interview Preparation** - 8 comprehensive Q&A scenarios ready
✅ **Documentation** - 1,900+ lines of knowledge captured

### Key Discoveries:
🔍 **System:** CentOS, RPM package manager, running as root (teaching moment)
🔐 **Security:** Docker API exposed on 2375 (no TLS), service account tokens auto-injected
📊 **Architecture:** Control plane components, node structure, pod networking
💰 **Cost Reality:** AKS minimum €238/month (validated decision to skip deployment)
🎯 **Network Plugin:** Learned importance of CNI (tried Weave Net, switched to Flannel)

### Next Session Preview:
📅 **Day 43: Local Kubernetes Hands-On Practice**
- Install k3s on Parrot machine (lightweight K8s)
- Deploy secure-app to local cluster
- Practice kubectl, YAML manifests, security policies
- Implement Pod Security Standards locally
- **Cost:** €0 (local only)
- **Outcome:** Practical K8s experience without Azure bills

### Personal Reflection:
💪 **Feeling empowered by exploration sessions!**
- Theory provides foundation, hands-on reveals reality
- Small discoveries (IPv6 notation, token injection) = big security insights
- €0 budget constraint drives creative learning approaches
- Documentation captures knowledge for future reference

---

**Created:** October 25, 2025 (Session #21)  
**Completed:** October 26, 2025  
**Author:** YomHub (Azure Security Journey)  
**Purpose:** Understand AKS and Kubernetes without spending €238/month  
**Status:** ✅ COMPLETE - Theory mastered, ready for Day 43 practice!  
**Next:** Day 43 - Local Kubernetes hands-on practice 🚀
