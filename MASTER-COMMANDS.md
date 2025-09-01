# 🎯 Azure Security Mastery - Master Command Reference

## 🔧 Essential Daily Commands

### Authentication
```bash
az logout && az login --use-device-code
az account show
```

### Resource Management
```bash
az resource list --resource-group RESOURCE_GROUP --output table
```

### Bicep/IaC Operations
```bash
# Validate template
az deployment group validate --resource-group RG_NAME --template-file template.bicep --parameters @params.json

# Preview changes
az deployment group what-if --resource-group RG_NAME --template-file template.bicep --parameters @params.json

# Deploy
az deployment group create --resource-group RG_NAME --template-file template.bicep --parameters @params.json
```

### Container Operations (Podman)
```bash
# Build secure container
podman build -t app-name:version -f Dockerfile.secure .

# Run locally
podman run -d -p 3000:3000 --name test-container app-name:version

# Test endpoints
curl http://localhost:3000
curl http://localhost:3000/health

# Cleanup
podman stop test-container && podman rm test-container
```

### Azure Container Registry (ACR)
```bash
# Deploy ACR with IaC
az deployment group create --resource-group RG_NAME --template-file azure-container-registry.bicep --parameters @acr-dev.parameters.json

# Get access token for Podman
az acr login --name REGISTRY_NAME --expose-token

# Authenticate Podman (use token from above)
podman login REGISTRY_NAME.azurecr.io -u 00000000-0000-0000-0000-000000000000 -p ACCESS_TOKEN

# Tag and push
podman tag app-name:version REGISTRY_NAME.azurecr.io/app-name:version  
podman push REGISTRY_NAME.azurecr.io/app-name:version

# List repositories and tags
az acr repository list --name REGISTRY_NAME --output table
az acr repository show-tags --name REGISTRY_NAME --repository app-name --output table

# Enable admin user (for ACI integration)
az acr update --name REGISTRY_NAME --admin-enabled true
```

### Azure Container Instances (ACI)
```bash
# Deploy container to cloud
az deployment group create --resource-group RG_NAME --template-file container-instance.bicep --parameters @aci-dev.parameters.json

# Test running container
curl http://CONTAINER_IP:3000
curl http://CONTAINER_IP:3000/health
```

## 🎯 Current Infrastructure
- **Networks**: dev-vnet-security (10.0.0.0/16), prod-vnet-security
- **Security**: dev-nsg-security, prod-nsg-security  
- **Storage**: Multiple secure storage accounts
- **Containers**: 
  - Registry: devacr4uybw3c2lbkwm.azurecr.io/secure-app:v1
  - Running: http://4.175.207.4:3000 (dev-secure-container)

## 📋 Session Progress
- [x] **Phase 1**: IaC Foundations (Bicep, multi-resource templates)
- [x] **Phase 2**: Network Security (VNets, NSGs, subnetting)  
- [x] **Phase 3**: Container Security (Podman, ACR, ACI) ✅ **COMPLETED Sept 1**
- [ ] **Phase 4**: Application Security (Key Vault, web apps) ← **NEXT SESSION**
- [ ] **Phase 5**: Compliance & Governance (Policy, RBAC)

*Updated: September 1, 2025*
