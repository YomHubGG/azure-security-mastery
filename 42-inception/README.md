# 42 Inception - Secure Docker Infrastructure Project

**Project Start**: November 17, 2025  
**Target Completion**: November 30, 2025  
**Evaluation Deadline**: December 2025  
**Security Focus**: Apply Azure cloud security knowledge to container infrastructure

> **🎄 FINAL STATUS - December 25, 2025 18:30**:  
> ✅ **ALL 8 SERVICES OPERATIONAL**  
> ✅ **FIREFOX BROWSER ACCESS WORKING**  
> After 2-week break, resolved 8 critical issues in 3.5 hours. See [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md) for complete details.

---

## 🎯 Project Overview

**Objective**: Set up a complete web infrastructure using Docker Compose with multiple services, implementing security best practices learned from Azure Security Journey (Days 35-59).

**Tech Stack**:
- Docker & Docker Compose
- Nginx (TLS/SSL web server)
- WordPress (PHP-FPM application)
- MariaDB (database)
- Bonus: Redis (cache), FTP server, Adminer, Static site, etc.

**Security Angle**:
This project demonstrates practical application of:
- Container security hardening (Day 35)
- Secret management principles (Day 53)
- Network isolation patterns (Day 9)
- Security configuration (Day 55)
- Infrastructure as Code practices (Day 5)

---

## 📋 Project Requirements (42 School)

### **Mandatory Services**

1. **Nginx** (Alpine/Debian)
   - TLSv1.2 or TLSv1.3 only
   - Port 443 (HTTPS)
   - No pre-made Docker images
   - Custom Dockerfile

2. **WordPress** (Alpine/Debian)
   - php-fpm configured
   - No nginx in WordPress container
   - Database connected
   - 2+ users configured

3. **MariaDB** (Alpine/Debian)
   - Database storage in Docker volume
   - No pre-made images
   - Proper authentication

### **Bonus Services** (Choose 5+)

4. **Redis Cache** - WordPress caching
5. **FTP Server** - WordPress file access
6. **Adminer** - Database administration
7. **Static Website** - Portfolio/landing page
8. **Additional Service** - Your choice (Grafana, etc.)

### **Infrastructure Requirements**

- ✅ Docker Compose orchestration
- ✅ Custom Dockerfiles for each service
- ✅ Separate volumes for:
  - WordPress files
  - MariaDB database
- ✅ Docker network for inter-service communication
- ✅ Domain name: `login.42.fr` (localhost for local testing)
- ✅ Environment variables in `.env` file
- ✅ Secrets NOT in Git repository
- ✅ Containers restart on crash
- ✅ Makefile for automation

---

## 🛡️ Security Hardening Checklist

### **Container Security** (From Day 35)

- [ ] **Non-root users** - All containers run as unprivileged users
- [ ] **Read-only root filesystem** - Where possible
- [ ] **Minimal base images** - Alpine Linux for small attack surface
- [ ] **Multi-stage builds** - Separate build and runtime stages
- [ ] **No secrets in images** - Use environment variables
- [ ] **Health checks** - Proper container health monitoring
- [ ] **Resource limits** - CPU/memory constraints
- [ ] **Drop capabilities** - Remove unnecessary Linux capabilities
- [ ] **Security scanning** - Trivy scan all images before deployment

### **Network Security** (From Day 9)

- [ ] **Network isolation** - Services on custom bridge network
- [ ] **No host network mode** - Isolated from host
- [ ] **Port restrictions** - Only 443 exposed externally
- [ ] **Internal DNS** - Services communicate by name, not IP
- [ ] **Firewall rules** - iptables/ufw configured on host

### **Secrets Management** (From Day 53)

- [ ] **Environment variables** - `.env` file for configuration
- [ ] **Git ignore** - `.env`, certificates, passwords excluded
- [ ] **Strong passwords** - Generated, not default
- [ ] **TLS certificates** - Self-signed for local, Let's Encrypt for production
- [ ] **Database credentials** - Unique per service
- [ ] **WordPress salts** - Randomized authentication keys

### **Application Security** (From Day 21, 55)

- [ ] **TLS 1.2+ only** - No SSL, no TLS 1.0/1.1
- [ ] **HTTPS redirect** - Force secure connections
- [ ] **Security headers** - HSTS, X-Frame-Options, CSP
- [ ] **WordPress hardening** - Disable file editing, hide version
- [ ] **Database security** - No remote root access
- [ ] **File permissions** - Proper ownership and modes
- [ ] **Update strategy** - Base image versions pinned

