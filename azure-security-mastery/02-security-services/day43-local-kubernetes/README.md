# Day 43: Local Kubernetes Practice - Hands-On Learning

**Date:** October 29, 2025  
**Session:** #22  
**Cost:** €0 (local k3s cluster)  
**Status:** ✅ COMPLETED

---

## 🎯 Learning Objectives

Today we deployed a **production-grade Kubernetes cluster locally** and practiced real-world orchestration without the €238/month AKS cost!

**What We Accomplished:**
- ✅ Installed k3s (lightweight Kubernetes v1.33.5)
- ✅ Deployed `secure-app` with 3 replicas
- ✅ Practiced kubectl commands (get, describe, logs, exec, scale)
- ✅ Tested self-healing (deleted pod, K8s auto-recreated it)
- ✅ Scaled deployment (3→5→2→3 replicas)
- ✅ Implemented Network Policy (pod firewall)
- ✅ Verified pod-to-pod communication
- ✅ Exposed app via NodePort Service (localhost:30080)

---

## 📁 Files Created

```
day43-local-kubernetes/
├── 01-namespace.yaml              # Isolated namespace for secure-app
├── 02-deployment.yaml             # 3 replicas with security context
├── 03-service.yaml                # NodePort service (port 30080)
├── 04-network-policy.yaml         # Pod firewall (blocks internet)
├── KUBECTL-CHEATSHEET.md          # Command reference + exercises
└── README.md                      # This file
```

---

## 🚀 Deployment Architecture

```
                    Internet
                       |
                       ↓
              [localhost:30080]
                       |
                       ↓
            ┌──────────────────────┐
            │  Service (NodePort)  │ ← Load balances traffic
            │   secure-app-service │
            └──────────────────────┘
                       |
         ┌─────────────┼─────────────┐
         ↓             ↓             ↓
    [Pod 1]       [Pod 2]       [Pod 3]      ← 3 replicas
    10.42.0.10    10.42.0.9     10.42.0.11
    
    Each pod runs: ghcr.io/yomhubgg/secure-app:1.0.0
```

---

## 🛡️ Security Features Implemented

### 1. **Security Context (Pod Level)**
```yaml
securityContext:
  runAsNonRoot: true      # Must run as non-root user
  runAsUser: 1001         # Specific UID (not 0!)
  fsGroup: 1001           # File system group
```

### 2. **Security Context (Container Level)**
```yaml
securityContext:
  allowPrivilegeEscalation: false  # Can't gain more privileges
  runAsNonRoot: true               # Enforced again at container level
  runAsUser: 1001                  # UID 1001 (from Day 35!)
  capabilities:
    drop:
    - ALL                          # Drop all Linux capabilities
  readOnlyRootFilesystem: false    # Allow logs (app writes to /tmp)
```

### 3. **Resource Limits**
```yaml
resources:
  requests:
    memory: "128Mi"      # Minimum guaranteed
    cpu: "100m"          # 0.1 CPU minimum
  limits:
    memory: "256Mi"      # Maximum allowed (prevents OOM)
    cpu: "250m"          # 0.25 CPU max (prevents cryptomining!)
```

### 4. **Health Checks**
```yaml
livenessProbe:           # Restart if unhealthy
  httpGet:
    path: /
    port: 3000
  initialDelaySeconds: 10
  periodSeconds: 10

readinessProbe:          # Remove from service if not ready
  httpGet:
    path: /
    port: 3000
  initialDelaySeconds: 5
  periodSeconds: 5
```

### 5. **Network Policy (Pod Firewall)**
```yaml
# Blocks:
❌ Outbound internet access
❌ Connections to other namespaces
❌ Cryptomining pool connections
❌ Unauthorized egress traffic

# Allows:
✅ DNS lookups (port 53)
✅ Pod-to-pod communication (port 3000)
✅ Inbound traffic (for testing)
```

---

## 📊 Key Commands Used

### Deployment
```bash
# Install k3s
curl -sfL https://get.k3s.io | sh -

# Apply manifests
sudo kubectl apply -f 01-namespace.yaml
sudo kubectl apply -f 02-deployment.yaml
sudo kubectl apply -f 03-service.yaml
sudo kubectl apply -f 04-network-policy.yaml

# Verify deployment
sudo kubectl get all -n secure-app
```

