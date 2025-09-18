# 🐳 Day 39: Azure Container Instances + Minishell Demo

**Date**: October 9, 2025 (Scheduled)  
**Focus**: Serverless container security with practical minishell deployment  
**Duration**: 1-2 hours  
**Session**: #20 (according to every-other-day schedule)

## 🎯 **Today's Learning Goals** 

- ✅ Deploy secure container groups with ACI
- ✅ **PRACTICAL PROJECT: Deploy minishell container securely**
- ✅ Implement virtual network integration
- ✅ Set up managed identity for container access
- ✅ Configure monitoring and logging
- ✅ Create cost-management scripts (start/stop)
- ✅ Commit: "Serverless container security + minishell demo"

## 🚀 **The Minishell Container Project**

Your **42 minishell** project will serve as the perfect practical demonstration of:
- **Container security best practices**
- **Azure Container Instances deployment**
- **Network security integration**
- **Cost-effective cloud hosting**
- **Monitoring and logging**

## 🏗️ **Implementation Plan**

### **Phase 1: Container Preparation**
```bash
# Build secure container image
docker build -t minishell:secure .
docker tag minishell:secure devacr4uybw3c2lbkwm.azurecr.io/minishell:v1.0

# Push to Azure Container Registry (from Day 37)
az acr login --name devacr4uybw3c2lbkwm
docker push devacr4uybw3c2lbkwm.azurecr.io/minishell:v1.0
```

### **Phase 2: Secure ACI Deployment**
```bicep
// minishell-aci.bicep
resource containerGroup 'Microsoft.ContainerInstance/containerGroups@2023-05-01' = {
  name: 'cg-minishell-secure'
  location: location
  identity: {
    type: 'SystemAssigned'
  }
  properties: {
    containers: [
      {
        name: 'minishell'
        properties: {
          image: 'devacr4uybw3c2lbkwm.azurecr.io/minishell:v1.0'
          ports: [
            {
              port: 80
              protocol: 'TCP'
            }
          ]
          resources: {
            requests: {
              cpu: 1
              memoryInGB: 2
            }
          }
          environmentVariables: [
            {
              name: 'ENVIRONMENT'
              value: 'production'
            }
          ]
        }
      }
    ]
    osType: 'Linux'
    restartPolicy: 'OnFailure'
    ipAddress: {
      type: 'Public'
      ports: [
        {
          port: 80
          protocol: 'TCP'
        }
      ]
    }
  }
}
```

### **Phase 3: Cost Management Integration**
- **start-minishell.sh** - Deploy container when needed
- **stop-minishell.sh** - Remove container to save costs
- **monitor-minishell.sh** - Check logs and metrics

### **Phase 4: Security Features**
- **Virtual Network integration** for private access
- **Managed Identity** for secure resource access
- **Azure Monitor** integration for logging
- **Container Registry** authentication

## 🎯 **Success Criteria**

By Day 39, you'll have:
- ✅ **Production-ready minishell container** in Azure
- ✅ **Secure deployment patterns** demonstrated
- ✅ **Cost-effective hosting** with start/stop automation
- ✅ **Enterprise security** features integrated
- ✅ **Portfolio project** showcasing cloud + security skills

## 💡 **Why This Approach Works**

1. **Educational Value**: Combines your 42 project with Azure skills
2. **Portfolio Impact**: Shows practical cloud deployment experience  
3. **Security Focus**: Demonstrates enterprise security patterns
4. **Cost Conscious**: Automated start/stop for budget management
5. **Resume Ready**: Real-world Azure container experience

**Estimated Timeline**: We're currently on Day 17, so this is 11 sessions (21 days) away!

This will be an awesome capstone for your container security learning! 🚀