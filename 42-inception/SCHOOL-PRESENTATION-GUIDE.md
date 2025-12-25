# School Presentation Guide - Inception Project

## 🎓 Setup Instructions for Evaluation

### Prerequisites
- USB 3.0 NVME drive with VM already configured
- School computer with VirtualBox installed
- Network access (for checking services)

---

## 📋 Step-by-Step Presentation

### 1. **Import and Start VM** (2 minutes)

```bash
# If VM is exported as .ova:
VBoxManage import /path/to/inception.ova --vsys 0 --vmname inception

# Or if using the NVME directly (VM already on external drive):
# Just start VirtualBox and the VM should appear in the list

# Start the VM
VBoxManage startvm inception --type headless
# Or use GUI: VirtualBox Manager → Select "inception" → Start
```

**While VM boots**, explain:
> "This is a Debian 12 Bookworm VM running a complete Docker infrastructure with 8 services. It's configured with NAT port forwarding for external access."

---

### 2. **Verify Port Forwarding** (1 minute)

```bash
# Check current port forwarding rules
VBoxManage showvminfo inception | grep "NIC 1 Rule"
```

**Expected output:**
```
NIC 1 Rule(0): https, tcp, host port = 8443, guest port = 443
NIC 1 Rule(1): https443, tcp, host port = 443, guest port = 443
NIC 1 Rule(2): portainer, tcp, host port = 9443, guest port = 9443
NIC 1 Rule(3): ssh, tcp, host port = 2222, guest port = 22
```

**If missing**, add them:
```bash
VBoxManage controlvm "inception" natpf1 "https,tcp,,8443,,443"
VBoxManage controlvm "inception" natpf1 "https443,tcp,,443,,443"
VBoxManage controlvm "inception" natpf1 "portainer,tcp,,9443,,9443"
VBoxManage controlvm "inception" natpf1 "ssh,tcp,,2222,,22"
```

**Explain:**
> "These NAT rules allow us to access services from the host. Port 8443 and 443 for HTTPS, 9443 for Portainer, and 2222 for SSH management."

---

### 3. **SSH Into VM and Start Services** (2 minutes)

```bash
# Connect to VM
ssh -p 2222 ggrisole@localhost
# Password: (your password)

# Navigate to project directory
cd ~/inception/srcs

# Check if containers are already running
docker ps

# If not running, start everything
make
# Or manually:
docker-compose up -d --build
```

**While building** (if needed), explain:
> "The Makefile orchestrates docker-compose, which builds 8 custom images from Dockerfiles. No pre-built images from Docker Hub - everything built from scratch per subject requirements."

---

### 4. **Verify All Containers** (2 minutes)

```bash
# Check all 8 containers are up
docker ps --format 'table {{.Names}}\t{{.Status}}\t{{.Ports}}'
```

**Expected output:**
```
NAMES         STATUS                    PORTS
nginx         Up X minutes              0.0.0.0:443->443/tcp
wordpress     Up X minutes              9000/tcp
mariadb       Up X minutes (healthy)    3306/tcp
redis         Up X minutes (healthy)    6379/tcp
adminer       Up X minutes              9001/tcp
ftp           Up X minutes              0.0.0.0:21->21/tcp, 21000-21010/tcp
static-site   Up X minutes              8080/tcp
portainer     Up X minutes              0.0.0.0:9443->9000/tcp
```

**Point out:**
> "Notice MariaDB and Redis show '(healthy)' - these have health checks that verify they're actually functional, not just running."

---

### 5. **Impressive Command Demonstrations** (5-7 minutes)

#### A. **Show Network Isolation**
```bash
# Show custom Docker network
docker network ls | grep inception
docker network inspect srcs_inception_network --format '{{range .Containers}}{{.Name}}: {{.IPv4Address}}{{"\n"}}{{end}}'
```

**Explain:**
> "All services communicate on an isolated Docker bridge network. They can't reach the internet, only each other. This is security by design."

#### B. **Demonstrate Secrets Management**
```bash
# Show secrets are NOT in environment variables
docker exec wordpress env | grep -i password
# Output: Should show only _FILE paths, not actual passwords

# Show secrets are mounted as files
docker exec wordpress ls -la /run/secrets/
docker exec wordpress wc -c /run/secrets/db_password
# Output: 44 /run/secrets/db_password (exactly 44 bytes, base64 encoded)

# Show secrets are read-only
docker exec wordpress cat /run/secrets/db_password | wc -c
# Output: 44 (no trailing newline!)
```