### Testing
```bash
# Test app via service
curl http://localhost:30080

# Test pod-to-pod communication
sudo kubectl exec <pod-1> -n secure-app -- wget -qO- http://<pod-2-ip>:3000

# Watch pods in real-time
sudo kubectl get pods -n secure-app -w
```

### Self-Healing Demo
```bash
# Delete a pod (Kubernetes auto-recreates it!)
sudo kubectl delete pod <pod-name> -n secure-app

# Watch it respawn
sudo kubectl get pods -n secure-app -w
```

### Scaling
```bash
# Scale to 5 replicas
sudo kubectl scale deployment secure-app -n secure-app --replicas=5

# Scale back to 3
sudo kubectl scale deployment secure-app -n secure-app --replicas=3
```

### Logs & Debugging
```bash
# View logs
sudo kubectl logs <pod-name> -n secure-app

# Exec into pod
sudo kubectl exec -it <pod-name> -n secure-app -- sh

# Check resource usage
sudo kubectl top pods -n secure-app
```

---

## 🎓 What We Learned

### **Core Kubernetes Concepts**

1. **Pods**
   - Smallest deployable unit
   - Can contain 1+ containers
   - Ephemeral (can die and be recreated)
   - Each gets its own IP address

2. **Deployments**
   - Manages desired state (e.g., 3 replicas)
   - Handles rolling updates
   - Ensures pods are always running
   - Creates ReplicaSets automatically

