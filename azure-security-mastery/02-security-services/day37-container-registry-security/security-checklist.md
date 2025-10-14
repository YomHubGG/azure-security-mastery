# ACR Security Audit Checklist

**Purpose**: Evaluate Azure Container Registry security posture  
**Use**: Annual audits, security reviews, pre-production deployments

---

## 🔐 1. Authentication & Authorization (Weight: 30%)

### Identity Management
- [ ] **Admin user is DISABLED** (except documented breakglass)
  - Command: `az acr update --name <acr> --admin-enabled false`
  - Why: Shared credentials = audit nightmare
  
- [ ] **Managed identities configured for Azure services**
  - AKS clusters: `az aks show --name <aks> --query identityProfile`
  - ACI: Check managed identity in deployment
  - Why: No credentials to manage or steal

- [ ] **Service principals use minimum required role**
  - List: `az role assignment list --all --query "[?principalType=='ServicePrincipal']"`
  - Required roles only: AcrPull, AcrPush, AcrDelete
  - Why: Principle of least privilege

- [ ] **No hardcoded credentials in source code**
  - Check: Git history, CI/CD configs, Dockerfiles
  - Tool: `git grep -i "azurecr.io" -- '*.yml' '*.yaml' '*.json'`
  - Why: Credentials in git = forever compromised

- [ ] **Service principal secrets rotated every 90 days**
  - Document: Last rotation date, next due date
  - Process: Update GitHub/Azure DevOps secrets
  - Why: Limit blast radius of compromised credentials

**Score: ___/5 checks passed**

---

## 🌐 2. Network Security (Weight: 25%)

### Access Controls
- [ ] **Public access is restricted**
  - Check: `az acr show --name <acr> --query publicNetworkAccess`
  - Options: IP allowlist OR disabled entirely
  - Why: Reduce attack surface

- [ ] **IP allowlist configured and documented**
  - List: `az acr network-rule list --name <acr> --output table`
  - Document: Who needs access, why, review date
  - Why: Only known sources can connect

- [ ] **Firewall rules reviewed quarterly**
  - Last review: _____________
  - Next review: _____________
  - Why: Remove stale rules, add new requirements

- [ ] **Private endpoint configured (Premium tier)**
  - Check: `az network private-endpoint list --query "[?privateLinkServiceConnections[0].privateLinkServiceId contains '<acr>']"`
  - Alternative: Public + strict firewall (Standard/Basic)
  - Why: Complete network isolation for production

- [ ] **Virtual network rules implemented**
  - Check: `az acr network-rule list --name <acr>`
  - Verify: Only required subnets have access
  - Why: Defense in depth

- [ ] **Only trusted Azure services allowed if firewall enabled**
  - Check: `az acr show --name <acr> --query "networkRuleSet.defaultAction"`
  - Setting: Deny + specific trusted services
  - Why: Blocks random Azure VMs, allows AKS/ACI

**Score: ___/6 checks passed**

---

## 🛡️ 3. Image Security (Weight: 30%)

### Vulnerability Management
- [ ] **Vulnerability scanning enabled**
  - Check: `az security pricing show --name ContainerRegistry`
  - Required: Microsoft Defender for Containers enabled
  - Why: Automatic CVE detection

- [ ] **Quarantine policy enabled for critical vulnerabilities**
  - Check: `az acr config content-trust show --name <acr>`
  - Setting: Critical/High vulnerabilities block deployment
  - Why: Prevent vulnerable images in production

- [ ] **Images scanned before production deployment**
  - Process: CI/CD pipeline includes scan step
  - Gate: Fail build if critical vulnerabilities found
  - Why: Shift-left security

- [ ] **Content trust (image signing) enabled**
  - Check: `az acr config content-trust show --name <acr> --query status`
  - Required: Premium tier
  - Why: Verify image authenticity, prevent tampering

- [ ] **Base images updated monthly**
  - Check: Review Dockerfile FROM statements
  - Process: Rebuild images with latest base
  - Why: Include latest security patches

- [ ] **Image tags follow semantic versioning**
  - Pattern: app:v1.2.3, app:v1.2.3-20251014
  - Avoid: app:latest (ambiguous)
  - Why: Traceability, rollback capability

**Score: ___/6 checks passed**

---

## 🔄 4. Lifecycle Management (Weight: 10%)

### Retention & Cleanup
- [ ] **Retention policy configured**
  - Check: `az acr config retention show --name <acr>`
  - Setting: Delete untagged manifests after 7-30 days
  - Why: Reduce storage costs, remove orphaned manifests

- [ ] **Production images locked**
  - Check: `az acr repository show --name <acr> --image app:v1.0.0 --query '{write:writeEnabled,delete:deleteEnabled}'`
  - Setting: write=false, delete=false
  - Why: Prevent accidental deletion/overwrite

- [ ] **Old images archived or deleted per policy**
  - Policy: Keep production images X months, dev images Y days
  - Tool: Script to clean up based on tags/dates
  - Why: Compliance requirements, cost optimization

- [ ] **Geo-replication configured (if Premium)**
  - Check: `az acr replication list --name <acr> --output table`
  - Setting: Replicas in required regions
  - Why: Performance, availability

**Score: ___/4 checks passed**

---

## 📊 5. Monitoring & Compliance (Weight: 15%)

### Observability
- [ ] **Activity logs enabled**
  - Check: `az monitor diagnostic-settings list --resource <acr-resource-id>`
  - Send to: Log Analytics workspace
  - Why: Audit trail of all operations

- [ ] **Alerts configured for suspicious activities**
  - Examples: Failed auth attempts, unusual pull patterns
  - Tool: Azure Monitor alerts + Log Analytics
  - Why: Detect potential breaches

