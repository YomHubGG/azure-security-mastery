# 🔍 Day 21: Application Security Assessment Lab

## 🎯 **Lab Objective**
Perform a comprehensive security assessment of your Azure App Service and identify security improvements.

## 📋 **Security Checklist Assessment**

### **Phase 1: Transport Security** ✅
```bash
# ✅ HTTPS Enforcement Check
az webapp show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --query "httpsOnly"
# Expected: true ✅

# Test SSL/TLS configuration
curl -I https://app-secureapp-dev-rubf4v.azurewebsites.net | grep -E "(HTTP|Server|Strict-Transport)"
```

**Results:**
- ✅ HTTPS Only: Enabled
- ✅ SSL Certificate: Azure-managed
- ❌ HSTS Header: Missing (should add in application code)

### **Phase 2: Authentication & Access Control**
```bash
# Authentication status
az webapp auth show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --query "enabled"
# Expected: false (currently public access)

# Managed identity verification
az webapp identity show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --query "type"
# Expected: SystemAssigned ✅
```

**Results:**
- ❌ Azure AD Authentication: Disabled (security gap)
- ✅ Managed Identity: Configured (good for service-to-service auth)

### **Phase 3: Network Security**
```bash
# Check access restrictions
az webapp config access-restriction show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001

# Check CORS configuration
az webapp cors show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001
```

**Results:**
- ❓ Access Restrictions: None configured (allows global access)
- ❓ CORS Settings: Need to verify configuration

### **Phase 4: Application-Level Security**
```bash
# Test security headers
curl -I https://app-secureapp-dev-rubf4v.azurewebsites.net
```

**Security Headers Assessment:**
- ❌ Strict-Transport-Security: Missing
- ❌ X-Content-Type-Options: Missing  
- ❌ X-Frame-Options: Missing
- ❌ Content-Security-Policy: Missing
- ✅ Secure Cookies: Present (ARRAffinity)

## 🛡️ **Security Recommendations**

### **High Priority (Critical)**
1. **Implement Security Headers** (Application Level)
   ```javascript
   // Add to Express.js application
   app.use(helmet({
     hsts: { maxAge: 31536000, includeSubDomains: true },
     contentTypeOptions: { nosniff: true },
     frameguard: { action: 'deny' },
     xssFilter: true
   }));
   ```

2. **Enable Azure AD Authentication** (Azure Level)
   ```bash
   # Requires Azure AD App Registration first
   az webapp auth update --name app-secureapp-dev-rubf4v \
     --resource-group rg-security-learning-001 --enabled true
   ```

### **Medium Priority (Important)**
3. **Configure CORS Properly**
   ```bash
   # Replace wildcard with specific domains
   az webapp cors add --name app-secureapp-dev-rubf4v \
     --resource-group rg-security-learning-001 \
     --allowed-origins "https://yourtrustedsite.com"
   ```

4. **Add Network Access Restrictions**
   ```bash
   # Restrict to office/trusted networks
   az webapp config access-restriction add \
     --name app-secureapp-dev-rubf4v \
     --resource-group rg-security-learning-001 \
     --rule-name "trusted-networks" \
     --action Allow --ip-address "YOUR-OFFICE-IP/32"
   ```

### **Low Priority (Enhancement)**
5. **Custom Domain with SSL Certificate**
6. **Web Application Firewall (requires Premium tier)**
7. **Private Endpoints (requires Premium tier)**

## 🔧 **Practical Security Improvements**

### **Safe Changes (No Risk)**
```bash
# 1. Add application settings for security
az webapp config appsettings set --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --settings "ENABLE_SECURITY_HEADERS=true" "API_RATE_LIMIT=100"

# 2. Verify managed identity permissions
az keyvault show --name kvlearning4uybw3c2lbkwm \
  --query "properties.accessPolicies[?objectId=='681313d3-7e72-46b8-b994-be681ec95d8e']"
```

### **Changes Requiring Caution (Test First)**
```bash
# 3. Configure specific CORS origins (test with your frontend)
az webapp cors add --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --allowed-origins "https://localhost:3000"

# 4. Add IP restrictions (test with your current IP first)
MY_IP=$(curl -s https://api.ipify.org)
az webapp config access-restriction add \
  --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --rule-name "my-ip" --action Allow \
  --ip-address "$MY_IP/32" --priority 100
```

## 📊 **Security Score Assessment**

**Current Security Score: 6/10**

**Breakdown:**
- ✅ HTTPS Enforcement (2/2 points)
- ✅ Managed Identity (2/2 points)  
- ✅ Key Vault Integration (1/1 points)
- ✅ Secure Cookies (1/1 points)
- ❌ Authentication (0/2 points)
- ❌ Security Headers (0/1 points)
- ❌ Network Restrictions (0/1 points)

**Target Score: 9/10** (achievable with free tier)

## 🎯 **Next Steps**
1. **Immediate**: Add security headers to application code
2. **Short-term**: Configure Azure AD authentication
3. **Medium-term**: Implement CORS and network restrictions
4. **Long-term**: Consider WAF and private endpoints (requires paid tiers)

---

**Lab Duration**: 30-45 minutes  
**Difficulty**: Intermediate  
**Cost Impact**: Free (all recommendations use free tier features)