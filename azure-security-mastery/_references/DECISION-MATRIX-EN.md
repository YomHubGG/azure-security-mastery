# 🎯 Cloud Service Decision Matrix - When to Use What?

**Last Updated:** October 25, 2025  
**Language:** English  
**Purpose:** Make the right architectural decisions (AKS vs ACI vs VMs vs Functions)

---

## 📊 Quick Decision Matrix

| Your Need | Use This | Monthly Cost | Why |
|-----------|----------|--------------|-----|
| **1-5 containers, simple app** | ACI | €10-30 | No orchestration needed |
| **6-20 containers, growing** | ACI + Load Balancer | €50-100 | Still manageable manually |
| **21+ containers, complex** | AKS | €238+ | Need orchestration |
| **Batch jobs (5-10 min)** | ACI or Functions | €2-5 | Pay only for execution time |
| **24/7 web server** | VM or App Service | €15-50 | More cost-effective than ACI |
| **Event-driven tasks** | Azure Functions | €0-10 | Serverless, auto-scale |
| **Database server** | VM + Managed Disk | €25-60 | Need persistence |
| **Learning/Testing** | Local (k3s/minikube) | €0 | No Azure costs! |

---

## 🔍 Detailed Comparison

### 1️⃣ Azure Container Instances (ACI)

**What It Is:**
- Serverless containers
- Deploy single containers or container groups
- Per-second billing
- No cluster management

**Pricing:**
```
vCPU: €0.0000133/second (~€34/month for 1 vCPU continuous)
Memory: €0.0000015/GB/second (~€4/month for 1GB continuous)
Free tier: 50,000 vCPU-seconds/month (~14 hours at 1 vCPU)
```

**✅ Best For:**
- 1-10 containers
- Short-lived workloads (minutes to hours)
- CI/CD build agents
- Batch processing
- Testing and development
- Simple architectures

**❌ Avoid For:**
- 20+ containers (too complex to manage)
- Need auto-scaling policies
- Need service mesh (mTLS)
- Complex networking requirements
- Need Kubernetes knowledge/skills

**Real Cost Examples:**
```
Your secure-app (0.5 CPU, 0.5 GB):
├─ 7 minutes testing: €0.00 (within free tier) ✅
├─ 1 hour per day: €0.50/month ✅
├─ 24/7: €17/month ⚠️ (VM cheaper!)
```

### 2️⃣ Azure Kubernetes Service (AKS)

**What It Is:**
- Managed Kubernetes cluster
- Orchestrates 100s of containers
- Auto-scaling, rolling updates, self-healing
- Enterprise-grade features

**Pricing:**
```
Control plane: €0 (Microsoft pays!)
Worker nodes: €70/month each (D2s_v3: 2 vCPU, 8GB RAM)
Load Balancer: €15/month
Storage: €5-15/month
Minimum: €238/month (3-node cluster)
```

**✅ Best For:**
- 20+ containers
- Microservices architecture
- Need auto-scaling
- Zero-downtime deployments
- Service mesh requirements
- Multi-environment (dev/staging/prod)
- Learning Kubernetes skills

**❌ Avoid For:**
- 1-10 containers (overkill)
- Simple architectures
- Budget <€200/month
- Don't need orchestration features
- Learning phase (use local K8s instead)

**Real Cost Examples:**
```
Small AKS (3 nodes):
├─ Worker nodes (3x D2s_v3): €210/month
├─ Load Balancer: €15/month
├─ Storage: €10/month
├─ Total: €235/month

Large AKS (10 nodes):
├─ Worker nodes (10x D2s_v3): €700/month
├─ Load Balancer: €15/month
├─ Storage: €30/month
├─ Total: €745/month
```

### 3️⃣ Virtual Machines (VMs)

**What It Is:**
- Full virtual computer
- Complete OS control
- Persistent storage
- Traditional approach

**Pricing:**
```
B1s (1 vCPU, 1GB): €7.50/month
B2s (2 vCPU, 4GB): €30/month
D2s_v3 (2 vCPU, 8GB): €70/month
+ Managed Disk (30GB SSD): €4/month
+ Public IP (static): €3/month
```

