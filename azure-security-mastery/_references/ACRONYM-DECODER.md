# 🔤 Tech Acronym Decoder - Complete Reference

**Last Updated:** October 25, 2025  
**Purpose:** Decode ALL the acronyms you'll encounter in cloud/DevOps/security

---

## 🎯 How to Use This Guide

**Format:**
- **Acronym** = Full Name (Category) - What it means in plain English

**Categories:**
- 🏗️ Infrastructure
- 🔐 Security
- 🐳 Containers
- ☁️ Cloud Services
- 📊 DevOps/CI/CD
- 🌐 Networking
- 💾 Storage/Database

---

## A

### **ACR** = Azure Container Registry (🐳 Containers)
Private registry for Docker/container images on Azure. Like Docker Hub but private.  
**Cost:** €4.60/month minimum  
**Alternative:** GitHub Container Registry (free)

### **ACI** = Azure Container Instances (🐳 Containers)
Serverless containers on Azure. Deploy single containers without managing VMs.  
**Cost:** €0.0000133/vCPU-second  
**Use:** Short-lived workloads, testing, simple apps

### **AD** = Active Directory (🔐 Security)
Microsoft's identity management system. Enterprise user/group management.  
**Azure AD:** Cloud version for Azure resources

### **AKS** = Azure Kubernetes Service (🐳 Containers)
Managed Kubernetes on Azure. Orchestrates 100s of containers.  
**Cost:** €0 control plane + €70+/month for worker nodes  
**Use:** Large-scale container deployments (20+ containers)

### **ALB** = Application Load Balancer (🌐 Networking)
Distributes traffic across multiple servers. Layer 7 (HTTP/HTTPS) routing.  
**Azure equivalent:** Application Gateway

### **ANS** = Accord de Niveau de Service (🏗️ Infrastructure)
French for SLA (Service Level Agreement). Same thing.

### **API** = Application Programming Interface (📊 DevOps)
How software talks to other software. Example: Azure API, GitHub API.

### **ARM** = Azure Resource Manager (🏗️ Infrastructure)
Azure's infrastructure deployment system. ARM Templates = JSON files.  
**Better alternative:** Bicep (human-readable)

### **ASDK** = Azure Software Development Kit (📊 DevOps)
Code libraries for interacting with Azure services.

### **ASG** = Application Security Group (🔐 Security)
Group VMs by application for firewall rules.

### **ASP** = App Service Plan (☁️ Cloud Services)
Compute tier for Azure App Service (like VM size).

### **AZ** = Availability Zone (🏗️ Infrastructure)
Separate physical datacenter within Azure region. HA uses multiple AZs.

---

## B

### **BCDR** = Business Continuity and Disaster Recovery (🏗️ Infrastructure)
Plan for when shit hits the fan. Backups, failover, recovery.

### **Bicep** = Not an acronym! (🏗️ Infrastructure)
Azure's Infrastructure-as-Code language. Better than ARM JSON.

### **BLOB** = Binary Large Object (💾 Storage)
File storage in Azure. Images, videos, documents, etc.

### **BOM** = Bill of Materials (🔐 Security)
See SBOM (Software Bill of Materials).

---

## C

### **CAPEX** = Capital Expenditure (☁️ Cloud Services)
Buying physical servers (upfront cost).  
**vs OPEX:** Operating Expenditure (cloud = pay-as-you-go)

### **CDN** = Content Delivery Network (🌐 Networking)
Cache content closer to users. Faster load times globally.  
**Azure:** Azure CDN

### **CI/CD** = Continuous Integration / Continuous Deployment (📊 DevOps)
Automatically test and deploy code when you push to GitHub.  
**Tools:** GitHub Actions, Azure DevOps, Jenkins

### **CLI** = Command Line Interface (📊 DevOps)
Terminal commands. Example: `az` (Azure CLI), `kubectl` (Kubernetes)

### **CORS** = Cross-Origin Resource Sharing (🔐 Security)
Browser security: Which domains can access your API.  
**You use this:** Day 35 secure-app has CORS configured!

### **CPU** = Central Processing Unit (🏗️ Infrastructure)
The brain of a computer. "vCPU" = virtual CPU in cloud.

