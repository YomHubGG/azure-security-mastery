# 🔐 Day 21: Application Security in Azure

**Date**: September 22, 2025  
**Focus**: Azure App Service security and application protection  
**Duration**: 1-2 hours  
**Session**: #11 (according to every-other-day schedule)

## 🔍 **Current Environment Discovery**

**Existing App Service**: `app-secureapp-dev-rubf4v`
- **Status**: ✅ Running (West Europe)
- **Plan**: `asp-secureapp-dev` (Linux, Free Tier)
- **HTTPS Only**: ✅ Enabled (good security baseline)
- **Kind**: `app,linux` (Linux container-based)

## 🎯 **Today's Learning Goals** 

- ✅ Analyze current Azure App Service security posture
- ✅ Implement Azure AD authentication
- ✅ Configure security headers and HTTPS enforcement
- ✅ Set up Web Application Firewall (WAF) concepts
- ✅ Explore API security and access controls

## 📚 **What is Application Security?**

Application security in Azure focuses on protecting web applications, APIs, and services from threats like:
- **OWASP Top 10** vulnerabilities
- **DDoS attacks** and traffic flooding
- **Data breaches** through insecure APIs
- **Authentication bypass** and privilege escalation
- **Injection attacks** (SQL, XSS, etc.)

### **Key Security Layers**:
1. **Transport Security** - HTTPS, TLS, certificate management
2. **Authentication & Authorization** - Azure AD, RBAC, OAuth
3. **Application Layer Protection** - WAF, input validation, security headers
4. **Network Security** - VNets, private endpoints, traffic filtering
5. **Data Protection** - Encryption at rest/transit, Key Vault integration

## 🔍 **Current Security Assessment**

### **Our App Service Security Status**:
- ✅ **HTTPS Only**: Enabled (traffic automatically redirected)
- ❌ **Authentication**: Disabled (public access allowed)
- ✅ **SSL Certificate**: Azure-managed (*.azurewebsites.net)
- ❓ **Security Headers**: Need to verify
- ❓ **WAF Protection**: Not configured (requires higher tier)

### **Security Baseline Analysis**:
```bash
# Current findings from our discovery:
Name: app-secureapp-dev-rubf4v
Status: Running (good)
HTTPS Only: True (✅ secure)
Authentication: False (❌ security gap)
Plan: Free Tier (⚠️ limited security features)
```

## 🏗️ **Today's Focus Areas**

### **Phase 1: Transport Security**
- HTTPS enforcement and SSL/TLS configuration
- Security headers implementation
- Custom domain and certificate management

### **Phase 2: Authentication & Authorization**
- Azure AD integration and configuration
- Multi-factor authentication setup
- RBAC and access policies

### **Phase 3: Application Layer Protection**
- Input validation and sanitization
- Security headers configuration
- CORS and API security

### **Phase 4: Integration Security**
- Key Vault integration (✅ already configured!)
- Storage account secure access
- Database connection security

## 🔧 **Phase 1: Transport Security Deep Dive**

### **HTTPS Enforcement Analysis**
Our app already has HTTPS enforcement enabled, which means:
- All HTTP traffic is automatically redirected to HTTPS
- SSL/TLS encryption protects data in transit
- Azure-managed certificate handles *.azurewebsites.net domain

```bash
# Verify HTTPS-only setting
az webapp show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --query "httpsOnly"
# Result: true ✅
```

### **Security Headers Configuration**
Security headers protect against common web vulnerabilities:

**Critical Headers to Implement**:

1. **`Strict-Transport-Security` (HSTS)** - Force HTTPS
   - Forces browsers to only use HTTPS for future requests
   - Prevents downgrade attacks and man-in-the-middle

2. **`X-Content-Type-Options: nosniff`** - Prevent MIME sniffing
   - Stops browsers from "guessing" file types based on content
   - **Attack prevented**: Malicious files disguised as images executing as JavaScript
   - **Example**: Attacker uploads "innocent.jpg" containing `<script>` tags

3. **`X-Frame-Options: DENY`** - Prevent clickjacking
   - Prevents your site from being embedded in iframes on other sites
   - **Attack prevented**: Users tricked into clicking invisible buttons
   - **Example**: Fake "Win $1000!" button hiding real "Transfer Money" button

4. **`X-XSS-Protection: 1; mode=block`** - XSS attack protection
   - Browser's built-in XSS filter stops malicious script execution
   - Legacy protection (modern CSP is better)

