# Day 35: Docker Security on Azure 🐳

**Date**: October 12, 2025  
**Focus**: Container Security Fundamentals  
**Cost**: €0.00 (Local practice only)  
**Status**: In Progress 🚧

---

## 🎯 Learning Objectives

By the end of this session, you will:

1. ✅ Understand container security fundamentals
2. ✅ Master Docker/Podman security best practices
3. ✅ Learn Azure Container Registry (ACR) security features (theory)
4. ✅ Build secure container images
5. ✅ Implement image scanning and vulnerability management
6. ✅ Practice locally with Podman (rootless security!)

---

## 📋 Prerequisites

### **What You Already Know**:
- ✅ Linux basics (from your 42 experience)
- ✅ Azure fundamentals (Days 1-33)
- ✅ Security principles (3 months of learning)
- ✅ Infrastructure as Code (Bicep from Day 33)

### **What You Have Installed**:
- ✅ **Podman 4.3.1** (Better than Docker for security!)
- ✅ Git, VS Code, Azure CLI
- ✅ Node.js (from Day 33 capstone)

### **Podman vs Docker**:
Podman is a **drop-in replacement** for Docker with better security:
- 🔒 **Rootless by default** (Docker requires root or sudo)
- 🔒 **Daemonless** (No background process with root privileges)
- 🔒 **Pod support** (Kubernetes-compatible)
- 🔒 **Same commands** (just replace `docker` with `podman`)

For this session, we'll use Podman commands but document Docker equivalents.

---

## 📚 Container Security Fundamentals

### **What is a Container?**

A container is a lightweight, standalone, executable package that includes:
- Application code
- Runtime environment
- System libraries
- Dependencies

**Key Difference from VMs**:
```
Virtual Machines                    Containers
┌─────────────────┐                ┌─────────────────┐
│   App A │ App B │                │ App A │ App B   │
├─────────┼───────┤                ├─────────────────┤
│  OS A   │ OS B  │                │  Container Eng  │
├─────────┴───────┤                ├─────────────────┤
│   Hypervisor    │                │   Host OS       │
├─────────────────┤                ├─────────────────┤
│   Hardware      │                │   Hardware      │
└─────────────────┘                └─────────────────┘

Size: GBs                          Size: MBs
Boot: Minutes                      Boot: Seconds
Isolation: Strong                  Isolation: Process-level
```

### **Why Container Security Matters**:

1. **Attack Surface**: Containers share the host kernel (unlike VMs)
2. **Supply Chain**: Images can contain vulnerabilities from base layers
3. **Runtime**: Misconfigurations can expose the host
4. **Scale**: Security issues multiply quickly (1 bad image → 1000 containers)

**Real-World Example**:
- 2019: **cryptojacking** in Docker Hub images (mining cryptocurrency)
- 2020: **51% of containers** had high/critical vulnerabilities (Snyk report)
- 2021: **85% of organizations** reported container security incidents

---

## 🔒 Container Security Model (The 4 C's)

```
┌─────────────────────────────────────────┐
│            Cloud (Azure)                │  ← Identity, Network, Compliance
│  ┌───────────────────────────────────┐  │
│  │         Cluster (AKS)             │  │  ← RBAC, Network Policies
│  │  ┌─────────────────────────────┐  │  │
│  │  │    Container (Runtime)      │  │  │  ← Security Context, Scanning
│  │  │  ┌───────────────────────┐  │  │  │
│  │  │  │   Code (App)          │  │  │  │  ← SAST, Secrets Management
│  │  │  └───────────────────────┘  │  │  │
│  │  └─────────────────────────────┘  │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
```

### **Security at Each Layer**:

1. **Code**: 
   - Static analysis (SAST)
   - Dependency scanning
   - Secret management

2. **Container**:
   - Minimal base images
   - Vulnerability scanning
   - Non-root user
   - Read-only filesystem

3. **Cluster** (AKS/ACI):
   - RBAC policies
   - Network policies
   - Pod security standards

4. **Cloud** (Azure):
   - Managed identity
   - Private networking
   - Compliance policies

---

## 🛡️ Docker/Podman Security Best Practices

### **1. Use Official Base Images**

❌ **Bad**:
```dockerfile
FROM ubuntu:latest
RUN apt-get update && apt-get install -y nodejs npm
```

✅ **Good**:
```dockerfile
FROM node:18-alpine
# Alpine = 5MB vs Ubuntu = 72MB
```