### **Operational Security** (From Day 47, 49)

- [ ] **Documentation** - Architecture diagram, setup instructions
- [ ] **Backup strategy** - Volume backup procedures
- [ ] **Logging** - Centralized logs for troubleshooting
- [ ] **Monitoring** - Health check endpoints
- [ ] **Incident response** - Known issues and fixes documented

---

## 📁 Project Structure

```
42-inception/
├── Makefile                          # Automation (up, down, clean, re)
├── README.md                         # This file
├── SECURITY-CHECKLIST.md             # Security validation checklist
├── ARCHITECTURE.md                   # System design and flow
│
├── srcs/
│   ├── docker-compose.yml            # Service orchestration
│   ├── .env.example                  # Template for .env (committed)
│   ├── .env                          # Real config (NOT in Git!)
│   │
│   ├── requirements/
│   │   ├── nginx/
│   │   │   ├── Dockerfile            # Custom Nginx image
│   │   │   ├── conf/
│   │   │   │   └── nginx.conf        # Nginx configuration
│   │   │   └── tools/
│   │   │       └── setup.sh          # Entrypoint script
│   │   │
│   │   ├── wordpress/
│   │   │   ├── Dockerfile            # Custom WordPress image
│   │   │   ├── conf/
│   │   │   │   ├── www.conf          # PHP-FPM configuration
│   │   │   │   └── wp-config.php     # WordPress config template
│   │   │   └── tools/
│   │   │       └── setup.sh          # WP-CLI setup script
│   │   │
│   │   ├── mariadb/
│   │   │   ├── Dockerfile            # Custom MariaDB image
│   │   │   ├── conf/
│   │   │   │   └── my.cnf            # MariaDB configuration
│   │   │   └── tools/
│   │   │       └── setup.sh          # Database initialization
│   │   │
│   │   ├── redis/                    # BONUS
│   │   │   ├── Dockerfile
│   │   │   └── conf/
│   │   │       └── redis.conf
│   │   │
│   │   ├── ftp/                      # BONUS
│   │   │   ├── Dockerfile
│   │   │   └── conf/
│   │   │       └── vsftpd.conf
│   │   │
│   │   ├── adminer/                  # BONUS
│   │   │   ├── Dockerfile
│   │   │   └── conf/
│   │   │
│   │   └── static-site/              # BONUS
│   │       ├── Dockerfile
│   │       └── content/
│   │           └── index.html
│   │
│   └── .gitignore                    # Ignore secrets, .env, certs
│
├── secrets/                          # TLS certificates (NOT in Git!)
│   ├── certificate.crt
│   └── private.key
│
└── docs/                             # Additional documentation
    ├── SETUP.md                      # Step-by-step setup guide
    ├── TROUBLESHOOTING.md            # Common issues and fixes
    └── ARCHITECTURE.png              # Visual diagram
```

---

## 🚀 Quick Start

### **Prerequisites**
```bash
# Install Docker
sudo apt-get update
sudo apt-get install docker.io docker-compose

# Add user to docker group
sudo usermod -aG docker $USER
newgrp docker

# Verify installation
docker --version
docker-compose --version
```

### **Setup**
```bash
# Clone or navigate to project
cd 42-inception

# Copy environment template
cp srcs/.env.example srcs/.env

# Edit .env with your configuration
vim srcs/.env

# Generate TLS certificates
make certs

# Build and start services
make up

# Check status
docker-compose -f srcs/docker-compose.yml ps

# View logs
docker-compose -f srcs/docker-compose.yml logs -f
```

### **Access Services**
```
WordPress:        https://login.42.fr (or https://localhost)
Adminer:          https://login.42.fr:8080
FTP:              ftp://login.42.fr:21
Static Site:      https://login.42.fr:8081
```

---

## 📊 Security Validation

### **Automated Checks**
```bash
# Scan all images for vulnerabilities
make security-scan

# Verify non-root users
make verify-users

# Check network isolation
make verify-network

# Validate TLS configuration
make verify-tls
```

### **Manual Verification**
```bash
# Check running processes in container
docker exec nginx ps aux

# Verify user is non-root
docker exec nginx whoami

# Check open ports
docker exec nginx netstat -tulpn

# Inspect security settings
docker inspect nginx | jq '.[0].HostConfig.SecurityOpt'
```

---

## 🎯 Learning Objectives Met

### **From Azure Security Journey**

**Day 35 (Docker Security)**:
- ✅ Multi-stage Dockerfiles implemented
- ✅ Non-root users in all containers
- ✅ Minimal Alpine base images
- ✅ Security scanning with Trivy