**Explain:**
> "Secrets are mounted as read-only files, never exposed in environment variables or code. This follows Docker and Kubernetes security best practices from my Azure Security training."

#### C. **Verify Database Initialization**
```bash
# Check MariaDB users
docker exec mariadb mysql -u root -p$(cat /run/secrets/db_root_password) -e "SELECT User, Host FROM mysql.user WHERE User='wpuser' OR User='root';"
```

**Expected output:**
```
User    Host
wpuser  %
root    localhost
```

**Explain:**
> "The wpuser was created dynamically during initialization. Root is localhost-only for security. This prevented Problem #9 where the database wasn't initializing."

#### D. **Show Redis Cache Integration**
```bash
# Check Redis status from WordPress
docker exec wordpress wp redis status --allow-root
```

**Point out:**
- Status: Connected
- Client: PhpRedis
- Metrics recorded: [number]
- Redis Version: 7.0.15

**Explain:**
> "WordPress uses Redis for object caching, reducing database queries. This was Problem #4 and #5 - network binding and configuration."

#### E. **TLS Certificate Inspection**
```bash
# Check TLS configuration
docker exec nginx openssl x509 -in /etc/ssl/certs/nginx-selfsigned.crt -noout -subject -dates
```

**Expected output:**
```
subject=C = FR, ST = IDF, L = Paris, O = 42, OU = Student, CN = ggrisole.42.fr
notBefore=...
notAfter=...
```

**Explain:**
> "Self-signed certificate with 2048-bit RSA, valid for 365 days. CN matches domain name. In production, this would be Let's Encrypt."

#### F. **Show Health Checks in Action**
```bash
# Check health check definitions
docker inspect mariadb --format='{{.State.Health.Status}}: {{range .State.Health.Log}}{{.ExitCode}} {{end}}'
docker inspect redis --format='{{.State.Health.Status}}: {{range .State.Health.Log}}{{.ExitCode}} {{end}}'
```

**Explain:**
> "Health checks run every 10 seconds. If a service fails 3 times, Docker can restart it automatically. This is Day 41-43 Kubernetes concepts applied to Docker."

---

### 6. **Web Interface Demonstrations** (3-5 minutes)

#### On the evaluation computer browser:

**A. WordPress Site**
```
https://ggrisole.42.fr:8443/
# Or: https://localhost:8443/
```
- Show main site loads
- Navigate to `/wp-admin` and show login page
- Explain two users: admin and regular author

**B. Adminer (Database UI)**
```
https://ggrisole.42.fr:8443/adminer
```
- Show login form
- Login with:
  - System: MySQL
  - Server: mariadb
  - Username: wpuser
  - Password: (from secrets)
  - Database: wordpress
- Show tables (wp_posts, wp_users, etc.)

**Explain Problem #13:**
> "This was returning 502 Bad Gateway because NGINX was using proxy_pass for a FastCGI service. PHP-FPM speaks FastCGI protocol, not HTTP."

**C. Static Site Showcase**
```
https://ggrisole.42.fr:8443/static/
```
- Show custom HTML/CSS page
- Point out it's a separate container with its own NGINX

**Explain Problem #14:**
> "This returned 404 because of missing trailing slashes in the proxy configuration. NGINX path rewriting requires precise syntax."

**D. Portainer (Container Management)**
```
http://localhost:9443
```
- Show dashboard with all 8 containers
- Navigate to container logs, stats, or inspect
- Show stacks, volumes, networks

**Explain Problem #11:**
> "This had a 404 because the Dockerfile only moved the binary, not the public/ folder with the web UI. Classic archive extraction mistake."

---

### 7. **Architecture Explanation** (2-3 minutes)

Use this diagram flow:

```
Internet (Evaluator's Browser)
         ↓ HTTPS (443/8443)
    ┌────────────┐
    │   NGINX    │ ← TLS termination, reverse proxy
    └────────────┘
         ↓
    ┌─────────────────────────────────┐
    │   Docker Bridge Network         │
    │   (isolated from internet)      │
    │                                  │
    │  WordPress ←→ MariaDB            │
    │      ↓         ↑                 │
    │   Redis    (3306)                │
    │   (cache)                        │
    │                                  │
    │  Adminer   Static-Site  FTP      │
    │  (db-ui)   (showcase)  (vsftpd)  │
    │                                  │
    │  Portainer (management)          │
    └─────────────────────────────────┘
         ↓
    Persistent Volumes
    ~/data/mariadb
    ~/data/wordpress
```

**Explain:**
> "NGINX is the only entry point. All services are behind it on an isolated network. Persistent data lives in bind-mounted volumes on the host, surviving container restarts."

---

### 8. **Problem-Solving Story** (3-5 minutes)

**Key problems to highlight:**

**Problem #8 (Secrets):**
> "After 2 weeks away, WordPress couldn't connect to MariaDB. Turned out Docker was mounting old 25-byte secrets from cache while I generated new 44-byte secrets in the wrong location. The fix required understanding Docker's relative path resolution."

**Problem #9 (MariaDB Init):**
> "MariaDB never initialized the database. Root cause: `apt-get install mariadb-server` creates default files that get baked into Docker image layers. My init script checked if a directory existed, but it always did because of layer caching. Solution: `RUN rm -rf /var/lib/mysql/*` in the Dockerfile."

**Problem #11 (Portainer):**
> "Portainer API worked but web UI returned 404. The Dockerfile extracted the tarball then moved only the binary, deleting the `public/` folder. Had to extract to final location and use WORKDIR instead of selective file moves."

**Problem #13 (Adminer):**
> "502 Bad Gateway because NGINX used `proxy_pass` for a PHP-FPM service. PHP-FPM speaks FastCGI, not HTTP. Changed to `fastcgi_pass` with proper FastCGI parameters."

**Problem #14 (Static Site):**
> "404 errors due to missing trailing slashes in proxy config. Without them, NGINX forwards `/static/page.html` as `/static/page.html` instead of `/page.html`. Trailing slashes enable path rewriting."

**Explain debugging methodology:**
> "I applied a systematic approach: symptom → logs → root cause → fix → verify. Documented everything in DECEMBER-25-FIXES.md (500+ lines). This is the DevSecOps methodology from my Azure Security training Day 59."

---

### 9. **Bonus Features Highlight** (2 minutes)

```bash
# Show all bonus services are running
docker ps --filter "name=adminer" --filter "name=static" --filter "name=portainer" --filter "name=redis" --filter "name=ftp" --format 'table {{.Names}}\t{{.Status}}'
```

**Count them:**
1. Redis (object cache)
2. FTP (vsftpd for file management)
3. Adminer (database UI)
4. Static site (custom showcase)
5. Portainer (container management)

**Explain:**
> "All 5 bonus services fully functional. This demonstrates understanding beyond basic requirements: caching strategies, file transfer protocols, database administration tools, and container orchestration."

---

### 10. **Cybersecurity Knowledge Applied** (2-3 minutes)

**Mention specific Azure Security days:**

- **Day 5 (IaC)**: docker-compose as declarative infrastructure
- **Day 9 (Network Security)**: Isolated Docker bridge network
- **Day 17 (Key Vault)**: Secrets management with Docker secrets
- **Day 35 (Container Security)**: Custom images, no :latest tags, layer optimization
- **Day 41-43 (Kubernetes)**: Health checks, restart policies, self-healing
- **Day 51 (Supply Chain)**: Pinned versions (Debian:bookworm, not :latest)
- **Day 59 (DevSecOps)**: Systematic debugging and documentation

**Explain:**
> "This isn't just a Docker project. It's applying 74 days of Azure Security training to a containerized infrastructure. Every security decision has a reason backed by industry best practices."

---

## 🎯 Commands to Impress Evaluators

### Quick Status Check (30 seconds)
```bash
# One-liner to show everything
ssh -p 2222 ggrisole@localhost "docker ps --format 'table {{.Names}}\t{{.Status}}' && echo && docker network ls | grep inception && echo && docker volume ls | grep inception"
```

### Secret Verification (20 seconds)
```bash
# Show secrets are exactly 44 bytes with no newline
ssh -p 2222 ggrisole@localhost "docker exec wordpress wc -c /run/secrets/* && docker exec mariadb wc -c /run/secrets/*"
```

