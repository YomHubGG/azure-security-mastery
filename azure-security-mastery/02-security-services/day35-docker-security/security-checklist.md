# 🔒 Container Security Checklist

A comprehensive checklist for building and deploying secure containers.

---

## 📋 Pre-Build Security Checklist

### **1. Base Image Selection**
- [ ] Using official images from trusted sources (Docker Hub Official, MCR)
- [ ] Base image is Alpine/Distroless (minimal attack surface)
- [ ] Version is pinned (not using `:latest`)
- [ ] Base image is actively maintained (check last update date)
- [ ] Known vulnerabilities checked (Trivy scan)

**Example**:
```dockerfile
✅ FROM node:18.17.1-alpine3.18
❌ FROM node:latest
❌ FROM ubuntu
```

---

### **2. Dockerfile Best Practices**
- [ ] Multi-stage build implemented (separate build and runtime)
- [ ] Non-root user created and used
- [ ] No secrets in Dockerfile (use environment variables)
- [ ] Minimal layers (combined RUN commands where possible)
- [ ] `.dockerignore` file exists and excludes sensitive files
- [ ] Health check configured
- [ ] CMD uses array syntax (not shell form)

**Example**:
```dockerfile
✅ CMD ["node", "server.js"]
❌ CMD node server.js  # Shell form (vulnerable to injection)
```

---

### **3. Secrets Management**
- [ ] No `.env` files copied into image
- [ ] No hardcoded passwords/tokens/API keys
- [ ] Secrets passed as environment variables at runtime
- [ ] `.dockerignore` excludes all secret files
- [ ] Build logs don't expose secrets

**Files to exclude**:
```
.env
.env.*
*.key
*.pem
secrets/
credentials/
```

---

## 🏗️ Build Security Checklist

### **4. Image Hardening**
- [ ] Security updates applied (`apk upgrade` or `apt-get upgrade`)
- [ ] Unnecessary packages removed
- [ ] Package manager cache cleaned
- [ ] Development dependencies excluded from production image
- [ ] Build artifacts removed after build

**Example**:
```dockerfile
RUN apk update && \
    apk upgrade --no-cache && \
    rm -rf /var/cache/apk/*
```

---

### **5. File System Security**
- [ ] Application files owned by non-root user
- [ ] Application files set to read-only (chmod 555)
- [ ] Writable temp directory created with restricted permissions (700)
- [ ] No world-writable directories

**Example**:
```dockerfile
RUN chmod -R 555 /app && \
    mkdir -p /tmp/app && \
    chown -R nodejs:nodejs /tmp/app && \
    chmod 700 /tmp/app
```

---

### **6. User Security**
- [ ] Non-root user created with specific UID/GID
- [ ] User has no shell access (`/sbin/nologin`)
- [ ] USER directive set before CMD
- [ ] User has minimal privileges

**Example**:
```dockerfile
RUN addgroup -g 1001 -S nodejs && \
    adduser -S -u 1001 -G nodejs -s /sbin/nologin nodejs
USER nodejs
```

---

## 🔍 Scan & Test Security Checklist

### **7. Vulnerability Scanning**
- [ ] Image scanned with Trivy (or equivalent)
- [ ] No CRITICAL vulnerabilities present
- [ ] HIGH vulnerabilities documented and mitigated
- [ ] Scan report saved for audit trail
- [ ] Regular rescans scheduled (weekly/monthly)

**Scanning tools**:
- Trivy (Open Source)
- Snyk
- Azure Defender for Containers
- Clair

---

### **8. Container Testing**
- [ ] Container starts successfully
- [ ] Health check endpoint responds correctly
- [ ] Application logs show no errors
- [ ] Container runs as non-root (verify with `podman exec ... whoami`)
- [ ] No exposed secrets in logs
- [ ] Memory/CPU limits tested

**Test commands**:
```bash
# Start container
podman run -d --name test-app secure-app:1.0.0

# Check user
podman exec test-app whoami  # Should NOT be root

# Check health
curl http://localhost:3000/health

# Check logs
podman logs test-app
```

---

## 🚀 Runtime Security Checklist

### **9. Deployment Configuration**
- [ ] Environment variables used for configuration
- [ ] Secrets injected at runtime (Azure Key Vault, Kubernetes Secrets)
- [ ] Resource limits set (CPU, memory)
- [ ] Network policies defined
- [ ] Read-only root filesystem enabled (if possible)
- [ ] Capabilities dropped (if using Docker/Podman security options)

