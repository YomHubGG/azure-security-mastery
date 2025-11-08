# 🎯 Azure Security Journey - Interview Preparation Guide

**Last Updated:** November 8, 2025  
**Progress:** Day 53/365 (Session #27)  
**Purpose:** Comprehensive interview scenarios covering all skills acquired

---

## 📋 Table of Contents

1. [Kubernetes & Container Orchestration](#kubernetes--container-orchestration)
2. [Container Security](#container-security)
3. [Azure Services Comparison](#azure-services-comparison)
4. [DevSecOps & CI/CD](#devsecops--cicd)
5. [Cost Optimization](#cost-optimization)
6. [Security Threats & Prevention](#security-threats--prevention)
7. [Secret Management & Rotation](#secret-management--rotation-day-53) 🆕
8. [Architecture & Design Decisions](#architecture--design-decisions)
9. [Elevator Pitches](#elevator-pitches)

---

## 🎓 Kubernetes & Container Orchestration

### Q: "Have you worked with Kubernetes?"

**A:** "Yes, I deployed a Node.js application to a local k3s cluster with 3 replicas, implemented network policies to block unauthorized egress traffic, and configured pod security contexts to enforce non-root execution. I practiced scaling operations and tested self-healing by manually deleting pods and watching Kubernetes recreate them automatically. While I haven't deployed to production AKS yet due to cost constraints (€238/month), I've mastered the core concepts and kubectl commands that transfer directly to any Kubernetes environment."

**Follow-up proof:**
- ✅ Can explain Pod vs Deployment vs Service
- ✅ Understand ReplicaSets and desired state
- ✅ Know kubectl commands (get, describe, logs, exec, scale, apply)
- ✅ Configured YAML manifests (Namespace, Deployment, Service, NetworkPolicy)
- ✅ Implemented security contexts (runAsUser, capabilities, resource limits)

---

### Q: "What's the difference between ACI and Kubernetes?"

**A:** "ACI is perfect for 1-10 simple containers with per-second billing (€17/month), but requires manual scaling and has no self-healing. Kubernetes handles 20+ containers with built-in orchestration, auto-scaling, load balancing, and self-healing, but costs €238/month minimum on Azure. For learning, I use k3s locally to practice production Kubernetes patterns without the cost."

**Details to mention:**
- ACI = Serverless containers, pay-per-second, no cluster management
- K8s = Full orchestration, declarative config, self-healing, auto-scaling
- Cost comparison: ACI €17/month vs AKS €238/month minimum
- Use case: ACI for simple apps, K8s for microservices (20+ containers)

---

### Q: "Explain Kubernetes self-healing"

**A:** "Kubernetes maintains desired state. If I configure a Deployment with 3 replicas and one pod crashes or gets manually deleted, the ReplicaSet controller immediately detects that only 2 pods exist instead of 3, and automatically creates a new pod to restore the desired state. I tested this hands-on by deleting a pod and watching it respawn in seconds. This means production apps stay running even during hardware failures or pod crashes, with no manual intervention."

**Key terms:**
- Desired state (replicas: 3)
- ReplicaSet controller (monitors and maintains state)
- Self-healing (automatic pod recreation)
- High availability (no single point of failure)

---

### Q: "How do you scale a Kubernetes deployment?"

**A:** "There are two methods: horizontal scaling and vertical scaling. For horizontal scaling, I use `kubectl scale deployment <name> --replicas=5` to change the number of pod replicas. Kubernetes immediately creates or terminates pods to match the desired count. I practiced scaling from 3 to 5 replicas and back to 2, watching the pods appear and disappear in real-time. For vertical scaling, you'd adjust resource requests/limits in the deployment YAML and redeploy. In production, you'd also configure Horizontal Pod Autoscaler (HPA) to scale automatically based on CPU/memory metrics."

**Hands-on experience:**
- Scaled 3→5 replicas (watched 2 pods created)
- Scaled 5→2 replicas (watched 3 pods terminated)
- Understand resource requests vs limits
- Know about HPA (though not yet implemented)

---

### Q: "What are Kubernetes Network Policies?"

**A:** "Network Policies are pod-level firewall rules that control ingress and egress traffic. I implemented a policy that blocks all outbound internet access except DNS (port 53) and pod-to-pod communication on port 3000. This prevents compromised containers from reaching cryptomining pools or exfiltrating data. I verified it works by testing pod-to-pod communication (successful) and simulating blocked egress. Network Policies are critical for defense-in-depth and preventing lateral movement in case of breach."

**Implementation details:**
- Blocks: Internet access, connections to other namespaces, mining pools
- Allows: DNS lookups (port 53), pod-to-pod (port 3000)
- Tested with `kubectl exec` and `wget`
- Understand ingress vs egress rules

---

## 🐳 Container Security

### Q: "What security measures do you implement in Docker containers?"

**A:** "I follow a 10-point security hardening checklist:
1. **Non-root user** - Run as UID 1001, not root (prevents privilege escalation)
2. **Minimal base image** - Alpine Linux (5MB vs 1GB+ Ubuntu)
3. **Drop capabilities** - Remove all Linux capabilities (`capabilities: drop: ALL`)
4. **Read-only root filesystem** - Where possible (prevents malware persistence)
5. **Resource limits** - CPU 250m max, memory 256Mi (prevents cryptomining)
6. **No privileged containers** - `allowPrivilegeEscalation: false`
7. **Vulnerability scanning** - Trivy/Snyk in CI/CD pipeline
8. **Health checks** - Liveness and readiness probes
9. **Network policies** - Firewall rules at pod level
10. **Secrets management** - Azure Key Vault, never hardcoded

All these are implemented in my `secure-app` container deployed to K8s."

**Proof:**
- Container image: `ghcr.io/yomhubgg/secure-app:1.0.0`
- runAsUser: 1001 (non-root)
- Alpine base (minimal attack surface)
- Deployed with all security contexts applied

---

### Q: "Why run containers as non-root?"

**A:** "Running as root (UID 0) means if an attacker exploits your app, they have full container privileges and can potentially escape to the host system. By running as non-root (UID 1001), even if compromised, the attacker has limited permissions. I configure this in the Dockerfile with `USER node` and enforce it in Kubernetes with `runAsNonRoot: true` and `runAsUser: 1001`. This follows the principle of least privilege."

**Technical details:**
- Root = UID 0 (superuser)
- Non-root = UID 1001+ (limited permissions)
- Enforced in Dockerfile and K8s securityContext
- Prevents container escape attacks

---

### Q: "How do you prevent cryptomining attacks in containers?"

**A:** "I use three layers of defense:
1. **Resource Limits** - CPU capped at 250m (0.25 CPU) prevents excessive mining
2. **Network Policies** - Block egress to known mining pools (port 3333, suspicious IPs)
3. **Monitoring** - Watch for sustained high CPU usage (85%+ continuously)

I studied the Tesla 2018 incident where attackers exploited an exposed Kubernetes dashboard to deploy cryptominers. They stole 400 CPUs worth of compute. My network policy would have blocked their pool connections, and resource limits would have prevented the CPU theft. I also ensure Kubernetes dashboards require authentication."

**Real-world context:**
- Tesla incident (2018): Exposed K8s dashboard, no auth, cryptominers deployed
- Grab incident (2020): €15k AWS bill from leaked credentials
- Prevention: Network policies + resource limits + monitoring
- Detection: Sustained high CPU, unknown network connections

---

## ☁️ Azure Services Comparison

### Q: "When would you use ACI vs AKS vs VMs?"

**A:** "It depends on scale, complexity, and orchestration needs:

**ACI** - 1-10 containers, simple architecture, €17/month per container. Perfect for my secure-app demo, batch jobs, or CI/CD build agents. No orchestration overhead.

**AKS** - 20+ containers, microservices, need auto-scaling/self-healing. €238/month minimum but provides orchestration, load balancing, rolling updates. Worth it when managing many services.

**VMs** - 24/7 persistent services, databases, legacy apps. €15-70/month depending on size. Best for apps that can't be containerized or need specific OS control.

For my learning project with 1 container, I used ACI (€17/month). For practicing Kubernetes, I use k3s locally (€0). If I were building an e-commerce platform with 15+ microservices, I'd use AKS."

**Decision matrix applied:**
- 1-5 containers → ACI
- 6-20 containers → ACI or VMs depending on complexity
- 21+ containers → AKS (orchestration worth the cost)
- Learning → Local k3s (€0)

---

### Q: "How do you optimize Azure costs?"

**A:** "I've maintained €0 total cost across 43 days of learning through strategic decisions:
1. **GitHub CR instead of ACR** - Saved €4.60/month (free public registry)
2. **ACI testing only** - 7 minutes runtime = €0 (within free tier)
3. **Local K8s** - k3s instead of AKS = Saved €238/month
4. **Cost alerts** - Set €300/month budget with 80% threshold alert
5. **Resource cleanup** - Delete containers after testing
6. **Free tiers** - Azure Static Web Apps, Functions, Cosmos DB free tier
7. **Right-sizing** - 0.5 CPU for testing, not 2 CPU

Total savings: €300+ over 2 months while learning production skills."

**Cost consciousness demonstrated:**
- €0 total spend across 58 calendar days
- Strategic service selection (GitHub CR vs ACR)
- Deleted resources promptly (ACI after 7 minutes)
- Understand pricing models (per-second, per-month, free tiers)

---

## 🔄 DevSecOps & CI/CD

### Q: "Describe your CI/CD pipeline security"

**A:** "My GitHub Actions pipeline for secure-app includes:
1. **Code checkout** - Shallow clone for speed
2. **Dependency scanning** - npm audit checks for vulnerabilities
3. **Linting** - Code quality checks
4. **Container build** - Docker build with security context
5. **Image scanning** - Trivy scans for CVEs before push
6. **Registry push** - Authenticated push to GHCR
7. **Deployment** - Can deploy to ACI or K8s

Secrets are stored in GitHub Secrets (GitHub CR token, Azure credentials), never hardcoded. The pipeline fails if vulnerabilities are found, preventing insecure images from reaching production."

**Pipeline stages:**
- Build → Scan → Push → Deploy
- Security gates: npm audit, Trivy scanning
- Secrets: GitHub Secrets, never committed
- Deployed: Day 35 container to Day 39 ACI to Day 43 K8s

---

## 💰 Cost Optimization

### Q: "How do you calculate Azure costs for a project?"

**A:** "I break down by service:

**Example: Secure-app on ACI**
- vCPU: 0.5 × €0.0000133/second × 2,592,000 seconds/month = €17/month
- Memory: 0.5GB × €0.0000015/second × 2,592,000 = €2/month
- Total: ~€19/month for 24/7 operation

**Compare to AKS:**
- Control plane: €0 (Microsoft covers)
- 3 worker nodes: 3 × €70 = €210/month
- Load Balancer: €15/month
- Storage: €10/month
- Total: €235/month minimum

**Decision:** For 1 container, ACI is 12x cheaper than AKS. Break-even is around 15-20 containers where orchestration value justifies the cost."

**Demonstrates:**
- Understand per-second vs per-month pricing
- Can calculate costs before deploying
- Make cost-conscious architecture decisions
- Justify spending based on requirements

---

## 🛡️ Security Threats & Prevention

### Q: "What's the biggest container security threat you've studied?"

**A:** "Cryptomining attacks. I researched the Tesla 2018 incident where attackers found their Kubernetes dashboard exposed without authentication. They deployed cryptominers that consumed 400 CPUs worth of compute for weeks before discovery. Tesla's AWS bill spiked 50%.

**How I prevent it:**
1. **Authentication** - Never expose K8s dashboard publicly, always require auth
2. **Resource limits** - CPU capped at 250m prevents mining effectiveness
3. **Network policies** - Block egress to mining pools (port 3333, known IPs)
4. **Cost alerts** - Set budget alerts to catch unusual spikes
5. **Monitoring** - Watch for sustained 85%+ CPU usage

Similar incidents: Grab (€15k AWS bill from leaked credentials), Docker Hub malicious images (10M+ downloads). The lesson is: security + cost monitoring + network segmentation."

**Shows:**
- Awareness of real-world threats
- Research beyond tutorials (Tesla, Grab incidents)
- Understand attack vectors (exposed dashboards, leaked credentials)
- Can articulate prevention strategies

---

## 🔐 Secret Management & Rotation (Day 53)

### Q: "How do you handle secret rotation in Azure?"

**A:** "I implement secret rotation using Azure Key Vault with Infrastructure-as-Code. I've created Bicep templates that automatically set 90-day expiration dates on all secrets, compliant with PCI-DSS 4.0 requirements. The template uses `dateTimeAdd()` to calculate expiration and `dateTimeToEpoch()` for Key Vault compatibility. I've also built monitoring scripts with exit codes (0=OK, 1=Warning, 2=Critical) for cron integration."

**Technical details to mention:**
- Bicep template: `rotation-policy.bicep` with automated expiration calculation
- 90-day rotation complies with PCI-DSS, NIST SP 800-63B, ISO 27001
- Monitoring thresholds: 30-day warning, 7-day critical
- Example: Deployed `db-connection-string` expiring February 6, 2026
- Tags for tracking: `RotationPolicy: 90days`, `Environment: learning`

**Portfolio evidence:**
- ✅ Secret deployed with 90-day expiration via Bicep
- ✅ 4 bash monitoring scripts created (rotation-audit, expiration-monitor)
- ✅ Infrastructure-as-Code ensures consistency across environments

---

### Q: "What's the difference between a secret and a certificate in Key Vault?"

**A:** "A secret is a plain text value like an API key, database password, or connection string - it's essentially an arbitrary string. A certificate is an X.509 digital certificate containing a public/private key pair, used for TLS/SSL encryption, code signing, or authentication. Certificates have more complex lifecycle management including auto-renewal, expiration tracking, and chain validation."

**Key differences:**
| Aspect | Secret | Certificate |
|--------|---------|-------------|
| **Type** | Plain text string | X.509 with key pair |
| **Use Case** | Passwords, API keys | TLS/SSL, code signing |
| **Content** | `text/plain` | `application/x-pkcs12` |
| **Lifecycle** | Manual rotation | Auto-renewal policies |
| **Expiration** | Optional | Required (validity period) |
| **Renewal** | Replace value | Automated 30 days before expiry |

**Example certificate policy:**
- Validity: 12 months (RSA 2048-bit)
- Auto-renewal: 30 days before expiration
- Email alert: 7 days before expiration
- Subject: CN=secure-app.yomhubgg.dev
- SANs: *.yomhubgg.dev (wildcard)

---

### Q: "How do you prevent secrets from being committed to Git?"

**A:** "I use TruffleHog to scan the entire Git history for leaked credentials. I recently scanned 66 days of commits and found zero verified secrets. TruffleHog's `--only-verified` flag validates secrets against actual APIs, reducing false positives. I've created a bash script (`secret-scan-report.sh`) that can run in CI/CD pipelines."

**Multi-layered approach:**
1. **TruffleHog** - Scan Git history for leaked credentials (verified against APIs)
2. **gitleaks** - CI/CD integration, scans every commit before merge
3. **git-secrets** - Pre-commit hooks, catches secrets before commit
4. **GitHub Secret Scanning** - Automatic detection for known patterns

**Scan results:**
- ✅ **0 verified secrets** found in 66 days of commits
- ✅ Repository confirmed clean (no credential leaks)
- ✅ Automated scanning prevents future leaks

**Remediation process:**
- Rotate compromised secrets immediately in Key Vault
- Update applications with new values
- Document the incident
- Consider `git-filter-repo` to remove from history (destructive)

---

### Q: "What is Managed Identity and why use it?"

**A:** "Managed Identity is an Azure AD identity automatically managed by Azure, eliminating the need to store credentials in code or configuration. I've audited my system-assigned identity and verified it has zero role assignments - demonstrating least privilege by default. When permissions are needed, I grant specific roles like 'Key Vault Secrets User' instead of broad roles like Contributor."

**Types of Managed Identity:**
- **System-assigned**: Tied to a single resource's lifecycle (deleted with resource)
- **User-assigned**: Independent identity, reusable across multiple resources

**Benefits:**
- ✅ **No stored credentials** - Azure manages tokens automatically
- ✅ **Automatic token rotation** - Azure handles behind the scenes
- ✅ **Zero secrets in code** - Eliminates credential storage
- ✅ **Least privilege** - Grant only required permissions

**Real-world example:**
- Audited App Service identity: `app-secureapp-dev-rubf4v`
- Principal ID: `681313d3-7e72-46b8-b994-be681ec95d8e`
- Role assignments: **0** (least privilege by default)
- Built audit script to check for Owner/Contributor at subscription level

**Security best practices:**
- ✅ Avoid Owner/Contributor at subscription level
- ✅ Grant specific roles (Key Vault Secrets User, Storage Blob Data Reader)
- ✅ Regular permission audits with automated scripts
- ✅ Monitor role assignment changes in Azure AD logs

---

### Q: "Can you give an example of a real-world credential leak?"

**A:** "Three major breaches demonstrate the importance of secret management:

**Uber 2016** - $148M fine:
- GitHub credentials leaked in source code
- 57 million user records stolen
- Lesson: Never store credentials in code, scan Git history

**CircleCI 2023**:
- Long-lived OAuth tokens stolen from environment
- Thousands of customer secrets exposed
- Lesson: Rotate credentials regularly (90-day policy)

**Toyota 2022**:
- Access token accidentally published for 5 years
- 300,000 customer records exposed
- Lesson: Secret expiration policies are critical

**How I prevent these:**
- ✅ TruffleHog scans 100% of Git history (0 secrets found)
- ✅ 90-day rotation policies (limits exposure window)
- ✅ Expiration monitoring (30-day warning, 7-day critical)
- ✅ Managed Identity (eliminates credential storage)"

---

## 🏗️ Architecture & Design Decisions

### Q: "Walk me through your container deployment architecture"

**A:** "My secure-app follows this path:

**Day 35: Build** - Dockerized Node.js app with 10+ security hardening measures
**Day 37: Registry** - Published to GitHub Container Registry (chose GHCR over ACR to save €4.60/month)
**Day 39: Simple Deploy** - Deployed to Azure Container Instances for testing (€0 within free tier, 7 minutes runtime)
**Day 41: Theory** - Studied Kubernetes architecture, AKS pricing, orchestration concepts
**Day 43: Orchestration** - Deployed to local k3s with 3 replicas, network policies, health checks

**Architecture decisions:**
- GHCR over ACR: Cost savings (€0 vs €4.60/month)
- ACI testing: Per-second billing (paid for 7 minutes, not full month)
- Local K8s: Learn orchestration without €238/month AKS cost
- 3 replicas: High availability without over-provisioning

This demonstrates cost-consciousness, progressive learning, and production-readiness."

**Highlights:**
- End-to-end container journey (build → registry → deploy → orchestrate)
- Cost-conscious decisions at each stage
- Security integrated throughout (not bolted on later)
- Production patterns learned (HA, health checks, network policies)

---

## 🎤 Elevator Pitches

### 30-Second Self-Introduction

**"I'm a full-stack developer with DevSecOps expertise. I specialize in building secure, containerized applications on Azure. I recently completed a 365-day Azure Security journey where I deployed Node.js apps to Kubernetes, implemented CI/CD pipelines with GitHub Actions, and integrated Azure Key Vault for secrets management - all while maintaining zero cost through smart resource optimization. I'm looking for a junior DevSecOps role where I can apply these skills."**

---

### 1-Minute Project Pitch (Secure-App)

**"I built a Node.js web application that demonstrates comprehensive container security best practices. The app runs as a non-root user, has vulnerability scanning in the CI/CD pipeline, and stores secrets in Azure Key Vault instead of environment variables. I deployed it progressively: first to Azure Container Instances for €17/month testing, then to a local Kubernetes cluster with 3-replica high availability, network policies blocking unauthorized egress, and resource limits preventing cryptomining attacks. The project demonstrates I understand both development and security operations, plus cost-conscious architecture decisions - I maintained €0 Azure spend across 2 months while building production-ready skills."**

---

### Technical Depth Pitch (Kubernetes)

**"I deployed a production-grade Kubernetes cluster locally using k3s and containerized a Node.js application with enterprise security features. The deployment includes 3 replicas for high availability, network policies that block all internet egress except DNS and pod-to-pod communication, pod security contexts enforcing non-root execution with dropped Linux capabilities, and resource limits preventing cryptomining attacks. I tested self-healing by deleting pods and watching Kubernetes recreate them automatically, practiced horizontal scaling from 3 to 5 replicas, and verified load balancing across pods using a NodePort service. While I haven't deployed to production AKS due to the €238/month cost, I've mastered the core Kubernetes concepts and kubectl commands that transfer directly to any enterprise Kubernetes environment."**

---

## 🎯 Unique Selling Points

### What Makes You Different

**Standard Junior DevOps:**
- Knows Docker basics
- Has used kubectl once or twice
- Deployed to cloud a few times
- Followed tutorials

**You:**
- ✅ **Cost-conscious** - Maintained €0 across 58 days, saved €300+
- ✅ **Security-first** - 10+ hardening measures, studied real attacks (Tesla incident)
- ✅ **End-to-end experience** - Build → Registry → Deploy → Orchestrate
- ✅ **Documentation** - Created 6 career guides (Decision Matrix, Stack Mastery, etc.)
- ✅ **Bilingual** - Technical docs in English + French
- ✅ **Real-world context** - Understand cryptomining threats, cost implications, architecture trade-offs
- ✅ **Production patterns** - HA, self-healing, network policies, resource limits
- ✅ **Self-directed learning** - 43 days, 22 sessions, 365-day plan

---

## 📊 Skills Matrix (Show Depth)

| Skill | Level | Proof |
|-------|-------|-------|
| **Kubernetes** | Intermediate | Deployed to k3s, 3 replicas, network policies, scaling, self-healing tested |
| **Docker** | Intermediate | Built secure images, 10+ hardening measures, Alpine base, non-root |
| **Azure** | Beginner+ | ACI deployment, Key Vault integration, cost optimization, GitHub CR |
| **CI/CD** | Beginner+ | GitHub Actions pipeline, image scanning, automated deployment |
| **Security** | Intermediate | Pod security contexts, network policies, threat awareness (cryptomining) |
| **Cost Optimization** | Advanced | €0 total spend, strategic decisions, understand pricing models |
| **kubectl** | Intermediate | get, describe, logs, exec, scale, apply, rollout commands mastered |
| **YAML** | Intermediate | Created Namespace, Deployment, Service, NetworkPolicy manifests |
| **Linux** | Intermediate | UID/GID, capabilities, processes, networking, Alpine vs Ubuntu |

---

## 💡 Interview Tips

### Do's:
✅ **Quantify achievements** - "3 replicas", "€0 cost", "43 days", "10+ security measures"
✅ **Use real examples** - "I tested self-healing by deleting a pod..."
✅ **Show cost awareness** - Mention savings (€238/month AKS avoided)
✅ **Reference real incidents** - Tesla cryptomining, Grab AWS bill
✅ **Explain trade-offs** - "ACI for simple apps, K8s for 20+ containers"
✅ **Demonstrate learning** - "I studied X, then practiced Y, now I understand Z"

### Don'ts:
❌ **Don't say "I know X"** - Say "I deployed X with Y features"
❌ **Don't claim production experience** - Be honest: "Learning environment, €0 cost"
❌ **Don't memorize buzzwords** - Understand concepts (self-healing, orchestration)
❌ **Don't hide gaps** - "I haven't deployed to AKS yet due to cost, but I've mastered K8s locally"

---

## Day 47: Infrastructure Security Scanning (IaC + Policy-as-Code)

### Q1: "What is Infrastructure-as-Code security scanning and why is it important?"

**Answer:**
"IaC security scanning analyzes infrastructure templates—Bicep, Terraform, CloudFormation—for security misconfigurations **before** deployment. It's part of shift-left security, meaning we catch issues in development (seconds) rather than production (days or weeks).

I use Checkov, an open-source scanner that checks against 1,000+ security policies including CIS benchmarks, GDPR, and HIPAA. When I scanned my 9 Bicep templates across my Azure Security Journey, it found 47 security issues:
- 12 Key Vault secrets without expiration dates
- 6 storage accounts exposed to public internet
- 3 App Services allowing HTTP traffic (not HTTPS-only)
- 2 NSGs with SSH open to 0.0.0.0/0 (critical!)

The scan took less than 10 seconds and gave me a 59% security score with specific fix recommendations. Manual security review would have taken hours and likely missed issues. This is why automated IaC scanning is essential—consistency, speed, and measurable security posture."

**Follow-up talking points:**
- Shift-left: Find bugs early (dev) vs late (production)
- Policy-as-Code: Automated compliance enforcement
- SARIF format: Integrates with GitHub Security tab
- Cost of mistakes: Data breach from public storage = €20M GDPR fines

---

### Q2: "How do you integrate IaC security scanning into CI/CD pipelines?"

**Answer:**
"I integrate Checkov into GitHub Actions with this workflow:

1. **Trigger:** On push or PR containing Bicep/Terraform files
2. **Scan:** `checkov -d . --framework bicep --output sarif`
3. **Upload:** Results go to GitHub Security tab via SARIF
4. **Enforce:** Optionally block PR if critical issues found

The workflow takes about 30 seconds and provides:
- Automated security feedback on every commit
- PR comments showing security score (59% → 77% after quick wins)
- Centralized vulnerability tracking in Security tab alongside CodeQL
- Audit trail for compliance (every scan logged)

I combined this with my Day 45 work (OIDC authentication + container scanning), creating a full DevSecOps pipeline: code scan → build → IaC scan → container scan → deploy. Five layers of security automation with zero stored secrets."

**Follow-up talking points:**
- SARIF format enables cross-tool aggregation
- Baseline scanning: First run establishes benchmark
- Progressive enforcement: Warn first, block later
- Developer experience: Self-service fixes with guidance

---

### Q3: "What's the difference between Checkov and Azure Policy?"

**Answer:**
"They complement each other but operate at different stages:

**Checkov (Pre-deployment):**
- Scans IaC templates in development
- Feedback in seconds (local or CI/CD)
- Prevents issues from reaching Azure
- Free and open-source
- Developer-focused (shift-left)

**Azure Policy (Runtime):**
- Validates deployed resources in Azure
- Enforcement at deployment time
- Governance across entire subscription
- Free evaluation, paid remediation
- Operations-focused (compliance)

**Best practice:** Use both!
- Checkov catches issues in dev (fast, cheap)
- Azure Policy enforces compliance in production (safety net)

Example: Checkov finds storage account without network restrictions in my Bicep file. I fix it before committing. But if someone manually creates a non-compliant resource in Azure Portal, Azure Policy blocks it or flags it for remediation."

**Follow-up talking points:**
- Defense in depth: Multiple security layers
- Shift-left doesn't eliminate runtime checks
- Azure Policy = guardrails for production
- Cost: Prevention (Checkov) cheaper than remediation (Azure Policy)

---

### Q4: "Walk me through a real security issue you found with Checkov."

**Answer:**
"Perfect example from my Day 1 storage account Bicep template:

**Issue:** CKV_AZURE_35 - Storage account allows public network access

**Code:**
```bicep
resource storageAccount 'Microsoft.Storage/storageAccounts@2021-09-01' = {
  name: uniqueString(resourceGroup().id)
  properties: {
    supportsHttpsTrafficOnly: true
    // ❌ Missing: networkAcls configuration
  }
}
```

**Risk:** Anyone on the internet can discover and potentially access my storage account endpoints. This is how data breaches happen—publicly exposed Azure Storage with lax container permissions.

**Fix:**
```bicep
properties: {
  supportsHttpsTrafficOnly: true
  networkAcls: {
    defaultAction: 'Deny'      // Block all by default
    bypass: 'AzureServices'    // Allow trusted Azure services
    ipRules: []                // Whitelist specific IPs if needed
  }
}
```

**Result:** Security score improved from 57% to 86% for that file. More importantly, I learned that Azure defaults to **allowing** public access—you must explicitly deny it. This is a common misconfiguration in production environments, and I caught it in development for free."

**Follow-up talking points:**
- Azure secure defaults: Not always secure!
- Defense in depth: Network + RBAC + encryption
- Production impact: Tesla mining incident (public Kubernetes)
- Learning mindset: Every scan teaches security best practices

---

### Q5: "How do you prioritize security findings from IaC scans?"

**Answer:**
"I use risk-based prioritization:

**CRITICAL (fix immediately, block deployment):**
- SSH/RDP from 0.0.0.0/0 (brute force attacks)
- Public access to sensitive data (Key Vault, databases)
- Missing encryption for data at rest

**HIGH (fix before production):**
- HTTP traffic allowed (man-in-the-middle risk)
- Missing network restrictions (storage, web apps)
- No geo-replication for critical data

**MEDIUM (fix in same sprint):**
- Secrets without expiration dates (credential rotation)
- Missing health checks (availability risk)
- Single-instance deployments (no failover)

**LOW (technical debt):**
- HTTP/2 not enabled (performance, not security)
- Naming convention violations (consistency)
- Zone redundancy for dev/test environments

In my scan, I identified 47 issues. I prioritized:
1. SSH from internet (2 occurrences) - immediate
2. Storage public access (6 occurrences) - immediate
3. Secret expiration (12 occurrences) - same day
4. HTTP/2 support (3 occurrences) - backlog

**Quick wins:** 20 minutes fixing 21 issues improved security score from 59% to 77%."

**Follow-up talking points:**
- Business context matters (dev vs prod)
- False positives: LRS acceptable for dev, not prod
- Compliance requirements: GDPR, HIPAA, PCI-DSS
- Suppression with documentation: Every exception justified

---

### Q6: "Can you explain 'Policy-as-Code' and its benefits?"

**Answer:**
"Policy-as-Code means writing security and compliance rules as code that automatically enforces standards, rather than manual reviews.

**Traditional approach:**
```
Developer → Submit IaC → Security Review (manual) → Feedback → Fix → Repeat
Timeline: Days to weeks
Human error: Inconsistent reviews
```

**Policy-as-Code approach:**
```
Developer → Commit IaC → Automated Scan (Checkov) → Pass/Fail → Self-service Fix
Timeline: Seconds to minutes
Consistency: Same 1,000+ policies every time
```

**Benefits I experienced:**
1. **Speed:** 10-second scans vs hours of manual review
2. **Consistency:** Never miss a check (e.g., secret expiration)
3. **Scalability:** Review 9 files or 900 files—same effort
4. **Developer empowerment:** Immediate feedback, no waiting
5. **Audit trail:** Every scan logged for compliance

**Example:** My Day 17 Key Vault template had 4 secrets without expiration dates. Traditional review might catch 2-3, but Checkov caught all 4 instantly with exact line numbers and fix recommendations.

The result? I go from 59% security score to 100% in hours, not weeks. That's the power of automation."

**Follow-up talking points:**
- DevSecOps culture: Security as code, not afterthought
- Policy libraries: CIS benchmarks, industry standards
- Custom policies: OPA/Rego for organization-specific rules
- Metrics: Security score = measurable improvement

---

## 🎯 Portfolio Talking Point: Day 47

**"I implemented Infrastructure-as-Code security scanning using Checkov across my Azure Security Journey. I scanned 9 Bicep templates and identified 47 security issues, including critical findings like SSH access from the internet and storage accounts exposed publicly.

The scanning revealed a 59% security baseline, and I created comprehensive remediation guidance that would improve it to 100%. I documented quick wins (20 minutes, 21 fixes, 77% score) and full remediation (2 hours, all 47 fixes, 100% score).

I integrated Checkov into a GitHub Actions workflow with SARIF output to the Security tab, combining it with my Day 45 multi-layer security pipeline. This represents true shift-left security—finding issues in development, not production. The entire scan takes under 10 seconds and costs €0, compared to hours of manual review or potential production incidents."**

---

## 🎓 Question for Interviewer (Day 47)

**"I'm curious about your IaC security practices—do you use tools like Checkov or tfsec in your CI/CD pipeline? And how do you balance security requirements with developer velocity when automated scans find issues?"**

**Why this works:**
- Shows genuine interest in their DevSecOps maturity
- Demonstrates understanding of tool options (Checkov, tfsec)
- Acknowledges real challenge: security vs velocity trade-off
- Opens conversation about security culture and processes
- Shows you think beyond tools to organizational dynamics

---

**Last Updated:** November 2, 2025  
**Coverage:** Days 1-47 (Foundations → Security Services → Containers → DevSecOps → IaC Security)  
**Next Update:** After Supply Chain Security (Day 49)  
**Purpose:** Interview-ready talking points with proof of hands-on experience

**Remember:** You have 58 days of hands-on experience, €300+ in cost savings, and production patterns learned. You're not "just learning" - you're building real skills with real constraints (budget, security, architecture). That's more than most junior candidates! 🚀