**✅ Best For:**
- 24/7 services (web servers, APIs)
- Databases (need persistence)
- Legacy applications
- Need full OS control
- Complex software installation
- Predictable monthly costs

**❌ Avoid For:**
- Short-lived workloads (pay 24/7 even when idle)
- Need frequent scaling
- Stateless applications
- Container-first architectures

**Real Cost Examples:**
```
Single VM (B1s + Disk + IP):
├─ VM: €7.50/month
├─ Disk: €4/month
├─ IP: €3/month
├─ Total: €14.50/month ✅ Cheapest for 24/7!

3 VMs (for HA):
├─ VMs: €22.50/month
├─ Disks: €12/month
├─ IPs: €9/month
├─ Load Balancer: €15/month
├─ Total: €58.50/month
```

### 4️⃣ Azure Functions (Serverless)

**What It Is:**
- Event-driven code execution
- No servers to manage
- Pay per execution
- Auto-scales infinitely

**Pricing:**
```
Free tier: 1 million executions/month + 400,000 GB-seconds
After: €0.000014/execution + €0.000014/GB-second
```

**✅ Best For:**
- Event-driven tasks (file upload → process)
- API backends (simple endpoints)
- Scheduled tasks (cron jobs)
- Webhooks
- IoT data processing
- Truly serverless needs

**❌ Avoid For:**
- Long-running tasks (>10 minutes)
- Complex state management
- Container-based workflows
- Learning full-stack development

**Real Cost Examples:**
```
Small workload (10,000 executions/day):
├─ 300,000/month executions
├─ Cost: €0.00 (within free tier) ✅

Medium workload (100,000 executions/day):
├─ 3,000,000/month executions
├─ Cost: ~€28/month
```

### 5️⃣ Azure App Service (PaaS)

**What It Is:**
- Platform for web apps
- No infrastructure management
- Built-in scaling, deployment slots
- Supports Node.js, Python, .NET, PHP, Java

**Pricing:**
```
Free tier: €0 (limited, no custom domain)
B1 (Basic): €12/month
S1 (Standard): €60/month
P1v2 (Premium): €140/month
```

**✅ Best For:**
- Web apps without containers
- .NET or Java applications
- Need deployment slots (blue/green)
- Simple PaaS needs
- CI/CD integration

**❌ Avoid For:**
- Container-first architecture (use ACI/AKS)
- Complex microservices
- Need Kubernetes
- Learning modern DevOps (containers better)

---

## 🎯 Real-World Scenarios

### Scenario 1: Your Secure-App (Single Container)

**App:** Node.js web server (Day 35 container)

**Options:**

| Service | Monthly Cost | Pros | Cons | Verdict |
|---------|--------------|------|------|---------|
| **ACI** | €17 (24/7) | Simple, no management | No persistence | ✅ **BEST** |
| **VM B1s** | €15 (24/7) | Persistent disk, full control | Manage OS updates | ✅ Also good |
| **AKS** | €238 | Overkill for 1 container | Too expensive | ❌ No |
| **Functions** | Can't host web server | - | Doesn't fit | ❌ No |
| **App Service** | €12 (F1 free tier) | Easy deployment | Less control than container | ⚠️ OK |

**WINNER: ACI** for containerized single app, or **App Service Free Tier** if not using containers.

---

### Scenario 2: Startup with 15 Microservices

**App:** E-commerce platform
- 3 Frontend services (React)
- 8 Backend APIs (Node.js)
- 2 Background workers
- 2 Databases (PostgreSQL, Redis)

**Options:**

| Service | Monthly Cost | Complexity | Scalability | Verdict |
|---------|--------------|------------|-------------|---------|
| **15 ACI containers** | ~€255 | High (manage 15 individually) | Manual | ❌ Hard to manage |
| **AKS** | €238 | Medium (K8s learning curve) | Automatic | ✅ **BEST** |
| **5 VMs + Docker** | ~€75 | Medium (Docker Compose) | Manual | ⚠️ Budget option |
| **App Service (15 apps)** | ~€180 | Low | Manual | ⚠️ OK but expensive |

**WINNER: AKS** - At 15 containers, orchestration pays for itself in time saved and auto-scaling.