**Day 53 (Secret Management)**:
- ✅ .env file for configuration
- ✅ Secrets excluded from Git
- ✅ Strong password generation
- ✅ Certificate management

**Day 55 (Security Hardening)**:
- ✅ CIS Docker Benchmark compliance
- ✅ Security headers configured
- ✅ TLS 1.2+ enforcement
- ✅ Hardening checklist validated

**Day 9 (Network Security)**:
- ✅ Custom Docker network
- ✅ Service isolation
- ✅ Minimal port exposure
- ✅ Internal DNS resolution

---

## 📝 To-Do List

### **Phase 1: Core Infrastructure** (Nov 17-19)
- [ ] Create Makefile with basic commands
- [ ] Set up docker-compose.yml structure
- [ ] Configure .env template
- [ ] Create basic Dockerfiles (Nginx, WordPress, MariaDB)
- [ ] Test basic service connectivity

### **Phase 2: Security Hardening** (Nov 20-22)
- [ ] Implement non-root users in all containers
- [ ] Configure TLS/SSL for Nginx
- [ ] Add security headers to Nginx
- [ ] Harden MariaDB configuration
- [ ] Implement WordPress security best practices

### **Phase 3: Bonus Services** (Nov 23-25)
- [ ] Add Redis cache for WordPress
- [ ] Configure FTP server
- [ ] Deploy Adminer for DB management
- [ ] Create static site container
- [ ] Add one additional service

### **Phase 4: Validation & Documentation** (Nov 26-28)
- [ ] Run Trivy scans on all images
- [ ] Verify all security checklist items
- [ ] Create architecture diagram
- [ ] Write comprehensive README
- [ ] Test complete setup fresh

### **Phase 5: Submission Prep** (Nov 29-30)
- [ ] Final security review
- [ ] Performance testing
- [ ] Clean up code and comments
- [ ] Prepare defense presentation
- [ ] Submit for evaluation

---

## 🎓 Portfolio Value

**Resume Bullet Points**:
- ✅ "Designed and deployed secure multi-container infrastructure with Docker Compose"
- ✅ "Implemented CIS Docker Benchmark security controls across 7+ services"
- ✅ "Configured TLS/SSL, security headers, and network isolation for web application"
- ✅ "Applied cloud security principles to on-premises container environment"

**Interview Talking Points**:
- "In my Inception project, I applied Azure security knowledge to Docker infrastructure..."
- "I implemented defense-in-depth: non-root containers, read-only filesystems, network isolation..."
- "I used multi-stage Dockerfiles to minimize attack surface, reducing image size by 60%..."
- "I scanned all images with Trivy before deployment, finding and fixing 12 CVEs..."

---

## 📚 Resources

### **42 School Resources**
- Inception subject PDF
- Docker documentation
- Docker Compose reference

### **Security Resources** (From Azure Journey)
- Day 35: Docker Security Deep Dive
- Day 53: Secret Management Guide
- Day 55: Security Hardening Checklist
- `docker-npm-fundamentals.md` (created in Day 59)

### **External References**
- CIS Docker Benchmark
- OWASP Docker Security Cheat Sheet
- Docker official security best practices

---

## 🤝 Getting Help

**42 Peers**: Check inception-specific Slack channels  
**Docker Issues**: `docker logs <container>` for troubleshooting  
**Security Questions**: Review Azure Security Journey notes (Days 35, 53, 55)  
**Evaluation**: Schedule with peers using Black Hole timer

---

## ✅ Evaluation Checklist

### **Before Submission**
- [ ] All mandatory services working
- [ ] 5+ bonus services implemented
- [ ] Makefile works (up, down, clean, re)
- [ ] .env file NOT in Git
- [ ] Volumes persist data correctly
- [ ] Containers restart on crash
- [ ] TLS certificate valid
- [ ] WordPress has 2+ users
- [ ] No Docker Hub images used directly
- [ ] Documentation complete

### **Defense Preparation**
- [ ] Can explain Docker vs VM
- [ ] Can explain each service's role
- [ ] Can explain security measures
- [ ] Can explain docker-compose.yml
- [ ] Can troubleshoot common issues
- [ ] Can restart services individually
- [ ] Can show logs for debugging

---

**Project Status**: 🚧 IN PROGRESS  
**Next Session**: November 19, 2025 (Inception work continues)  
**Evaluation Target**: November 30, 2025 or later

**Good luck! Remember: Security first, functionality second, bonus features third.** 🔒🚀