3. **Services**
   - Stable network endpoint
   - Load balances traffic across pods
   - Types: ClusterIP, NodePort, LoadBalancer
   - We used NodePort (exposes on node's IP:30080)

4. **Namespaces**
   - Logical isolation
   - Resource quotas per namespace
   - Separate environments (dev/staging/prod)
   - Network policies apply per namespace

5. **Network Policies**
   - Pod-level firewall rules
   - Control ingress (incoming) and egress (outgoing) traffic
   - Prevent lateral movement
   - Block cryptomining attacks

---

### **Kubernetes Magic Demonstrated**

#### **Self-Healing**
```
Before: 3 pods running
Action: Deleted 1 pod manually
Result: Kubernetes detected 2/3 pods → Created new pod instantly
Status: 3 pods running again (desired state maintained!)
```

**Why this matters:**
- Pod crashes? K8s restarts it
- Node dies? K8s moves pods to healthy nodes
- Hardware failure? App stays running
- **No manual intervention needed!**

#### **Scaling**
```
Original: 3 pods
Command: kubectl scale --replicas=5
Result:  Kubernetes created 2 new pods in seconds
Command: kubectl scale --replicas=2
Result:  Kubernetes terminated 3 pods gracefully
```

**Why this matters:**
- Traffic spike? Scale up instantly
- Low traffic? Scale down to save resources
- Black Friday? Auto-scaling handles it
- **One command, automatic orchestration!**

#### **Load Balancing**
```
Service IP: 10.43.83.187
Backend Pods: 10.42.0.10, 10.42.0.9, 10.42.0.11
Request: curl http://localhost:30080
Result: Traffic distributed across all 3 pods
```

**Why this matters:**
- No single point of failure
- Traffic evenly distributed
- Unhealthy pods removed automatically
- **Built-in high availability!**

---

## 🆚 Comparison: ACI (Day 39) vs K8s (Day 43)

| Feature | ACI | Kubernetes (k3s) | Winner |
|---------|-----|------------------|--------|
| **Cost (Azure)** | €17/month | €238/month (AKS) | K8s (but expensive) |
| **Cost (Local)** | N/A | €0 | ✅ K8s |
| **Self-Healing** | No | Yes (automatic) | ✅ K8s |
| **Scaling** | Manual | One command | ✅ K8s |
| **Load Balancing** | Need external LB (+€15) | Built-in Service | ✅ K8s |
| **Health Checks** | Manual | Built-in probes | ✅ K8s |
| **Replicas** | Create multiple containers | `replicas: 3` in YAML | ✅ K8s |
| **Complexity** | Simple | High learning curve | ACI (simpler) |
| **Best For** | 1-10 containers | 20+ containers | Depends on scale |

**Verdict:**
- **ACI**: Perfect for simple, stateless apps (1-10 containers)
- **K8s**: Essential for microservices (20+ containers, orchestration needed)
- **Local K8s**: Best learning tool (€0 cost, 95% same as AKS)

---

## 💡 Real-World Scenarios

### **Scenario 1: Node Crash**
```
What happens: Your server crashes at 3am
ACI Response: Containers dead, manual restart needed
K8s Response: Detects unhealthy pods, moves them to other nodes automatically
Downtime: ACI = minutes, K8s = seconds
```

### **Scenario 2: Traffic Spike**
```
What happens: Black Friday, traffic jumps 10x
ACI Response: Manually create more containers, configure load balancer
K8s Response: Horizontal Pod Autoscaler scales from 3→30 pods automatically
Time to scale: ACI = 10+ minutes, K8s = 30 seconds
```

### **Scenario 3: Cryptomining Attack**
```
What happens: Attacker deploys cryptominer container
ACI Response: No resource limits, miner consumes all CPU, bill skyrockets
K8s Response: Resource limits (cpu: 250m) prevent excessive usage
           Network Policy blocks mining pool connections
Result: ACI = €15k surprise bill, K8s = Attack blocked
```

### **Scenario 4: Rolling Update**
```
What happens: Need to deploy new app version
ACI Response: Stop all containers, start new ones (downtime!)
K8s Response: Rolling update (25% at a time), zero downtime
User Experience: ACI = "503 Service Unavailable", K8s = seamless
```

---

## 🎯 Skills Acquired

### **Technical Skills**
- ✅ k3s installation and configuration
- ✅ kubectl command mastery (get, describe, logs, exec, scale, apply)
- ✅ YAML manifest creation (Namespace, Deployment, Service, NetworkPolicy)
- ✅ Pod security contexts (runAsUser, capabilities, read-only filesystem)
- ✅ Resource limits and requests (CPU, memory)
- ✅ Health check configuration (liveness, readiness probes)
- ✅ Network policy implementation (ingress/egress rules)
- ✅ Pod-to-pod networking verification
- ✅ Service exposure (NodePort type)
- ✅ Deployment scaling (horizontal scaling)

### **Conceptual Understanding**
- ✅ Kubernetes orchestration principles
- ✅ Declarative vs imperative configuration
- ✅ Desired state management
- ✅ Self-healing mechanisms
- ✅ Load balancing strategies
- ✅ Namespace isolation
- ✅ Network segmentation
- ✅ Container security best practices
- ✅ Resource management
- ✅ High availability patterns

### **Operational Skills**
- ✅ Troubleshooting pod issues
- ✅ Log analysis with kubectl logs
- ✅ Real-time monitoring (kubectl get -w)
- ✅ Interactive debugging (kubectl exec)
- ✅ Performance testing (pod-to-pod communication)
- ✅ Security validation (network policy testing)
- ✅ Scaling operations (replica management)
- ✅ Deployment lifecycle (create, update, rollback)

---

## 🔗 Connection to Previous Days

### **Day 35: Docker Security**
- ✅ Container image: `ghcr.io/yomhubgg/secure-app:1.0.0`
- ✅ Security hardening: runAsUser 1001, non-root
- ✅ Alpine base image (minimal attack surface)
- ✅ 10+ security measures applied

### **Day 37: Container Registry**
- ✅ Image stored in GitHub Container Registry
- ✅ Public visibility for easy deployment
- ✅ Kubernetes pulled image automatically

### **Day 39: Azure Container Instances**
- ✅ Deployed same container to ACI
- ✅ Learned per-second billing (€17/month)
- ✅ Understood ACI limitations (no orchestration)

### **Day 41: AKS Theory**
- ✅ Learned Kubernetes concepts (control plane, nodes, pods)
- ✅ Understood AKS pricing (€238/month minimum)
- ✅ Decided to practice locally first (BCA plan!)
- ✅ Now using 95% of AKS features locally for €0

---

## 📈 Progress Update

**Journey Stats:**
- **Days Completed:** 43/365 (11.8%)
- **Sessions:** 22
- **Calendar Days:** 58 (Sep 1 → Oct 29)
- **Cost Total:** €0 (100% free tier optimization)
- **Container Path:** Build (Day 35) → Registry (Day 37) → Deploy (Day 39) → Orchestration (Day 43) ✅

**Container Security Mastery:**
- ✅ Docker security hardening
- ✅ Image vulnerability awareness
- ✅ Registry management
- ✅ Serverless containers (ACI)
- ✅ Kubernetes orchestration (local)
- 🔄 Next: DevSecOps CI/CD integration

---

## 🚀 Next Steps

### **Immediate (This Week)**
1. ✅ Day 43 complete!
2. 🔄 Practice more kubectl commands (see KUBECTL-CHEATSHEET.md)
3. 🔄 Experiment with scaling and self-healing
4. 🔄 Add ConfigMaps and Secrets (optional)

### **Next Session (Day 45+)**
1. ❌ Skip AKS deployment (save €238/month)
2. ❌ Move to DevSecOps deep dive
3. ❌ GitHub Actions + Kubernetes integration
4. ❌ SAST/DAST scanning in CI/CD
5. ❌ Container scanning automation

### **Month 3 Goals**
- ✅ Complete container security path (DONE!)
- 🔄 Master CI/CD security patterns
- 🔄 Implement policy-as-code (OPA)
- 🔄 Supply chain security (SBOM)
- 🔄 Q2 Capstone project

---

## 💼 Portfolio Value

### **Resume Bullet Points**
```
✅ "Deployed containerized applications to Kubernetes with 3-replica high availability"
✅ "Implemented network policies to prevent lateral movement and cryptomining attacks"
✅ "Configured pod security contexts with non-root users and dropped capabilities"
✅ "Managed resource limits (CPU/memory) to prevent resource exhaustion"
✅ "Practiced kubectl operations: scaling, self-healing, rolling updates"
```

### **Interview Talking Points**
```
Q: "Have you worked with Kubernetes?"
A: "Yes, I deployed a Node.js application to a local k3s cluster with 3 replicas,
    implemented network policies to block unauthorized egress traffic, and configured
    pod security contexts to enforce non-root execution. I practiced scaling operations
    and tested self-healing by manually deleting pods and watching Kubernetes recreate
    them automatically. While I haven't deployed to production AKS yet due to cost
    constraints (€238/month), I've mastered the core concepts and kubectl commands
    that transfer directly to any Kubernetes environment."

Q: "What's the difference between ACI and Kubernetes?"
A: "ACI is perfect for 1-10 simple containers with per-second billing (€17/month),
    but requires manual scaling and has no self-healing. Kubernetes handles 20+
    containers with built-in orchestration, auto-scaling, load balancing, and self-
    healing, but costs €238/month minimum on Azure. For learning, I use k3s locally
    to practice production Kubernetes patterns without the cost."
```

---

## 🎓 Key Takeaways

1. **Kubernetes is orchestration magic** - One YAML file manages desired state, K8s makes it reality
2. **Self-healing is production-critical** - Pod crashes at 3am? K8s fixes it automatically
3. **Scaling is trivial** - One kubectl command scales from 3→100 replicas in seconds
4. **Security is layered** - Network policies + pod security + resource limits = defense in depth
5. **Local K8s = Free learning** - 95% of AKS features, 0% of the cost
6. **Declarative > Imperative** - Describe what you want (replicas: 3), K8s figures out how
7. **High availability built-in** - Service load balances, unhealthy pods removed automatically
8. **Cost matters in production** - AKS is €238/month, justify before deploying

---

## 🔧 Useful Commands Reference

```bash
# Start k3s (if stopped)
sudo systemctl start k3s

# Stop k3s (save resources)
sudo systemctl stop k3s

# Check k3s status
sudo systemctl status k3s

# View cluster info
sudo kubectl cluster-info

# Get all resources
sudo kubectl get all --all-namespaces

# Describe cluster
sudo kubectl describe nodes

# Clean up namespace (when done)
sudo kubectl delete namespace secure-app

# Uninstall k3s completely
sudo /usr/local/bin/k3s-uninstall.sh
```

---

## 📚 Further Learning

### **Recommended Next**
1. Kubernetes Patterns: ConfigMaps and Secrets
2. Multi-container pods (sidecar pattern)
3. StatefulSets (for databases)
4. Persistent Volumes (storage)
5. Ingress Controllers (advanced routing)
6. Helm (package manager for K8s)

### **Resources**
- [Kubernetes Official Docs](https://kubernetes.io/docs/)
- [k3s Documentation](https://docs.k3s.io/)
- [kubectl Cheat Sheet](https://kubernetes.io/docs/reference/kubectl/cheatsheet/)
- [Network Policies](https://kubernetes.io/docs/concepts/services-networking/network-policies/)

---

**Session #22 Complete!** 🎉  
**Cost:** €0  
**Value:** Priceless (€238/month AKS knowledge for free!)  
**Status:** Ready for DevSecOps deep dive!

**You just learned what companies pay €238/month for... for FREE!** 🚀