### Health Check Status (15 seconds)
```bash
# Show health check details
ssh -p 2222 ggrisole@localhost "docker ps --format '{{.Names}}: {{.Status}}' | grep healthy"
```

### Redis Cache Hit Rate (30 seconds)
```bash
# Show WordPress cache statistics
ssh -p 2222 ggrisole@localhost "docker exec wordpress wp redis info --allow-root | grep -E 'Status|Hits|Misses|Ratio'"
```

### TLS Configuration (20 seconds)
```bash
# Show TLS versions and ciphers
ssh -p 2222 ggrisole@localhost "docker exec nginx nginx -T 2>&1 | grep -E 'ssl_protocols|ssl_ciphers'"
```

### Complete Service Test (45 seconds)
```bash
# Test all web interfaces return 200/valid content
curl -k -I https://localhost:8443/ 2>&1 | head -1
curl -k -s https://localhost:8443/adminer | grep -o '<title>.*</title>'
curl -k -s https://localhost:8443/static/ | grep -o '<title>.*</title>'
curl -s http://localhost:9443/ | grep -o '<title>.*</title>'
```

---

## ⚠️ Troubleshooting at School

### If containers are not running:
```bash
ssh -p 2222 ggrisole@localhost
cd ~/inception
make re  # Clean rebuild
```

### If ports are already in use on school computer:
```bash
# Change host ports (before starting VM)
VBoxManage modifyvm inception --natpf1 delete "https"
VBoxManage modifyvm inception --natpf1 delete "https443"
VBoxManage modifyvm inception --natpf1 "https,tcp,,9443,,443"
VBoxManage modifyvm inception --natpf1 "https443,tcp,,9444,,443"

# Then access via https://localhost:9443/ instead
```

### If VM won't start:
```bash
# Check VM state
VBoxManage showvminfo inception | grep State

# If saved state is causing issues:
VBoxManage controlvm inception poweroff
VBoxManage startvm inception --type headless
```

---

## 📝 Quick Reference - All Service URLs

| Service | URL | Login |
|---------|-----|-------|
| WordPress | https://localhost:8443/ | See IDENTIFIANTS-COMPLETS.md |
| WP Admin | https://localhost:8443/wp-admin | ggrisoleadmin / (secret) |
| Adminer | https://localhost:8443/adminer | wpuser@mariadb / (secret) |
| Static Site | https://localhost:8443/static/ | No login required |
| Portainer | http://localhost:9443 | Create admin on first access |

---

## 🎓 Defense Tips

1. **Start confident**: "I've built a complete Docker infrastructure with 8 services, all from scratch, no Docker Hub images."

2. **Emphasize debugging**: "After a 2-week break, everything was broken. I systematically diagnosed and fixed 7 distinct problems in 3 hours."

3. **Show documentation**: "I've documented everything: 16 markdown files, over 150KB of documentation including 500-line troubleshooting guide."

4. **Link to training**: "I applied 74 days of Azure Security training to this project. Every security decision has a reason."

5. **Be specific**: Don't say "I configured NGINX." Say "I configured NGINX with TLSv1.2/1.3, FastCGI for WordPress, FastCGI for Adminer, and HTTP proxy for static site with trailing slash path rewriting."

6. **Highlight challenges**: "The hardest bug was MariaDB never initializing because of Docker layer caching baking in files from apt-get install."

7. **End strong**: "This project demonstrates not just Docker skills, but systematic problem-solving, security best practices, and production-grade documentation."

---

## ✅ Pre-Evaluation Checklist

- [ ] USB NVME plugged in and detected
- [ ] VM imported/visible in VirtualBox
- [ ] VM started (headless or GUI)
- [ ] Port forwarding rules configured
- [ ] SSH access working (`ssh -p 2222 ggrisole@localhost`)
- [ ] All 8 containers running (`docker ps`)
- [ ] WordPress accessible in browser
- [ ] Adminer accessible in browser
- [ ] Static site accessible in browser
- [ ] Portainer accessible in browser
- [ ] Secrets are 44 bytes (`wc -c ~/inception/secrets/*`)
- [ ] No hardcoded passwords in source files
- [ ] Health checks passing for MariaDB and Redis
- [ ] Documentation files ready to show

**You're ready! Good luck with your defense! 🚀**