---

### Scenario 3: Batch Image Processing

**App:** Process user-uploaded images
- 1,000 images per day
- Each takes 5 minutes to process
- Only runs when images uploaded

**Options:**

| Service | Daily Runtime | Monthly Cost | Verdict |
|---------|---------------|--------------|---------|
| **ACI** | 5,000 minutes | ~€10 (pay only for usage) | ✅ **BEST** |
| **VM (always on)** | 24/7 | €15 | ❌ Waste (idle 95% of time) |
| **AKS** | - | €238 | ❌ Massive overkill |
| **Azure Functions** | 5,000 executions | ~€5 | ✅ Also great |

**WINNER: Azure Functions** or **ACI** - Both serverless, pay-per-use is perfect here.

---

### Scenario 4: Black Friday Traffic Spike

**App:** E-commerce site
- Normal: 10 containers (100 req/sec)
- Black Friday: Need 100 containers (1,000 req/sec)
- Spike lasts 4 hours

**Options:**

| Service | Cost (Normal) | Cost (Peak 4h) | Scaling | Verdict |
|---------|---------------|----------------|---------|---------|
| **ACI** | €170/month | +€280 for 4h | Manual (slow) | ❌ Hard to scale fast |
| **AKS** | €238/month | €238 (auto-scales) | Automatic (fast!) | ✅ **BEST** |
| **VMs** | Keep 100 VMs? | €1,500/month | Manual | ❌ Way too expensive |

**WINNER: AKS** - Horizontal Pod Autoscaler handles traffic spikes automatically.

---

### Scenario 5: Learning Kubernetes

**Goal:** Learn K8s without Azure costs

**Options:**

| Service | Cost | Learning Value | Verdict |
|---------|------|----------------|---------|
| **AKS** | €238/month | High (real Azure) | ❌ Too expensive |
| **Local k3s** | €0 | High (95% same as AKS) | ✅ **BEST** |
| **Minikube** | €0 | High (100% real K8s) | ✅ **BEST** |
| **Kind** | €0 | High (K8s in Docker) | ✅ Also great |
| **AKS (1h test)** | ~€0.30 | High but stressful | ⚠️ Risky (might forget to delete) |

**WINNER: Local k3s or minikube** - Learn K8s for €0, deploy to AKS later when you have a job! 🎯

---

## 📈 Cost Comparison Chart

**Scenario: Running 10 Containers 24/7 for 1 Month**

```
Service         | Cost/Month | Management Effort | Auto-Scale | HA
----------------|------------|-------------------|------------|----
ACI (10 × 0.5)  | €170       | High (10 manual)  | No         | Manual
AKS (3 nodes)   | €238       | Medium (K8s)      | Yes        | Built-in
VMs (3 × B2s)   | €90        | High (SSH, patch) | No         | Manual
Functions       | N/A        | Low               | Yes        | Built-in
```

**Verdict:**
- Budget <€100: **VMs with Docker Compose**
- Budget €100-200: **ACI** if simple, **VMs** if need persistence
- Budget >€200: **AKS** (orchestration worth it)

---

## 🎯 Decision Tree

```
START: I need to run containers

├─ How many containers?
│  ├─ 1-5 containers
│  │  ├─ Runs 24/7?
│  │  │  ├─ YES → ACI or VM (similar cost ~€15-30/mo)
│  │  │  └─ NO (short-lived) → ACI or Functions (pay-per-use)
│  │  └─ Learning only? → Local Docker (€0)
│  │
│  ├─ 6-20 containers
│  │  ├─ Budget >€200/month?
│  │  │  ├─ YES → AKS (worth the orchestration)
│  │  │  └─ NO → VMs + Docker Compose (€50-100/mo)
│  │  └─ Complex networking/scaling?
│  │     ├─ YES → AKS (need K8s features)
│  │     └─ NO → ACI with Load Balancer
│  │
│  └─ 21+ containers
│     ├─ Budget >€200/month?
│     │  ├─ YES → AKS (mandatory at this scale)
│     │  └─ NO → Multi-VM Docker Swarm (€100-150/mo)
│     └─ Learning? → Local k3s (€0)

├─ Need Kubernetes skills?
│  ├─ YES → Local k3s/minikube (free learning)
│  └─ NO → Use simpler options (ACI, App Service)

├─ Need auto-scaling?
│  ├─ YES → AKS or Functions
│  └─ NO → ACI or VMs

├─ Need zero-downtime updates?
│  ├─ YES → AKS (rolling updates) or Functions
│  └─ NO → ACI or VMs (manual blue/green)

└─ Budget constraint?
   ├─ €0-20/month → Local + Functions (free tier)
   ├─ €20-100/month → ACI or VMs
   ├─ €100-200/month → ACI + LB or VMs
   └─ €200+/month → AKS (full orchestration)
```