5. **`Content-Security-Policy`** - Script execution control
   - Controls which scripts, styles, and resources can load
   - Most powerful modern security header

```bash
# Check current security headers (need to test with curl)
curl -I https://app-secureapp-dev-rubf4v.azurewebsites.net
```

## 🔧 **Phase 2: Current Integration Analysis**

### **Existing Security Integrations** ✅
Our app service already has secure integrations configured:

1. **Key Vault Integration**:
   - URL: `https://kv-rubf4vld.vault.azure.net/`
   - Purpose: Secure secrets management
   - Status: ✅ Configured

2. **Storage Account Integration**:
   - Name: `stsecureappdevrubf4v`
   - Purpose: Application data storage
   - Security: Needs verification

3. **App Service Storage**:
   - Setting: Disabled (good for containers)
   - Purpose: Prevents persistent storage in container

## 🔧 **Phase 3: Security Headers Analysis**

### **Current Security Headers Assessment**
From our testing, the app is currently missing several critical security headers:

**✅ Present**:
- `Secure` cookies (ARRAffinity cookie has Secure flag)
- `HttpOnly` cookies (prevents XSS cookie theft)
- `SameSite=None` for cross-site compatibility

**❌ Missing Critical Headers**:
- `Strict-Transport-Security` (HSTS)
- `X-Content-Type-Options: nosniff`
- `X-Frame-Options: DENY`
- `X-XSS-Protection: 1; mode=block`
- `Content-Security-Policy`

### **Security Headers Implementation**
These headers should be implemented at the application level (in your Express.js app):

```javascript
// Security headers middleware (for reference)
app.use((req, res, next) => {
  res.setHeader('Strict-Transport-Security', 'max-age=31536000; includeSubDomains');
  res.setHeader('X-Content-Type-Options', 'nosniff');
  res.setHeader('X-Frame-Options', 'DENY');
  res.setHeader('X-XSS-Protection', '1; mode=block');
  res.setHeader('Content-Security-Policy', "default-src 'self'");
  next();
});
```

### **Attack Examples & Prevention**

**MIME Sniffing Attack Scenario**:
```bash
# 1. Attacker uploads malicious file disguised as image
echo 'GIF89a<script>alert("XSS via MIME sniffing!")</script>' > malicious.jpg

# 2. Server serves with image MIME type
Content-Type: image/jpeg

# 3. Browser without protection might execute JavaScript
# 4. With X-Content-Type-Options: nosniff → Attack blocked!
```

**Clickjacking Attack Scenario**:
```html
<!-- Malicious site embedding your banking app -->
<iframe src="https://yourbank.com/transfer" style="opacity:0">
</iframe>
<button>🎉 Click for Free Prize! 🎉</button>

<!-- User clicks "prize" button, actually clicks "transfer money" -->
<!-- With X-Frame-Options: DENY → Attack blocked! -->
```

## 🔧 **Phase 4: Authentication & Authorization**

### **Current Authentication Status**
- **Azure AD Integration**: ❌ Not configured
- **Anonymous Access**: ✅ Currently allowed
- **Authentication Provider**: None set

### **Azure AD Authentication Setup**
Let's explore how to configure Azure AD authentication (theory and commands):

```bash
# Enable Azure AD authentication
az webapp auth update \
  --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --enabled true \
  --action LoginWithAzureActiveDirectory \
  --aad-client-id YOUR_APP_REGISTRATION_ID \
  --aad-client-secret YOUR_CLIENT_SECRET \
  --aad-tenant-id $(az account show --query tenantId -o tsv)
```

**⚠️ Important**: This requires creating an Azure AD App Registration first, which we'll explore in theory.

## 🔧 **Phase 5: CORS & API Security Deep Dive**

### **What is CORS (Cross-Origin Resource Sharing)?**
CORS controls how web pages from one domain can access resources from another domain. It's a critical security feature that prevents malicious websites from stealing your data.

**The Security Problem:**
```javascript
// Without CORS protection, malicious site could do:
// From https://evil.com:
fetch('https://yourbank.com/api/transfer', {
  method: 'POST',
  body: JSON.stringify({ to: 'attacker', amount: 10000 }),
  credentials: 'include'  // Sends victim's cookies!
});
```

### **Same-Origin Policy (Browser Security)**
Browsers block cross-origin requests by default:
- ✅ `https://yourapp.com` → `https://yourapp.com/api` (Same origin - ALLOWED)
- ❌ `https://yourapp.com` → `https://evil.com/api` (Different origin - BLOCKED)

