# 💰 Azure Container Services: The REAL Cost Picture

*Understanding when services are actually "free" vs marketing claims*

---

## The Confusion: Why is ACI "free" but ACR/VMs cost money?

This document explains the **actual costs** behind Azure container services and when each service makes financial sense.

---

## 1️⃣ **AZURE CONTAINER INSTANCES (ACI)** - What we just used

**Free Tier:** 50,000 vCPU-seconds/month (first 1 million seconds free)  
**After Free:** €0.0000133/vCPU-second

### Reality Check:
- 50,000 seconds = **13.8 hours at 1 vCPU**
- Your 0.5 vCPU × 7 min = 210 seconds = **€0.00** ✅
- **BUT:** 24/7 at 0.5 CPU = 1,296,000 seconds/month = **€17.24!** 💸

### 🎯 THE TRICK
**It's only "free" for SHORT-LIVED workloads!**

If you run continuously → Same price as a small VM!

### Cost Calculation Formula:
```
Monthly Cost = (CPU cores) × (seconds running) × €0.0000133
Example: 0.5 CPU × 2,592,000 seconds (30 days) = €17.24
```

---

## 2️⃣ **AZURE CONTAINER REGISTRY (ACR)** - Storage for images

### ❌ NO FREE TIER (that's why we avoided it!)

| Tier | Monthly Cost | Storage | Features |
|------|--------------|---------|----------|
| **Basic** | €4.60 | 10GB | Storage only |
| **Standard** | €18.40 | 100GB | Webhooks, geo-replication |
| **Premium** | €92.00 | 500GB | Content trust, private link, zone redundancy |

### Reality Check:
- Just **storing** your 354MB image = **€4.60/month minimum**
- Even if you **NEVER pull/push** after initial upload!
- It's a 24/7 managed service (always running = always billing)
- You pay for storage, bandwidth, and management

### 🎯 WHY IT COSTS
- **Storage:** Blob storage + metadata + indexing
- **Managed Service:** High availability, automatic backups
- **Bandwidth:** Data transfer costs for pulls/pushes
- **Security:** Scanning, access control, audit logs

### 💡 Why GitHub CR is FREE:
Microsoft wants to encourage adoption. It's a loss leader to attract developers to the Azure ecosystem. Once you're comfortable with containers on Azure, they hope you'll use their paid services (ACR, AKS, etc.).

---

## 3️⃣ **VIRTUAL MACHINES (VMs)** - Traditional compute

### ❌ NO REAL FREE TIER (12 months promotional only)

| Size | vCPU | RAM | Monthly Cost (24/7) | Best For |
|------|------|-----|---------------------|----------|
| **B1s** | 1 | 1GB | ~€7.50 | Light workloads |
| **B2s** | 2 | 4GB | ~€30.00 | Production apps |
| **D2s_v3** | 2 | 8GB | ~€70.00 | High performance |

### Reality Check:
- VM = **Always running = Always billing**
- Even when **idle** = Still paying for allocated resources
- Storage costs extra (~€3-5/month for 30GB managed disk)
- Public IP costs ~€3/month (static)
- Bandwidth: First 100GB free, then ~€0.08/GB

### Total Example (B1s VM):
```
VM compute:      €7.50/month
Managed disk:    €4.00/month (30GB SSD)
Static IP:       €3.00/month
Total:          €14.50/month minimum
```

### 🎯 WHY IT COSTS
- **24/7 Compute Reservation:** You reserve the resources even when not using them
- **Storage:** Persistent disks (SSDs)
- **Networking:** Public IP, bandwidth
- **Management Overhead:** OS updates, patching, security

---

## 📊 Side-by-Side Comparison

### Scenario: Hosting secure-app 24/7 for 1 month