---

## 💡 Pro Tips

### When ACI is Perfect:
✅ Your container from Day 35 (single app)  
✅ CI/CD build agents (run for 10 minutes, destroy)  
✅ Batch processing (run job, shut down)  
✅ Testing before committing to AKS  
✅ Side projects with low traffic

### When AKS is Worth It:
✅ 20+ microservices  
✅ Need auto-scaling policies  
✅ Zero-downtime deployments critical  
✅ Service mesh (mTLS between services)  
✅ Building Kubernetes resume skills  
✅ Enterprise production workloads

### When VMs are Better:
✅ Databases (need persistent storage)  
✅ Legacy apps (can't containerize)  
✅ 24/7 simple services (cheaper than ACI)  
✅ Need full OS control  
✅ Predictable monthly budget

### When Functions Win:
✅ Event-driven (file upload triggers processing)  
✅ Webhooks (receive HTTP, process, respond)  
✅ Scheduled tasks (cron jobs)  
✅ True serverless needs  
✅ Budget = €0 (free tier generous)

---

## 🔢 Cost Calculator

**Your Workload:**
- Containers: _____ (number)
- Runtime: _____% (percentage of month running)
- CPU per container: _____
- Memory per container: _____

**Estimated Costs:**

```
ACI Cost:
= (Containers × CPU × 0.0000133 × 2,592,000 × Runtime%) 
+ (Containers × Memory × 0.0000015 × 2,592,000 × Runtime%)

Example: 5 containers, 0.5 CPU, 0.5 GB, 50% runtime
= (5 × 0.5 × 0.0000133 × 2,592,000 × 0.5)
+ (5 × 0.5 × 0.0000015 × 2,592,000 × 0.5)
= €43 + €4.86
= €48/month

AKS Cost (3 nodes):
= €238/month (flat, regardless of container count up to node capacity)

VM Cost:
= (VMs × VM_price) + (Disks × €4) + (IPs × €3)

Example: 3 VMs (B1s), 3 disks, 1 public IP
= (3 × €7.50) + (3 × €4) + (1 × €3)
= €22.50 + €12 + €3
= €37.50/month
```

---

## 🎓 For Your Portfolio/Interviews

**Show You Understand Trade-offs:**

> "For my secure-app project, I deployed to Azure Container Instances because it's a single container running 24/7. ACI costs €17/month which is comparable to a VM, but simpler to manage. I considered AKS but at €238/month minimum, it's overkill for one container. If I were running 20+ microservices, AKS would make sense for auto-scaling and orchestration. For learning Kubernetes, I practiced locally with k3s to avoid costs."

**This shows:**
- ✅ You understand cost implications
- ✅ You make architectural decisions based on requirements
- ✅ You know when NOT to over-engineer
- ✅ You're budget-conscious (valuable in any company!)

---

## 📚 Related Guides

- [Day 39: Azure Container Instances](../02-security-services/day39-container-instances/README.md)
- [Day 41: AKS & Kubernetes Theory](../02-security-services/day41-aks-kubernetes-theory/README.md)
- [COST-REALITY.md](../02-security-services/day39-container-instances/COST-REALITY.md)
- [ACRONYM-DECODER.md](./ACRONYM-DECODER.md)

---

**Created:** October 25, 2025  
**Author:** YomHub  
**Purpose:** Make smart cloud architecture decisions  
**Language:** English (French version: DECISION-MATRIX-FR.md)  
**Usage:** Architecture planning, interview prep, cost optimization