**Origins differ by**: Protocol (`http`/`https`), Domain (`app.com`/`evil.com`), Port (`:3000`/`:8080`)

### **Secure CORS Configuration**
```bash
# ✅ SECURE: Specific domains only
az webapp cors add --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 \
  --allowed-origins "https://yourfrontend.com"

# ❌ DANGEROUS: Wildcard allows ANY website!
az webapp cors add --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 \
  --allowed-origins "*"
```

### **CORS Headers Explained**
```http
Access-Control-Allow-Origin: https://trusted.com     # ✅ Specific domain
Access-Control-Allow-Methods: GET, POST              # ✅ Limited methods  
Access-Control-Allow-Headers: Content-Type           # ✅ Limited headers
Access-Control-Allow-Credentials: false              # ✅ No cookies (safer)
```

### **Current CORS Assessment**
```bash
# Check current CORS configuration
az webapp cors show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --output json

# Managed identity status (✅ already configured!)
az webapp identity show --name app-secureapp-dev-rubf4v --resource-group rg-security-learning-001 --output table
```

## 🔧 **Phase 6: Network Access Restrictions**

### **IP-Based Access Control**
Azure App Service allows you to restrict access by IP address or network range:

```bash
# Add IP restriction (whitelist specific IPs)
az webapp config access-restriction add \
  --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --rule-name "office-network" \
  --action Allow \
  --ip-address "203.0.113.0/24" \
  --priority 100

# Block specific IP ranges
az webapp config access-restriction add \
  --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 \
  --rule-name "block-suspicious" \
  --action Deny \
  --ip-address "198.51.100.0/24" \
  --priority 200

# Check current access restrictions
az webapp config access-restriction show \
  --name app-secureapp-dev-rubf4v \
  --resource-group rg-security-learning-001 --output table
```

## 🔧 **Phase 7: Managed Identity Integration** ✅

### **Current Managed Identity Status**
Your app already has **System-Assigned Managed Identity** configured:
- **Principal ID**: `681313d3-7e72-46b8-b994-be681ec95d8e`
- **Tenant ID**: `286ee762-df63-4515-be88-d6a2032dfe6f`
- **Type**: SystemAssigned ✅

### **Why Managed Identity is Secure**
- **No secrets in code**: App authenticates to Azure services without storing credentials
- **Automatic rotation**: Azure handles credential lifecycle
- **Least privilege**: Grant only specific permissions needed

### **Managed Identity Best Practices**
```bash
# Grant Key Vault access to managed identity
az keyvault set-policy \
  --name kvlearning4uybw3c2lbkwm \
  --object-id 681313d3-7e72-46b8-b994-be681ec95d8e \
  --secret-permissions get list

# Grant storage account access
az role assignment create \
  --assignee 681313d3-7e72-46b8-b994-be681ec95d8e \
  --role "Storage Blob Data Reader" \
  --scope "/subscriptions/$(az account show --query id -o tsv)/resourceGroups/rg-security-learning-001/providers/Microsoft.Storage/storageAccounts/stsecureappdevrubf4v"
```
- Managed identity implementation
- Network access restrictions

### **Phase 2: Authentication & Authorization**
- Azure AD integration
- Multi-factor authentication
- Role-based access control (RBAC)

### **Phase 3: Network Protection**
- SSL/TLS configuration
- Custom domains with certificates
- WAF policies and rules

## 🛠️ **Prerequisites**
- Completed Days 1-19 (security fundamentals)
- Basic understanding of web applications
- Azure AD tenant access

## 📋 **Learning Resources**

### **Microsoft Learn Paths**:
- [Secure Azure App Service](https://docs.microsoft.com/learn/paths/secure-azure-app-service/)
- [Implement authentication in applications](https://docs.microsoft.com/learn/paths/implement-authentication-applications/)

### **Hands-On Labs**:
- Deploy secure web application
- Configure Azure AD authentication
- Implement WAF protection

## 🎯 **Expected Outcomes**

By the end of this session, you'll understand:
- ✅ How to secure Azure App Service applications
- ✅ Azure AD authentication implementation
- ✅ SSL/TLS certificate management
- ✅ WAF configuration and monitoring
- ✅ API security best practices

---

**Next Session**: Day 23 - DevSecOps & CI/CD Security (September 24, 2025)