**Example (Podman)**:
```bash
podman run -d \
  --name secure-app \
  --read-only \
  --cap-drop=ALL \
  --cap-add=NET_BIND_SERVICE \
  --memory=512m \
  --cpus=1 \
  -e NODE_ENV=production \
  secure-app:1.0.0
```

---

### **10. Network Security**
- [ ] Only necessary ports exposed
- [ ] TLS/HTTPS configured for external communication
- [ ] Internal communication uses private networks
- [ ] Ingress/egress rules defined
- [ ] No direct internet access from container (if not needed)

---

### **11. Monitoring & Logging**
- [ ] Container logs centralized (Azure Monitor, ELK)
- [ ] Health checks configured and monitored
- [ ] Alerts set for failed health checks
- [ ] Resource usage monitored (CPU, memory, disk)
- [ ] Security events logged (failed auth, suspicious activity)

---

## ☁️ Azure-Specific Security Checklist

### **12. Azure Container Registry (ACR)**
- [ ] Registry access restricted to VNet (private endpoint)
- [ ] RBAC configured (least privilege)
- [ ] Managed identity used for authentication (no passwords)
- [ ] Content trust enabled (image signing)
- [ ] Vulnerability scanning enabled (Defender for Containers)
- [ ] Immutable tags configured (prevent overwriting)

---

### **13. Azure Container Instances (ACI)**
- [ ] Managed identity enabled
- [ ] Virtual network integration configured
- [ ] Environment variables used for secrets (not in YAML)
- [ ] Resource limits defined
- [ ] Restart policy configured
- [ ] Logs sent to Azure Monitor

---

### **14. Azure Kubernetes Service (AKS)**
- [ ] Pod security standards enforced (restricted)
- [ ] Network policies enabled
- [ ] RBAC configured
- [ ] Azure Policy for Kubernetes enabled
- [ ] Secrets stored in Azure Key Vault (not Kubernetes Secrets)
- [ ] Container image pulls use managed identity
- [ ] Admission controllers enabled (Pod Security Admission)

---

## 📊 Security Scoring Guide

### **Critical (Must Fix Immediately)**
- Running as root
- CRITICAL vulnerabilities in image
- Secrets in image or logs
- No authentication/authorization

### **High (Fix Within 7 Days)**
- HIGH vulnerabilities in image
- Missing health checks
- Excessive privileges
- No resource limits

### **Medium (Fix Within 30 Days)**
- MEDIUM vulnerabilities
- Large image size (>500MB)
- Missing monitoring
- No network policies

### **Low (Fix When Convenient)**
- LOW vulnerabilities
- Missing labels/metadata
- Optimization opportunities

---

## ✅ Quick Validation Commands

```bash
# Check image size
podman images secure-app:1.0.0

# Check user (should be 1001 or nodejs)
podman inspect secure-app:1.0.0 --format '{{.Config.User}}'

# Check vulnerabilities
trivy image secure-app:1.0.0

# Check running container user
podman exec <container-id> whoami

# Check container resources
podman stats <container-id>

# Check exposed ports
podman port <container-id>
```

---

## 🎯 Checklist Summary

**Before Building**:
- [ ] Secure Dockerfile created
- [ ] .dockerignore configured
- [ ] No secrets in source code

**After Building**:
- [ ] Image scanned (no CRITICAL/HIGH vulnerabilities)
- [ ] Image size optimized (<200MB for Node.js)
- [ ] Non-root user verified

**Before Deploying**:
- [ ] Runtime configuration secured
- [ ] Monitoring configured
- [ ] Network policies defined

**After Deploying**:
- [ ] Health checks passing
- [ ] Logs reviewed (no errors/secrets)
- [ ] Regular scans scheduled

---

## 📚 References

- [CIS Docker Benchmark](https://www.cisecurity.org/benchmark/docker)
- [OWASP Docker Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Docker_Security_Cheat_Sheet.html)
- [Azure Container Security Best Practices](https://learn.microsoft.com/azure/container-instances/container-instances-security)
- [NIST Application Container Security Guide](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-190.pdf)

---

*Use this checklist for every container you build! 🔒*