| Service | Setup Cost | Monthly Cost | Total Year 1 | When to Use |
|---------|------------|--------------|--------------|-------------|
| **ACI (0.5 CPU)** | €0 | ~€17/month | €204 | 🎯 Short tasks, testing, burst workloads |
| **VM B1s** | €0 | ~€10/month* | €120 | ✅ Always-on services, predictable costs |
| **ACR Basic** | €0 | €4.60/month | €55.20 | ❌ Use GitHub CR for learning instead |
| **GitHub CR** | €0 | €0/month | €0 | ✅ **BEST for learning and personal projects!** |

*\*Note: VM cost includes compute only. Add ~€7/month for disk + IP = €17/month total (same as ACI!)*

---

## 💡 The Truth About ACI

### ✅ ACI Wins When:

1. **Batch Jobs**
   - Run for 5 minutes → Process data → Shut down
   - Cost: ~€0.02 per job (essentially free)
   - Example: Image processing, data transformation

2. **Testing/Demos** (like we did today!)
   - 7 minutes runtime = €0 (within free tier)
   - Deploy → Test → Delete
   - Perfect for learning and portfolio demos

3. **Event-Driven Tasks**
   - Triggered by Azure Functions or Logic Apps
   - Process event → Shut down immediately
   - Only pay for actual processing time

4. **Burst Workloads**
   - Scale 0 → 100 containers → 0 in minutes
   - Handle traffic spikes without permanent infrastructure
   - Example: Black Friday sale processing

5. **CI/CD Pipelines**
   - Spin up test environment
   - Run tests (5-10 minutes)
   - Delete environment
   - Cost: Nearly free

### ✅ VM Wins When:

1. **Running 24/7 Services**
   - Web servers, APIs, databases
   - Predictable monthly cost (~€10-17/month)
   - More cost-effective than ACI for continuous use

2. **Persistent Storage Needs**
   - Databases requiring local storage
   - File systems that survive restarts
   - Stateful applications

3. **Complex Networking**
   - Multiple NICs
   - Custom routing
   - VPN connectivity

4. **Cost Predictability**
   - Fixed monthly cost regardless of usage
   - Easier to budget
   - No surprise bills from long-running processes

5. **Full OS Control**
   - Custom kernel modules
   - System-level software
   - Root access for everything

---

## 🎯 Why We Used Each Service

### Day 35: **Podman (Local)**
- **Cost:** €0 forever
- **Why:** Learn Docker/container concepts without any Azure costs
- **Outcome:** Built secure container with 10+ hardening measures

### Day 37: **GitHub Container Registry**
- **Cost:** €0 forever (unlimited public containers)
- **Why:** Avoid ACR's €4.60/month minimum
- **Outcome:** Published ghcr.io/yomhubgg/secure-app:1.0.0

### Day 39: **Azure Container Instances**
- **Cost:** €0 (7 minutes = 210 vCPU-seconds, within 50,000 free tier)
- **Why:** Learn Azure deployment without VM management overhead
- **Outcome:** Real cloud deployment, tested, monitored, deleted

### ❌ What We AVOIDED: **Azure Container Registry**
- **Would Cost:** €4.60/month minimum (€55.20/year)
- **Why Avoided:** GitHub CR does the same job for free
- **Savings:** €55.20/year by making smart technology choices!

---

## 🧮 Cost Calculation Examples

### Example 1: Daily 10-minute Test Runs (30 days)
```
Service: ACI (0.5 vCPU)
Runtime: 10 minutes/day × 30 days = 300 minutes = 18,000 seconds
vCPU-seconds: 0.5 × 18,000 = 9,000 seconds
Cost: FREE (within 50,000 free tier)
```

### Example 2: Continuous Web Server (24/7)
```
Service: ACI (0.5 vCPU)
Runtime: 30 days × 24 hours × 60 min × 60 sec = 2,592,000 seconds
vCPU-seconds: 0.5 × 2,592,000 = 1,296,000 seconds
Cost: (1,296,000 - 50,000) × €0.0000133 = €16.57/month

Compare to VM B1s (1 vCPU, 1GB RAM):
VM Cost: €7.50/month + €4 disk + €3 IP = €14.50/month
Winner: VM is slightly cheaper AND has persistent storage!
```