### **CQRS** = Command Query Responsibility Segregation (🏗️ Infrastructure)
Architecture pattern: Separate read and write operations.

### **CSI** = Container Storage Interface (🐳 Containers)
Standard API for storage in Kubernetes. Like USB for storage.  
**Example:** CSI Secrets Store Driver (Key Vault → Kubernetes)

### **CSP** = Content Security Policy (🔐 Security)
Security header that prevents XSS attacks.  
**You use this:** Day 35 secure-app sends CSP header!

### **CSV** = Comma-Separated Values (💾 Storage)
File format for spreadsheet data.

### **CVE** = Common Vulnerabilities and Exposures (🔐 Security)
Unique ID for security vulnerabilities.  
**Example:** CVE-2021-44228 = Log4Shell (huge Java vulnerability)

---

## D

### **DDoS** = Distributed Denial of Service (🔐 Security)
Attack that floods your site with traffic to crash it.  
**Azure:** DDoS Protection (costs extra)

### **DLP** = Data Loss Prevention (🔐 Security)
Prevent sensitive data from leaving your organization.

### **DNS** = Domain Name System (🌐 Networking)
Converts domain names to IP addresses.  
**Example:** google.com → 142.250.185.46

### **DoS** = Denial of Service (🔐 Security)
Attack that crashes your site (single source, vs DDoS = many sources).

### **DR** = Disaster Recovery (🏗️ Infrastructure)
Plan for recovering after catastrophic failure.

---

## E

### **ECR** = Elastic Container Registry (🐳 Containers)
AWS's container registry (like ACR for Azure).

### **ECS** = Elastic Container Service (🐳 Containers)
AWS's container orchestration (like AKS for Azure).

### **EKS** = Elastic Kubernetes Service (🐳 Containers)
AWS's managed Kubernetes (like AKS for Azure).

### **EOF** = End of File (📊 DevOps)
Marker for end of file in scripts.

### **etcd** = Not an acronym! (🐳 Containers)
Kubernetes database that stores cluster state. Pronounced "et-see-dee".

---

## F

### **FQDN** = Fully Qualified Domain Name (🌐 Networking)
Complete domain name including subdomain.  
**Example:** yom-secure-app.westeurope.azurecontainer.io