- [ ] **Regular security reviews conducted**
  - Frequency: Quarterly (minimum)
  - Last review: _____________
  - Next review: _____________
  - Why: Catch misconfigurations, update policies

- [ ] **Compliance requirements documented**
  - Standards: GDPR, SOC 2, ISO 27001, etc.
  - Evidence: Audit logs, retention policies, encryption
  - Why: Meet regulatory obligations

- [ ] **Disaster recovery plan documented and tested**
  - Backup: Geo-replication OR image export
  - RTO/RPO: Recovery time/point objectives defined
  - Last test: _____________
  - Why: Business continuity

**Score: ___/5 checks passed**

---

## 💰 6. Cost Optimization (Weight: 5%)

### Efficiency
- [ ] **Appropriate tier selected**
  - Basic (€4.60): Learning, single region, <10GB
  - Standard (€18.40): Multiple webhooks, <100GB
  - Premium (€92): Geo-replication, private endpoints, content trust
  - Why: Don't overpay for unused features

- [ ] **Geo-replication justified and documented**
  - Justification: Performance requirement, DR, compliance
  - Regions: Only those with active workloads
  - Why: €92/month per region adds up

- [ ] **Storage usage monitored**
  - Check: `az acr show-usage --name <acr> --output table`
  - Alert: When approaching tier limit
  - Why: Avoid surprise overage charges

- [ ] **Unused images cleaned up**
  - Process: Automated cleanup script
  - Frequency: Weekly/monthly
  - Why: Storage costs scale with usage

- [ ] **Cost alerts configured**
  - Alert: When ACR costs exceed X per month
  - Tool: Azure Cost Management
  - Why: Budget control

**Score: ___/5 checks passed**

---

## 📈 Overall Security Score

### Calculation
```
Total Score = (Auth/5 × 30%) + (Network/6 × 25%) + (Image/6 × 30%) + 
              (Lifecycle/4 × 10%) + (Monitoring/5 × 15%) + (Cost/5 × 5%)

Example:
- Auth: 4/5 = 80% × 30% = 24%
- Network: 5/6 = 83% × 25% = 21%
- Image: 6/6 = 100% × 30% = 30%
- Lifecycle: 3/4 = 75% × 10% = 7.5%
- Monitoring: 4/5 = 80% × 15% = 12%
- Cost: 5/5 = 100% × 5% = 5%

Total: 24 + 21 + 30 + 7.5 + 12 + 5 = 99.5%
```

### Your Score: ______%

### Rating Scale
```
90-100%  🟢 EXCELLENT  - Enterprise-grade security
75-89%   🟡 GOOD       - Minor improvements needed
60-74%   🟠 ADEQUATE   - Several gaps to address
45-59%   🔴 POOR       - Significant security risks
<45%     ⛔ CRITICAL   - Immediate action required
```

---

## 🎯 Priority Remediation

Based on your audit, prioritize fixes:

### 🔴 Critical (Fix Immediately)
Issues that could lead to immediate compromise:
- [ ] Admin user enabled in production
- [ ] No firewall rules (public access unrestricted)
- [ ] Critical vulnerabilities in production images
- [ ] No activity logging

### 🟠 High (Fix Within 7 Days)
Significant security gaps:
- [ ] Service principals with excessive permissions
- [ ] No vulnerability scanning
- [ ] Hardcoded credentials in source code
- [ ] High-severity vulnerabilities unpatched

### 🟡 Medium (Fix Within 30 Days)
Important improvements:
- [ ] Missing content trust (image signing)
- [ ] No retention policies
- [ ] Infrequent security reviews
- [ ] Alerts not configured

### 🟢 Low (Fix When Convenient)
Nice-to-have enhancements:
- [ ] Cost optimization opportunities
- [ ] Better documentation
- [ ] Additional monitoring

---

## 📝 Audit Report Template

```markdown
# ACR Security Audit Report

**ACR Name**: ________________
**Audit Date**: ______________
**Auditor**: _________________
**Next Audit Due**: __________

## Executive Summary
Overall Score: ____%
Rating: [EXCELLENT/GOOD/ADEQUATE/POOR/CRITICAL]

Key Findings:
- ✅ [Strength 1]
- ✅ [Strength 2]
- ❌ [Issue 1]
- ❌ [Issue 2]

## Detailed Findings

### Authentication & Authorization: ___/5 (___%)
[Notes]

### Network Security: ___/6 (___%)
[Notes]

### Image Security: ___/6 (___%)
[Notes]

### Lifecycle Management: ___/4 (___%)
[Notes]

### Monitoring & Compliance: ___/5 (___%)
[Notes]

### Cost Optimization: ___/5 (___%)
[Notes]

## Recommendations

### Critical Priority
1. [Action item]
   - Owner: [Name]
   - Due: [Date]

### High Priority
1. [Action item]
   - Owner: [Name]
   - Due: [Date]

### Medium Priority
[List items]

### Low Priority
[List items]

## Sign-off

Security Team: _________________ Date: ______
Engineering Lead: ______________ Date: ______
```

---

## 🔄 Continuous Improvement

### Quarterly Review Checklist
- [ ] Re-run this audit
- [ ] Compare scores to previous quarter
- [ ] Update policies based on new threats
- [ ] Review and update firewall rules
- [ ] Verify all remediation items completed
- [ ] Update disaster recovery documentation

### Annual Deep Dive
- [ ] Penetration testing of ACR access controls
- [ ] Review all service principal permissions
- [ ] Audit image provenance (where did images come from?)
- [ ] Validate compliance with current standards
- [ ] Update security playbooks

---

**Use this checklist every quarter for production ACRs!**  
**Keep audit reports for compliance evidence.**