### Example 3: Burst Traffic (Black Friday)
```
Service: ACI (100 containers × 1 vCPU each)
Runtime: 4 hours peak traffic
vCPU-seconds: 100 × 1 × (4 × 3600) = 1,440,000 seconds
Cost: (1,440,000 - 50,000) × €0.0000133 = €18.50

Compare to: 
- Keeping 100 VMs ready 24/7: €750/month 💸
- ACI advantage: Only pay for 4 hours, not 720 hours!
Winner: ACI saves €731.50 for this use case! 🎯
```

---

## 📚 Key Takeaways

### 1. **"Free" is Relative**
- ACI free tier = 50,000 vCPU-seconds = ~14 hours at 1 vCPU
- Great for learning and testing
- NOT suitable for running services 24/7

### 2. **Per-Second Billing is a Double-Edged Sword**
- ✅ Amazing for short tasks (pay only for what you use)
- ❌ Expensive for long-running workloads (exceeds VM costs)

### 3. **Choose Based on Usage Pattern**
- **Intermittent/Short:** ACI (batch jobs, CI/CD, testing)
- **Continuous/24/7:** VM (web servers, databases, APIs)
- **Storage:** GitHub CR for free, ACR only when you need enterprise features

### 4. **Learning Strategy**
- Use free tiers for actual deployment practice
- Avoid paid services you can learn about theoretically
- Document your understanding (proves knowledge without spending)

### 5. **Cost Consciousness = Professional Skill**
- Knowing when to use which service shows architectural maturity
- €0 spent across 39 days proves you understand cloud economics
- This knowledge is valuable in job interviews

---

## 🎓 Decision Framework

Use this flowchart to decide which service to use:

```
Is this for learning/portfolio?
├─ YES → Use free options (GitHub CR, ACI free tier, local Podman)
└─ NO → Continue...

Will it run 24/7?
├─ YES → Use VM (more cost-effective for continuous workloads)
└─ NO → Continue...

Will it run for minutes/hours then stop?
├─ YES → Use ACI (per-second billing wins)
└─ NO → Continue...

Do you need persistent storage?
├─ YES → Use VM with managed disks
└─ NO → ACI is fine for stateless workloads

Do you need container image storage?
├─ Personal/Learning → GitHub Container Registry (free)
└─ Enterprise → Azure Container Registry (paid, more features)
```

---

## 💼 Portfolio Value

**What This Knowledge Demonstrates:**

1. **Cloud Economics Understanding**
   - You know Azure pricing models
   - You can optimize costs intelligently
   - You've proven €0 spending across 3 container days

2. **Architectural Decision-Making**
   - Chose GitHub CR over ACR (saved €55/year)
   - Used ACI for learning/testing (appropriate use case)
   - Documented why VMs better for 24/7 workloads

3. **Professional Maturity**
   - Didn't just blindly use "official" Azure services
   - Made cost-conscious technology choices
   - Can explain tradeoffs to stakeholders

4. **Real-World Experience**
   - Deployed actual container to production cloud
   - Monitored and managed resources
   - Cleaned up properly (cost discipline)

---

## 🔗 Related Documentation

- [Day 35: Docker Security](/azure-security-mastery/02-security-services/day35-docker-security/README.md) - Build phase
- [Day 37: Container Registry Security](/azure-security-mastery/02-security-services/day37-container-registry-security/README.md) - Publish phase
- [Day 39: Azure Container Instances](/azure-security-mastery/02-security-services/day39-container-instances/README.md) - Deploy phase

---

**Created:** October 23, 2025 (Session #20)  
**Author:** YomHub (Azure Security Journey)  
**Purpose:** Demystify Azure container service pricing and help make informed architectural decisions