### **FTP** = File Transfer Protocol (🌐 Networking)
Old way to transfer files (insecure, don't use!).  
**Better:** SFTP (Secure FTP) or SCP

---

## G

### **GAR** = Google Artifact Registry (🐳 Containers)
Google's container registry (new version of GCR).

### **GCR** = Google Container Registry (🐳 Containers)
Google's container registry (old version, use GAR now).

### **GDPR** = General Data Protection Regulation (🔐 Security)
EU privacy law. Must protect user data or face huge fines.  
**You learned:** Day 19 Data Protection

### **GHCR** = GitHub Container Registry (🐳 Containers)
GitHub's container registry. FREE for public containers!  
**You use this:** Day 37 - ghcr.io/yomhubgg/secure-app:1.0.0

### **GIT** = Not an acronym! (📊 DevOps)
Version control system. Linus Torvalds named it after himself (British slang for "idiot").

### **GUID** = Globally Unique Identifier (📊 DevOps)
Unique ID like "f47ac10b-58cc-4372-a567-0e02b2c3d479".  
**Also called:** UUID (Universally Unique Identifier)

---

## H

### **HA** = High Availability (🏗️ Infrastructure)
System stays online even when components fail.  
**Example:** 3 servers instead of 1, automatic failover  
**Uptime:** 99.99% = 52 minutes downtime per year

### **HPA** = Horizontal Pod Autoscaler (🐳 Containers)
Kubernetes feature: Automatically add more Pods when CPU is high.  
**Example:** 3 Pods → 10 Pods when CPU > 80%

### **HSM** = Hardware Security Module (🔐 Security)
Physical device that stores encryption keys ultra-securely.  
**Azure:** Key Vault can use HSM (premium tier)

### **HSTS** = HTTP Strict Transport Security (🔐 Security)
Security header that forces HTTPS only.  
**You use this:** Day 35 secure-app sends HSTS header!

### **HTTP** = Hypertext Transfer Protocol (🌐 Networking)
How browsers talk to web servers (unencrypted).

### **HTTPS** = HTTP Secure (🌐 Networking)
Encrypted HTTP (SSL/TLS). The padlock 🔒 in your browser.

---

## I

### **IaaS** = Infrastructure as a Service (☁️ Cloud Services)
Rent VMs from cloud provider (you manage OS).  
**Example:** Azure VMs, AWS EC2  
**vs PaaS:** Platform as a Service (provider manages OS)  
**vs SaaS:** Software as a Service (just use the app)

### **IAM** = Identity and Access Management (🔐 Security)
Who can access what resources.  
**Azure calls it:** RBAC (Role-Based Access Control)  
**AWS/GCP call it:** IAM

### **IaC** = Infrastructure as Code (🏗️ Infrastructure)
Define infrastructure in code files (Bicep, Terraform).  
**You use:** Bicep templates for Azure resources

### **ICMP** = Internet Control Message Protocol (🌐 Networking)
Protocol for ping command.

### **IDE** = Integrated Development Environment (📊 DevOps)
Code editor with built-in tools.  
**Examples:** VS Code, IntelliJ, PyCharm

### **IdP** = Identity Provider (🔐 Security)
Service that handles authentication.  
**Examples:** Azure AD, Google, Okta, Auth0

### **IoC** = Indicators of Compromise (🔐 Security)
Signs that a system was hacked (strange IPs, files, processes).

### **IoT** = Internet of Things (🏗️ Infrastructure)
Smart devices connected to internet (thermostats, cameras, etc.).

### **IP** = Internet Protocol (🌐 Networking)
Address for computers on internet.  
**IPv4:** 192.168.1.1 (running out!)  
**IPv6:** 2001:0db8:85a3:0000:0000:8a2e:0370:7334 (future)

### **IOPS** = Input/Output Operations Per Second (💾 Storage)
How fast a disk can read/write.  
**High IOPS:** SSD (fast, expensive)  
**Low IOPS:** HDD (slow, cheap)

---

## J

### **JSON** = JavaScript Object Notation (📊 DevOps)
Data format using {} and [].  
**Example:** `{"name": "Yom", "skill": "Azure"}`

### **JWT** = JSON Web Token (🔐 Security)
Encrypted token for authentication.  
**Format:** header.payload.signature

---

## K

### **K8s** = Kubernetes (🐳 Containers)
"K" + 8 letters + "s" = K8s (shorthand).  
**Pronunciation:** "koo-ber-neh-teez" (Greek for "helmsman")

### **KMS** = Key Management Service (🔐 Security)
Manage encryption keys.  
**Azure:** Key Vault  
**AWS:** KMS  
**GCP:** Cloud KMS

### **KPI** = Key Performance Indicator (📊 DevOps)
Metrics to measure success (uptime, response time, etc.).

### **KV** = Key Vault (🔐 Security)
Azure service for storing secrets, keys, certificates.  
**You learned:** Day 17!

---

## L

### **LAMP** = Linux + Apache + MySQL + PHP (🏗️ Infrastructure)
Traditional web stack (old-school, still used for WordPress).

### **LB** = Load Balancer (🌐 Networking)
Distributes traffic across multiple servers.  
**Azure:** Azure Load Balancer, Application Gateway

### **LDAP** = Lightweight Directory Access Protocol (🔐 Security)
Protocol for accessing directory services (like Active Directory).

### **LTS** = Long Term Support (📊 DevOps)
Software version supported for many years.  
**Example:** Node.js 18 LTS (supported until 2025)

---

## M

### **MAC** = Media Access Control (🌐 Networking)
Physical address of network card.  
**Example:** 00:1B:44:11:3A:B7

### **MAU** = Monthly Active Users (📊 DevOps)
How many users use your app per month.

### **MEAN** = MongoDB + Express + Angular + Node.js (🏗️ Infrastructure)
Popular JavaScript web stack.

### **MERN** = MongoDB + Express + React + Node.js (🏗️ Infrastructure)
Most popular JavaScript web stack (React instead of Angular).  
**This is what you should learn!**

### **MFA** = Multi-Factor Authentication (🔐 Security)
Login with password + phone code.  
**Also called:** 2FA (Two-Factor Authentication)

### **MITM** = Man in the Middle (🔐 Security)
Attack where hacker intercepts communication.  
**Prevention:** Use HTTPS, VPN

### **ML** = Machine Learning (📊 DevOps)
AI that learns from data (not explicitly programmed).

### **MSI** = Managed Service Identity (🔐 Security)
Azure's old name for Managed Identity.  
**Now called:** Managed Identity (passwords not needed!)

### **mTLS** = Mutual TLS (🔐 Security)
Both client and server verify each other's identity.  
**Use case:** Service mesh (Pods verify each other)

---

## N

### **NAT** = Network Address Translation (🌐 Networking)
Private IPs (192.168.x.x) → Public IP (for internet access).

### **NIC** = Network Interface Card (🌐 Networking)
Hardware that connects computer to network.

### **NLB** = Network Load Balancer (🌐 Networking)
Load balancer for Layer 4 (TCP/UDP).  
**vs ALB:** Application Load Balancer (Layer 7, HTTP/HTTPS)

### **NoSQL** = Not Only SQL (💾 Database)
Non-relational databases (MongoDB, Cosmos DB, etc.).  
**vs SQL:** Relational databases (MySQL, PostgreSQL)

### **NSG** = Network Security Group (🔐 Security)
Azure firewall rules for VMs and subnets.  
**Example:** Allow port 443 (HTTPS), block port 22 (SSH) from internet

---

## O

### **OAuth** = Open Authorization (🔐 Security)
Standard for login delegation ("Sign in with Google").

### **OIDC** = OpenID Connect (🔐 Security)
Authentication layer on top of OAuth.

### **OPA** = Open Policy Agent (🔐 Security)
Tool for policy enforcement (written in Go).  
**Gatekeeper:** OPA for Kubernetes

### **OPEX** = Operating Expenditure (☁️ Cloud Services)
Pay-as-you-go costs (cloud bills).  
**vs CAPEX:** Capital Expenditure (buying physical servers)

### **OS** = Operating System (🏗️ Infrastructure)
Software that runs computer (Windows, Linux, macOS).

### **OSI** = Open Systems Interconnection (🌐 Networking)
7-layer network model (Physical, Data Link, Network, Transport, Session, Presentation, Application).

### **OWASP** = Open Web Application Security Project (🔐 Security)
Organization for web security standards.  
**Famous:** OWASP Top 10 (most critical web vulnerabilities)

---

## P

### **PaaS** = Platform as a Service (☁️ Cloud Services)
Provider manages OS and runtime, you just deploy code.  
**Example:** Azure App Service, Heroku  
**vs IaaS:** You manage OS (VMs)  
**vs SaaS:** Just use the app (Office 365)

### **PAT** = Personal Access Token (🔐 Security)
Token instead of password for API access.  
**You used:** GitHub PAT for pushing containers (Day 37)

### **PERN** = PostgreSQL + Express + React + Node.js (🏗️ Infrastructure)
Like MERN but with PostgreSQL instead of MongoDB.

### **PIM** = Privileged Identity Management (🔐 Security)
Azure AD feature: Time-limited admin access.  
**Example:** Grant admin for 2 hours, then auto-revoke

### **PKI** = Public Key Infrastructure (🔐 Security)
System for managing SSL/TLS certificates.

### **Pod** = Not an acronym! (🐳 Containers)
Smallest unit in Kubernetes (wraps 1+ containers).

### **PSP** = Pod Security Policy (🔐 Security)
Kubernetes security (deprecated, use Pod Security Standards).

### **PSS** = Pod Security Standards (🔐 Security)
New way to enforce Pod security (Privileged, Baseline, Restricted).  
**You learned:** Day 41!

---

## Q

### **QoS** = Quality of Service (🌐 Networking)
Prioritize certain traffic (VoIP over file downloads).

---

## R

### **RAM** = Random Access Memory (🏗️ Infrastructure)
Computer memory (temporary storage while running).  
**Cloud:** GB of RAM (0.5GB, 1GB, 8GB, etc.)

### **RBAC** = Role-Based Access Control (🔐 Security)
Who can do what (roles and permissions).  
**Azure:** Azure RBAC  
**Kubernetes:** Kubernetes RBAC  
**You learned:** Day 41!

### **Rego** = Not an acronym! (🔐 Security)
Policy language for OPA (Open Policy Agent).  
**Written in:** Go (Golang)

### **REST** = Representational State Transfer (🌐 Networking)
API design style using HTTP methods (GET, POST, PUT, DELETE).

### **RG** = Resource Group (🏗️ Infrastructure)
Azure container for related resources.  
**You use:** rg-learning, rg-containers-learning

### **RPO** = Recovery Point Objective (🏗️ Infrastructure)
How much data you can afford to lose.  
**Example:** RPO = 1 hour (backups every hour)

### **RTO** = Recovery Time Objective (🏗️ Infrastructure)
How fast you can recover after failure.  
**Example:** RTO = 30 minutes (must be back up in 30 min)

---

## S

### **SaaS** = Software as a Service (☁️ Cloud Services)
Just use the app, provider manages everything.  
**Examples:** Office 365, Gmail, Netflix

### **SAML** = Security Assertion Markup Language (🔐 Security)
Enterprise SSO protocol (older than OAuth).

### **SAST** = Static Application Security Testing (🔐 Security)
Scan code for vulnerabilities without running it.  
**Tools:** SonarCloud, Checkmarx

### **SBOM** = Software Bill of Materials (🔐 Security)
List of all components in your software (libraries, versions).  
**Why:** Track vulnerable dependencies

### **SCP** = Secure Copy Protocol (🌐 Networking)
Securely copy files over SSH.

### **SDK** = Software Development Kit (📊 DevOps)
Code libraries for interacting with services.  
**Example:** Azure SDK for Python

### **SFTP** = Secure File Transfer Protocol (🌐 Networking)
FTP over SSH (encrypted file transfer).

### **SIEM** = Security Information and Event Management (🔐 Security)
Centralized logging and security monitoring.  
**Azure:** Microsoft Sentinel

### **SKU** = Stock Keeping Unit (☁️ Cloud Services)
Azure's term for "pricing tier".  
**Example:** Basic SKU, Standard SKU, Premium SKU

### **SLA** = Service Level Agreement (🏗️ Infrastructure)
Provider's promise of uptime.  
**Example:** 99.95% = 4.38 hours downtime per year max

### **SOC** = Security Operations Center (🔐 Security)
Team that monitors security 24/7.

### **SOC** = System and Organization Controls (🔐 Security)
Audit standards (SOC 1, SOC 2, SOC 3).  
**Different from above SOC!** Context matters.

### **SPA** = Single Page Application (🏗️ Infrastructure)
Web app that loads once, no page refreshes.  
**Examples:** Gmail, Netflix, Facebook  
**Tech:** React, Angular, Vue

### **SPF** = Sender Policy Framework (🔐 Security)
Email security (prevent email spoofing).

### **SQL** = Structured Query Language (💾 Database)
Language for relational databases.  
**Databases:** MySQL, PostgreSQL, SQL Server

### **SSH** = Secure Shell (🌐 Networking)
Encrypted remote terminal access.  
**Default port:** 22

### **SSL** = Secure Sockets Layer (🔐 Security)
Old name for TLS (encryption protocol).  
**Actually means:** TLS now, but everyone still says "SSL certificate"

### **SSO** = Single Sign-On (🔐 Security)
One login for many apps.  
**Example:** Sign in with Google, then access Gmail, Drive, YouTube

### **SSD** = Solid State Drive (💾 Storage)
Fast storage (no moving parts).  
**vs HDD:** Hard Disk Drive (slow, mechanical)

---

## T

### **TCP** = Transmission Control Protocol (🌐 Networking)
Reliable data transfer (web pages, email).  
**vs UDP:** Fast but unreliable (video streaming, gaming)

### **TLS** = Transport Layer Security (🔐 Security)
Encryption protocol for HTTPS.  
**Versions:** TLS 1.2 (minimum), TLS 1.3 (best)

### **TTL** = Time to Live (🌐 Networking)
How long data is cached.  
**Example:** DNS TTL = 300 seconds (cache for 5 minutes)

---

## U

### **UAT** = User Acceptance Testing (📊 DevOps)
Testing by actual users before launch.

### **UDP** = User Datagram Protocol (🌐 Networking)
Fast but unreliable data transfer.  
**vs TCP:** Reliable but slower  
**Use:** Video streaming, gaming, DNS

### **UI** = User Interface (📊 DevOps)
What users see and interact with.

### **URI** = Uniform Resource Identifier (🌐 Networking)
Generic term for URLs and URNs.

### **URL** = Uniform Resource Locator (🌐 Networking)
Web address.  
**Example:** https://github.com/YomHubGG

### **UUID** = Universally Unique Identifier (📊 DevOps)
Same as GUID (unique ID).

### **UX** = User Experience (📊 DevOps)
How users feel when using your app.

---

## V

### **vCPU** = Virtual CPU (🏗️ Infrastructure)
CPU in cloud (fraction of physical CPU).  
**Example:** 0.5 vCPU = half a CPU core

### **VHD** = Virtual Hard Disk (💾 Storage)
Disk image file for VMs.

### **VLAN** = Virtual Local Area Network (🌐 Networking)
Separate networks on same physical switch.

### **VM** = Virtual Machine (🏗️ Infrastructure)
Emulated computer running on physical server.  
**Azure:** Azure VMs (B1s, D2s_v3, etc.)

### **VNet** = Virtual Network (🌐 Networking)
Private network in Azure (like LAN in cloud).

### **VPA** = Vertical Pod Autoscaler (🐳 Containers)
Kubernetes: Automatically increase Pod resources (CPU/RAM).  
**vs HPA:** Horizontal adds more Pods

### **VPN** = Virtual Private Network (🌐 Networking)
Encrypted tunnel to private network.  
**Use:** Secure remote access

---

## W

### **WAF** = Web Application Firewall (🔐 Security)
Firewall for web apps (blocks SQL injection, XSS, etc.).  
**Azure:** Application Gateway WAF

### **WORM** = Write Once Read Many (💾 Storage)
Immutable storage (can't modify or delete).  
**Use:** Compliance, audit logs

---

## X

### **XSS** = Cross-Site Scripting (🔐 Security)
Inject malicious JavaScript into web pages.  
**Prevention:** CSP header (you use this!)

### **XML** = Extensible Markup Language (📊 DevOps)
Data format using <tags>.  
**Like JSON but older and more verbose.**

---

## Y

### **YAML** = YAML Ain't Markup Language (📊 DevOps)
Config file format using indentation.  
**Example:** Kubernetes manifests, GitHub Actions workflows  
**Pronunciation:** "yam-uhl"

### **YML** = YAML file extension (📊 DevOps)
Same as YAML, just shorter extension (`.yml` vs `.yaml`).

---

## Z

### **ZTA** = Zero Trust Architecture (🔐 Security)
"Never trust, always verify" security model.  
**Principles:**
- Verify every request
- Assume breach
- Least privilege access

---

## 🎯 Most Important Acronyms for Your Career

**Must Know (Interview Killers if you don't):**
- ✅ AKS, ACI, ACR (Azure container services)
- ✅ RBAC, IAM (access control)
- ✅ CI/CD (DevOps)
- ✅ HA, SLA (reliability)
- ✅ IaaS, PaaS, SaaS (cloud models)
- ✅ K8s (Kubernetes)
- ✅ MERN (web stack)

**Good to Know (Shows expertise):**
- CSI, OPA, SAST, DAST
- mTLS, JWT, OAuth
- HPA, VPA (autoscaling)
- SBOM, CVE (security)

**Nice to Have (Impressive):**
- Rego, etcd, Gatekeeper
- CQRS, PSS, WORM
- RPO, RTO (disaster recovery)

---

**Created:** October 25, 2025  
**Author:** YomHub  
**Purpose:** Decode tech acronyms for cloud/DevOps/security careers  
**Usage:** Reference guide, interview prep, documentation reading