**Why**:
- Official images are maintained and scanned
- Smaller attack surface (Alpine has fewer packages)
- Regular security updates

### **2. Run as Non-Root User**

❌ **Bad**:
```dockerfile
FROM node:18-alpine
WORKDIR /app
COPY . .
CMD ["node", "server.js"]  # Runs as root!
```

✅ **Good**:
```dockerfile
FROM node:18-alpine
WORKDIR /app
COPY . .
RUN addgroup -g 1001 -S nodejs && adduser -S nodejs -u 1001
USER nodejs  # Switch to non-root
CMD ["node", "server.js"]
```

**Why**:
- If container compromised, attacker has root access
- Rootless = limited damage (can't modify host)

### **3. Multi-Stage Builds (Reduce Attack Surface)**

❌ **Bad**:
```dockerfile
FROM node:18
WORKDIR /app
COPY . .
RUN npm install  # Includes dev dependencies!
CMD ["node", "server.js"]
# Final image: 900MB+ with build tools
```

✅ **Good**:
```dockerfile
# Build stage
FROM node:18-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production

# Production stage
FROM node:18-alpine
WORKDIR /app
COPY --from=builder /app/node_modules ./node_modules
COPY . .
USER 1001
CMD ["node", "server.js"]
# Final image: 150MB, no build tools
```

### **4. Scan for Vulnerabilities**

```bash
# Using Podman (or Docker)
podman scan <image-name>

# Using Trivy (open-source scanner)
trivy image <image-name>

# Using Azure Defender for Containers
az acr scan --name <registry> --image <image>
```

### **5. Use .dockerignore**

```
# .dockerignore
node_modules/
npm-debug.log
.git/
.env
*.md
tests/
.vscode/
```

**Why**: Don't copy secrets, dev files, or large folders into images

### **6. Pin Versions (Avoid :latest)**

❌ **Bad**:
```dockerfile
FROM node:latest  # Changes unpredictably
```

✅ **Good**:
```dockerfile
FROM node:18.17.1-alpine3.18  # Specific, reproducible
```

---

## 🔍 Image Scanning & Vulnerability Management

### **Types of Scanners**:

1. **Trivy** (Open Source)
   - Fast, comprehensive
   - Detects OS packages + app dependencies
   - Free for all

2. **Snyk** (Commercial/Free tier)
   - Developer-friendly
   - Continuous monitoring
   - Fix recommendations

3. **Azure Defender for Containers**
   - Native Azure integration
   - Real-time threat detection
   - Costs ~€12/node/month (we'll skip for now)

4. **Clair** (Open Source)
   - API-driven
   - ACR uses this internally

### **Vulnerability Severity Levels**:

```
CRITICAL  🔴  Actively exploited, immediate fix required
HIGH      🟠  Serious flaw, fix within 7 days
MEDIUM    🟡  Moderate risk, fix within 30 days
LOW       🟢  Minor issue, fix when convenient
```

### **Example Scan Results**:

```
Scanning image: myapp:1.0.0

Total: 156 vulnerabilities
  - CRITICAL: 3  🔴
  - HIGH: 12     🟠
  - MEDIUM: 45   🟡
  - LOW: 96      🟢

Top Issues:
1. CVE-2023-12345 (CRITICAL): OpenSSL buffer overflow
2. CVE-2023-67890 (HIGH): Node.js prototype pollution
3. CVE-2023-11111 (HIGH): npm package with known backdoor
```

**Action Plan**:
1. Fix CRITICAL immediately (update OpenSSL)
2. Fix HIGH within 7 days (update Node.js, remove bad package)
3. Schedule MEDIUM/LOW for next sprint

---

## ☁️ Azure Container Registry (ACR) - Theory

> **NOTE**: ACR costs €20-40/month for Basic tier. We'll document without deploying.

### **What is ACR?**

Azure Container Registry is a **managed Docker registry** service:
- Store and manage container images
- Built-in security scanning (Defender for Containers)
- Geo-replication for high availability
- Integration with AKS, App Service, Azure DevOps

### **ACR Tiers**:

| Tier | Storage | Webhooks | Geo-Replication | Cost/Month |
|------|---------|----------|-----------------|------------|
| Basic | 10 GB | 2 | ❌ | €4.60 |
| Standard | 100 GB | 10 | ❌ | €18.40 |
| Premium | 500 GB | 500 | ✅ | €92.00 |

**Why ACR vs Docker Hub**:
- ✅ Private by default (Docker Hub public repos are free, private costs)
- ✅ Azure integration (managed identity, VNet, private endpoint)
- ✅ Built-in scanning (Docker Hub requires paid plan)
- ✅ Compliance (data stays in Azure, GDPR-friendly)
- ❌ Costs money (Docker Hub free tier = unlimited pulls)

### **ACR Security Features**:

1. **Content Trust** (Image Signing)
   ```bash
   # Sign images to prevent tampering
   az acr content-trust enable --name myregistry
   ```

2. **Vulnerability Scanning** (Defender for Containers)
   ```bash
   # Automatic scanning on push
   az acr task create \
     --name scan-on-push \
     --image {{.Run.Registry}}/{{.Run.ImageName}} \
     --trigger-on-push
   ```

3. **Private Endpoints**
   ```bash
   # Access ACR only from VNet (no public internet)
   az acr private-endpoint create \
     --name myacr-pe \
     --registry-name myregistry \
     --vnet-name myVNet \
     --subnet mySubnet
   ```

4. **RBAC & Managed Identity**
   ```bash
   # AKS pulls images without passwords
   az aks update \
     --name myAKS \
     --attach-acr myregistry
   ```

5. **Immutable Tags** (Prevent Overwriting)
   ```bash
   # Once pushed, tag cannot be changed
   az acr config content-trust update \
     --name myregistry \
     --enabled true
   ```

### **ACR Workflow**:

```
Developer                ACR                     AKS/ACI
    │                     │                        │
    │  1. Build Image     │                        │
    ├──────────────────>  │                        │
    │                     │                        │
    │  2. Scan Image      │                        │
    │     (Defender)      │                        │
    │  <──────────────────┤                        │
    │                     │                        │
    │  3. Push if Clean   │                        │
    ├──────────────────>  │                        │
    │                     │                        │
    │                     │  4. Pull Image         │
    │                     │  (Managed Identity)    │
    │                     ├─────────────────────>  │
    │                     │                        │
    │                     │  5. Deploy Container   │
    │                     │                        │
```

---

## 🚀 Hands-On: Build Secure Container

Let's containerize your Day 33 Node.js app with security best practices!

### **Files We'll Create**:
1. `Dockerfile.secure` - Production-ready Dockerfile
2. `.dockerignore` - Exclude unnecessary files
3. `security-checklist.md` - Validation checklist
4. `build-and-scan.sh` - Automated build + scan script

### **Security Goals**:
- ✅ Non-root user
- ✅ Multi-stage build
- ✅ Alpine base (minimal)
- ✅ No secrets in image
- ✅ Vulnerability scanning
- ✅ Health checks

---

## 📊 Success Criteria

By the end of Day 35, you should have:

1. ✅ Secure Dockerfile for Node.js app
2. ✅ Built and scanned container image locally
3. ✅ Container security checklist
4. ✅ ACR security concepts documented
5. ✅ Understanding of vulnerability management
6. ✅ Cost: €0.00 (all local practice)

---

## 📁 Session Structure

```
day35-docker-security/
├── README.md (this file)
├── Dockerfile.secure
├── .dockerignore
├── security-checklist.md
├── build-and-scan.sh
├── acr-security-guide.md
├── vulnerability-management.md
└── learning-summary.md
```

---

## 🎯 Next Steps

1. **Explore Container Security** → Review this README
2. **Build Secure Image** → Create Dockerfile
3. **Practice Scanning** → Run Trivy locally
4. **Document ACR** → Learn without deploying
5. **Update Progress** → Commit to GitHub

---

## 🔗 Resources

### **Official Documentation**:
- [Docker Security Best Practices](https://docs.docker.com/develop/security-best-practices/)
- [Podman Security Guide](https://docs.podman.io/en/latest/markdown/podman-security.1.html)
- [Azure Container Registry Docs](https://learn.microsoft.com/azure/container-registry/)
- [OWASP Container Security](https://cheatsheetseries.owasp.org/cheatsheets/Docker_Security_Cheat_Sheet.html)

### **Tools**:
- [Trivy Scanner](https://github.com/aquasecurity/trivy)
- [Hadolint (Dockerfile Linter)](https://github.com/hadolint/hadolint)
- [Dockle (Container Image Linter)](https://github.com/goodwithtech/dockle)

### **Learning**:
- [Docker Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Docker_Security_Cheat_Sheet.html)
- [CIS Docker Benchmark](https://www.cisecurity.org/benchmark/docker)

---

*Let's build secure containers! 🐳🔒*
