# 🐳 Day 39: Azure Container Instances Security

**Date**: October 23, 2025  
**Session**: #20  
**Focus**: Deploy YOUR published container to Azure with security best practices  
**Duration**: 1-2 hours  

## 🎯 **Today's Learning Goals** 

- ✅ Understand Azure Container Instances (ACI) architecture
- ✅ **Deploy YOUR container** (`ghcr.io/yomhubgg/secure-app:1.0.0`) to Azure
- ✅ Configure ACI security features
- ✅ Manage secrets and environment variables securely
- ✅ Implement networking and access controls
- ✅ Set up monitoring and logging
- ✅ Optimize for €0 cost using free tier

## 🚀 **Complete Container Workflow**

This session completes your end-to-end container journey:
- **Day 35**: Built secure Docker image with 10+ hardening measures
- **Day 37**: Pushed to GitHub Container Registry
- **Day 39**: **Deploy to Azure cloud** ← YOU ARE HERE!

Perfect progression showing:
- ✅ Container security expertise
- ✅ Registry management
- ✅ Cloud deployment skills
- ✅ Cost-conscious engineering

## 📚 What is Azure Container Instances (ACI)?

### **The Basics**

**Azure Container Instances** = **Serverless containers** in Azure

- Upload your container → Azure runs it → You get a URL
- No VMs, no clusters, no infrastructure management
- Pay only for what you use (per second!)
- Perfect for simple workloads, testing, demos

### **Why ACI for Day 39?**

✅ **Simplest Azure container service** (great for learning)  
✅ **FREE seconds available** (no credit card charges!)  
✅ **Deploy in minutes** (vs hours for AKS)  
✅ **You already have a container ready!** (`secure-app:1.0.0`)  
✅ **Natural progression** after registry learning (Day 37)

---

## 🏗️ **Quick Start: Deploy YOUR Container**

### **Prerequisites**
### **Prerequisites**

```bash
# 1. Azure CLI logged in
az account show

# 2. Your container is published
# Verify at: https://github.com/YomHubGG?tab=packages

# 3. GitHub PAT ready (if container is private)
export GITHUB_TOKEN="your_personal_access_token_here"
```

---

### **Method 1: Quick Deploy (Public Container)**

If you made your container public on GitHub:

```bash
# Create resource group
az group create \
  --name rg-containers-learning \
  --location westeurope

# Deploy container
az container create \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --image ghcr.io/yomhubgg/secure-app:1.0.0 \
  --cpu 0.5 \
  --memory 0.5 \
  --dns-name-label yom-secure-app \
  --ports 3000 \
  --restart-policy OnFailure

# Get your URL
az container show \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --query ipAddress.fqdn \
  --output tsv
# Output: yom-secure-app.westeurope.azurecontainer.io

# Test it!
curl http://yom-secure-app.westeurope.azurecontainer.io:3000
```

---

### **Method 2: Secure Deploy (Private Container)**

If your container is private (recommended):

```bash
# Deploy with GitHub Container Registry credentials
az container create \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --image ghcr.io/yomhubgg/secure-app:1.0.0 \
  --registry-login-server ghcr.io \
  --registry-username yomhubgg \
  --registry-password $GITHUB_TOKEN \
  --cpu 0.5 \
  --memory 0.5 \
  --dns-name-label yom-secure-app \
  --ports 3000 \
  --restart-policy OnFailure \
  --secure-environment-variables \
    NODE_ENV=production \
    PORT=3000
```

---

## 🔒 Security Best Practices for ACI

### **1. Registry Authentication**

```bash
# ❌ DON'T: Hardcode passwords
--registry-password "my-secret-token"

# ✅ DO: Use environment variables
--registry-password $GITHUB_TOKEN

# ✅ BEST: Use Azure Key Vault
--registry-password $(az keyvault secret show ...)
```

### **2. Resource Limits** (Prevent resource exhaustion)

```bash
# Always set CPU and memory limits
--cpu 0.5      # Half a core (enough for testing)
--memory 0.5   # 500 MB (your container is 354 MB)

# Don't over-provision!
# ❌ --cpu 4 --memory 8  (wastes money)
```

### **3. Network Security**

```bash
# Public access (for demos)
--ip-address Public --dns-name-label yom-secure-app --ports 3000

# Private access (production)
--vnet /subscriptions/.../vnet-name \
--subnet /subscriptions/.../subnet-name
```

### **4. Secrets Management**

```bash
# ❌ DON'T: Plain environment variables
--environment-variables DB_PASSWORD=secret123

# ✅ DO: Secure environment variables
--secure-environment-variables DB_PASSWORD=secret123

# ✅ BEST: Azure Key Vault integration
--secrets $(az keyvault secret show ...)
```

---

## 📊 Monitoring & Debugging

### **View Logs**

```bash
# Stream live logs
az container logs \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --follow

# Get all logs
az container logs \
  --resource-group rg-containers-learning \
  --name secure-app-demo
```

### **Execute Commands**

```bash
# Get shell access
az container exec \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --exec-command "/bin/sh"

# Run one-off command
az container exec \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --exec-command "ps aux"
```

### **Check Status**

```bash
# Get container state
az container show \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --query "{FQDN:ipAddress.fqdn, State:instanceView.state, IP:ipAddress.ip}" \
  --output table
```

---

## 💰 Cost Management

### **Free Tier Usage**

```
FREE: First 50,000 vCPU-seconds per month

Your demo (0.5 CPU for 10 minutes):
= 0.5 × 600 seconds = 300 vCPU-seconds
= Still 49,700 seconds remaining!

Cost: €0.00 ✅
```

### **Always Clean Up!**

```bash
# Delete container when done
az container delete \
  --resource-group rg-containers-learning \
  --name secure-app-demo \
  --yes

# Or delete entire resource group
az group delete \
  --name rg-containers-learning \
  --yes \
  --no-wait
```

---

## 🎯 What You'll Achieve Today

By the end of Day 39:

✅ **Deployed YOUR container to Azure cloud**  
✅ **Configured secure authentication** (GitHub CR → ACI)  
✅ **Managed secrets properly** (secure environment variables)  
✅ **Monitored logs and metrics**  
✅ **Executed debugging commands**  
✅ **Cost: €0** (within free tier!)  

**Portfolio Impact**:
- End-to-end container workflow demonstrated
- Cloud deployment experience
- Security-first mindset
- Cost-conscious engineering

---

## 🚀 Ready to Deploy?

Let's start with the hands-on deployment! Choose your method:
1. Public container (simpler, good for demos)
2. Private container (more secure, production-like)

**Which would you like to try first?** �☁️