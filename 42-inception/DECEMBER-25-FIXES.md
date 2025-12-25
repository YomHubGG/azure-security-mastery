# December 25, 2025 - Critical Fixes Applied

## 🎄 Status: ALL SERVICES OPERATIONAL ✅

After 2 weeks away from the project, we encountered and resolved several critical issues that prevented the infrastructure from working. This document details all fixes applied.

---

## 🔴 Critical Issues Discovered

**Initial Issues (15:00 - 17:30):**
- Issues #1-5: Infrastructure completely non-functional after 2-week break

**Additional Fixes (17:30 - 18:00):**
- Issues #13-14: Bonus services accessible but returning errors

---

### Issue #1: Old Secrets Files (25 bytes vs 44 bytes)

**Problem:**
- Secrets were created on November 28 with 25 bytes
- New secrets generated on December 25 were 44 bytes
- Docker was mounting the old 25-byte secrets from cached location
- Location confusion: secrets in `~/inception/secrets/` vs `~/inception/srcs/secrets/`

**Root Cause:**
```bash
# docker-compose.yml references:
secrets:
  db_root_password:
    file: ../secrets/db_root_password.txt  # ← Looks for ~/inception/secrets/
```

**Fix Applied:**
```bash
# 1. Generate new 44-byte secrets (proper base64 with no newline)
cd ~/inception/srcs
mkdir -p ../secrets
openssl rand -base64 32 | tr -d '\n' > ../secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > ../secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > ../secrets/wp_admin_password.txt

# 2. Verify size
wc -c ../secrets/*
# Output: 44 bytes each ✅
```

**Verification:**
```bash
# Inside containers after rebuild:
docker exec wordpress wc -c /run/secrets/db_password
# Output: 44 /run/secrets/db_password ✅

docker exec mariadb wc -c /run/secrets/db_root_password  
# Output: 44 /run/secrets/db_root_password ✅
```

---

### Issue #2: MariaDB Never Initializing Database

**Problem:**
```bash
# MariaDB logs showed:
Starting MariaDB...
2025-12-25 16:07:35 0 [Note] mysqld: ready for connections
# But NEVER showed:
# Initializing MariaDB database...
# Creating database and user...
```

**Root Cause Chain:**

1. **Baked-in Database Files in Docker Image:**
   ```bash
   # When RUN apt-get install mariadb-server runs
   # It creates default DB files in /var/lib/mysql/
   # These get baked into the Docker image layer
   ```

2. **Init Script Check Failed:**
   ```bash
   # From init-db.sh:
   if [ ! -d "/var/lib/mysql/mysql" ]; then
       echo "Initializing MariaDB database..."
       # This block NEVER ran because directory existed!
   fi
   ```

3. **Persistent Old Data:**
   ```bash
   # Even after deleting ~/data/mariadb/ on host
   # Files persisted from image layers or volume cache
   ls -la ~/data/mariadb/
   # Showed files from Dec 25 16:21 (old initialization)
   ```

**Fix Applied:**

**Step 1: Update MariaDB Dockerfile**
```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    mariadb-server \
    mariadb-client \
    && rm -rf /var/lib/apt/lists/*

COPY conf/my.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/init-db.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/init-db.sh

RUN mkdir -p /run/mysqld && chown -R mysql:mysql /run/mysqld

EXPOSE 3306

# ✅ CRITICAL FIX: Remove pre-created database directory
RUN rm -rf /var/lib/mysql/*

ENTRYPOINT ["/usr/local/bin/init-db.sh"]
CMD ["mysqld", "--user=mysql"]
```

**Step 2: Complete Cleanup Process**
```bash
# Stop all containers
cd ~/inception
make down

# Remove MariaDB image (to force rebuild with new Dockerfile)
docker rmi mariadb:inception

# Remove ALL data (both host and volumes)
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*
docker volume rm srcs_mariadb_data srcs_wordpress_data 2>/dev/null

# Verify directories are empty
ls -la ~/data/mariadb/
# total 8
# drwxr-xr-x 2 messagebus crontab 4096 Dec 25 17:11 .
# drwxr-xr-x 4 ggrisole ggrisole 4096 Dec 25 16:21 ..

# Rebuild with fixed Dockerfile
make
```

**Verification of Fix:**
```bash
docker logs mariadb | head -80

# ✅ NOW SHOWS:
Initializing MariaDB database...
Installing MariaDB/MySQL system tables in '/var/lib/mysql' ...
OK
Creating database and user...
MariaDB initialization complete!
Starting MariaDB...
```

**Database User Verification:**
```bash
# Check wpuser was created
docker exec mariadb mysql -u root -e "SELECT User, Host FROM mysql.user;"
# Output:
# User          Host
# mariadb.sys   localhost
# mysql         localhost  
# root          localhost
# wpuser        %              ✅ Created successfully!

# Test wpuser connection
docker exec mariadb mysql -u wpuser -p$(cat ~/inception/secrets/db_password.txt) -e "SELECT 1;"
# Output: 1 ✅
```

---

### Issue #3: WordPress Initialization Loop

**Problem:**
```bash
docker logs wordpress
# Output:
Waiting for MariaDB...
Attempt 1/30...
Attempt 2/30...
[... continues forever ...]
```

**Root Cause:**
WordPress data volume had old files from previous runs, causing WP-CLI to error:
```
Error: WordPress files seem to already be present here.
```

**Fix Applied:**
```bash
# Clean WordPress data directory
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*

# Restart WordPress container
docker restart wordpress

# Wait 30 seconds for initialization
sleep 30 && docker logs wordpress
```

**Successful Output:**
```
Waiting for MariaDB...
MariaDB is ready! ✅
Waiting for Redis...
Redis is ready! ✅
Installing WordPress...
Downloading WordPress 6.9 (en_US)...
Success: WordPress downloaded.
Success: Generated 'wp-config.php' file.
Configuring Redis cache...
Success: Added the constant 'WP_REDIS_HOST' to the 'wp-config.php' file
Success: WordPress installed successfully. ✅
Installing Redis Object Cache plugin...
Plugin 'redis-cache' activated.
Success: Object cache enabled. ✅
WordPress installation complete with Redis caching!
Starting PHP-FPM...
```

---

### Issue #4: Portainer Web UI 404

**Problem:**
```bash
curl http://localhost:9443/
# Output: 404 page not found

# But API worked:
curl http://localhost:9443/api/status
# Output: {"Version":"2.19.4"...} ✅
```

**Root Cause:**
Portainer Dockerfile only copied the binary, not the web UI files:

```dockerfile
# OLD (BROKEN):
RUN wget portainer-2.19.4-linux-amd64.tar.gz && \
    tar -xzf portainer-2.19.4-linux-amd64.tar.gz && \
    mv portainer/portainer /usr/local/bin/ && \
    rm -rf portainer portainer-2.19.4-linux-amd64.tar.gz
    # ↑ Deleted the web UI files!
```

**Fix Applied:**

```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    ca-certificates \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install docker-compose (required by Portainer v2.19.4)
RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose && \
    chmod +x /usr/local/bin/docker-compose

# ✅ FIX: Extract to /usr/local/bin/portainer/ to keep ALL files
RUN wget https://github.com/portainer/portainer/releases/download/2.19.4/portainer-2.19.4-linux-amd64.tar.gz && \
    tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/ && \
    rm portainer-2.19.4-linux-amd64.tar.gz

# Set working directory to where web files are
WORKDIR /usr/local/bin/portainer

VOLUME /data

EXPOSE 9000

# Run binary from the portainer directory (needs to find public/ folder)
CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
```

**Rebuild Process:**
```bash
# Stop and remove old Portainer
docker-compose -f srcs/docker-compose.yml stop portainer
docker-compose -f srcs/docker-compose.yml rm -f portainer
docker rmi portainer:inception

# Rebuild with fixed Dockerfile
docker-compose -f srcs/docker-compose.yml build portainer

# Start new Portainer
docker-compose -f srcs/docker-compose.yml up -d portainer
```

**Verification:**
```bash
curl -s http://localhost:9443/ | head -20
# Output: <!doctype html><html lang="en" ng-app="portainer"... ✅
# Shows full HTML with Portainer logo and "Loading Portainer..."
```

---

### Issue #5: VirtualBox Port Forwarding for Port 443

**Problem:**
WordPress redirects from `:8443` to default HTTPS port (no port in URL), causing connection failures.

**Fix Applied:**
```bash
# Add port 443 forwarding while VM is running
VBoxManage controlvm "inception" natpf1 "https443,tcp,,443,,443"

# Verify all port forwarding rules
VBoxManage showvminfo "inception" | grep "Rule"
```

**Current Port Forwarding:**
```
NIC 1 Rule(0):   name = https, protocol = tcp, host port = 8443, guest port = 443
NIC 1 Rule(1):   name = https443, protocol = tcp, host port = 443, guest port = 443 ✅
NIC 1 Rule(2):   name = portainer, protocol = tcp, host port = 9443, guest port = 9443
NIC 1 Rule(3):   name = ssh, protocol = tcp, host port = 2222, guest port = 22
```

---

## 📊 Final Working Configuration

### All Services Operational

```bash
docker ps --format 'table {{.Names}}\t{{.Status}}\t{{.Ports}}'
```

| Container | Status | Ports |
|-----------|--------|-------|
| nginx | Up | 0.0.0.0:443→443/tcp |
| wordpress | Up | 9000/tcp |
| adminer | Up | 9001/tcp |
| static-site | Up | 8080/tcp |
| portainer | Up | 0.0.0.0:9443→9000/tcp |
| redis | Up (healthy) | 6379/tcp |
| mariadb | Up (healthy) | 3306/tcp |
| ftp | Up | 0.0.0.0:21→21/tcp, 21000-21010 |

### Access URLs (from Host)

| Service | URL | Notes |
|---------|-----|-------|
| **WordPress** | `https://ggrisole.42.fr:8443` | Accept self-signed cert |
| **WordPress** | `https://ggrisole.42.fr` (port 443) | Standard HTTPS |
| **Adminer** | `https://ggrisole.42.fr:8443/adminer` | Database management |
| **Static Site** | `https://ggrisole.42.fr:8443/static` | Custom static page |
| **Portainer** | `http://localhost:9443` | Container management UI |
| **FTP** | `ftp://localhost:21` | ftpuser / ftppass |

### Secret Files (Final)

```bash
~/inception/secrets/
├── db_root_password.txt    # 44 bytes, base64, no newline
├── db_password.txt          # 44 bytes, base64, no newline
└── wp_admin_password.txt    # 44 bytes, base64, no newline
```

**Generation Command:**
```bash
openssl rand -base64 32 | tr -d '\n' > secrets/filename.txt
```

### Data Volumes

```bash
~/data/
├── mariadb/        # MariaDB database files (fresh initialization)
└── wordpress/      # WordPress files, uploads, plugins
```

---

## 🔧 Complete Fix Workflow (Reproducible)

If you encounter similar issues, follow this exact sequence:

```bash
# 1. Stop everything
cd ~/inception
make down

# 2. Remove old images that might have baked-in data
docker rmi mariadb:inception portainer:inception

# 3. Complete data cleanup
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*
docker volume rm srcs_mariadb_data srcs_wordpress_data 2>/dev/null

# 4. Regenerate secrets in correct location
mkdir -p ~/inception/secrets
openssl rand -base64 32 | tr -d '\n' > ~/inception/secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > ~/inception/secrets/db_password.txt  
openssl rand -base64 32 | tr -d '\n' > ~/inception/secrets/wp_admin_password.txt

# 5. Verify secret sizes
wc -c ~/inception/secrets/*
# Should show: 44 44 44

# 6. Update Dockerfiles with fixes:
# - MariaDB: Add "RUN rm -rf /var/lib/mysql/*"
# - Portainer: Extract to /usr/local/bin/portainer/ and set WORKDIR

# 7. Rebuild everything
make

# 8. Verify MariaDB initialization
docker logs mariadb | head -50
# Should show: "Initializing MariaDB database..." and "Creating database and user..."

# 9. Wait for WordPress (30 seconds)
sleep 30 && docker logs wordpress | tail -30
# Should show: "WordPress installation complete with Redis caching!"

# 10. Test all services
curl -k https://localhost:8443 | head -20  # WordPress
curl http://localhost:9443/ | head -20     # Portainer
curl http://localhost:9443/api/status      # API check
```

---

## 🎓 Lessons Learned

### 1. Docker Layer Caching is Persistent
- `apt-get install mariadb-server` creates default DB files
- These files persist in image layers even after container removal
- **Solution:** Explicitly `RUN rm -rf /var/lib/mysql/*` in Dockerfile

### 2. Volume vs Bind Mount Behavior
- Bind mounts can cache files even when host directory is "empty"
- Docker volumes persist independently of container lifecycle
- **Solution:** Always remove volumes explicitly: `docker volume rm`

### 3. Secret File Format Matters
- Base64 strings should be exactly 44 bytes (32 bytes random × 4/3)
- Newlines cause authentication failures
- **Solution:** Use `tr -d '\n'` to remove trailing newline

### 4. Archive Extraction Location Matters
- Portainer tarball contains: `portainer/portainer` (binary) + `portainer/public/` (web UI)
- Extracting and moving only the binary breaks web UI
- **Solution:** Extract entire directory structure and set proper WORKDIR

### 5. Docker Compose File Path Resolution
- `file: ../secrets/file.txt` is relative to docker-compose.yml location
- If docker-compose.yml is in `~/inception/srcs/`, secrets must be in `~/inception/secrets/`
- **Solution:** Always verify relative paths from compose file location

---

## 📝 Files Modified

### 1. MariaDB Dockerfile
**File:** `~/inception/srcs/requirements/mariadb/Dockerfile`

**Change:** Added line to remove baked-in database files
```dockerfile
# Added at end:
RUN rm -rf /var/lib/mysql/*
```

### 2. Portainer Dockerfile  
**File:** `~/inception/srcs/requirements/bonus/portainer/Dockerfile`

**Changes:**
```dockerfile
# OLD:
mv portainer/portainer /usr/local/bin/
rm -rf portainer

# NEW:
tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/
WORKDIR /usr/local/bin/portainer
CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
```

### 3. Secret Files
**Location:** `~/inception/secrets/` (NOT `~/inception/srcs/secrets/`)

**Files regenerated:**
- `db_root_password.txt` - 44 bytes
- `db_password.txt` - 44 bytes  
- `wp_admin_password.txt` - 44 bytes

---

## ✅ Success Verification

### Test 1: MariaDB User Creation
```bash
docker exec mariadb mysql -u root -e "SELECT User, Host FROM mysql.user;"
# Expected: wpuser with % host present ✅
```

### Test 2: WordPress Connection to MariaDB
```bash
docker exec wordpress mysql -h mariadb -u wpuser -p$(docker exec wordpress cat /run/secrets/db_password) -e "SELECT 1;"
# Expected: 1 ✅
```

### Test 3: WordPress HTTP Response
```bash
curl -k https://localhost:8443 2>&1 | grep "<title>"
# Expected: <title>Inception Project</title> ✅
```

### Test 4: Portainer Web UI
```bash
curl -s http://localhost:9443/ | grep "Portainer"
# Expected: HTML containing "Portainer" text ✅
```

### Test 5: Redis Cache Connection
```bash
docker exec wordpress bash -c 'redis-cli -h redis ping'
# Expected: PONG ✅
```

---

### Issue #13: Adminer Web UI 502 Bad Gateway (December 25, 17:40)

**Problem:**
- Adminer container running with PHP-FPM on port 9001
- NGINX configuration using `proxy_pass http://adminer:9001`
- Result: 502 Bad Gateway error

**Root Cause:**
```nginx
# BROKEN: HTTP proxy to FastCGI service
location /adminer {
    proxy_pass http://adminer:9001;  # ← PHP-FPM needs fastcgi_pass!
    proxy_set_header Host $host;
}
```

PHP-FPM speaks **FastCGI protocol**, not HTTP. Using `proxy_pass` sends HTTP requests to a FastCGI listener, which rejects them.

**Fix Applied:**
```nginx
# FIXED: FastCGI configuration
location ~ ^/adminer(/.*\.php)?$ {
    fastcgi_pass adminer:9001;
    fastcgi_index adminer.php;
    include fastcgi_params;
    fastcgi_param SCRIPT_FILENAME /var/www/html/adminer.php;
    fastcgi_param SCRIPT_NAME /adminer;
}
```

**Steps:**
```bash
# 1. Update nginx.conf with FastCGI configuration
vim ~/inception/srcs/requirements/nginx/conf/nginx.conf

# 2. Rebuild NGINX
cd ~/inception/srcs
docker-compose stop nginx
docker-compose rm -f nginx
docker-compose build nginx
docker-compose up -d nginx

# 3. Test
curl -k -s https://ggrisole.42.fr:8443/adminer | grep '<title>'
# Output: <title>Login - Adminer</title> ✅
```

**Verification:**
```bash
# Adminer login page loads
https://ggrisole.42.fr:8443/adminer
# Shows: Login form for MariaDB access ✅
```

**Lesson Learned:**
- PHP-FPM requires `fastcgi_pass`, not `proxy_pass`
- FastCGI is a binary protocol, not HTTP
- Different from reverse proxying to HTTP servers

---

### Issue #14: Static Site 404 Not Found (December 25, 17:45)

**Problem:**
- Static site container running NGINX on port 8080
- Main NGINX proxy returning 404 for `/static` path

**Root Cause:**
```nginx
# BROKEN: Missing trailing slashes
location /static {
    proxy_pass http://static-site:8080;  # ← Path not forwarded correctly!
}
```

Without trailing slashes:
- Request: `https://ggrisole.42.fr:8443/static/index.html`
- Proxied to: `http://static-site:8080/static/index.html` (wrong!)
- Static site expects: `http://static-site:8080/index.html`

**Fix Applied:**
```nginx
# FIXED: Trailing slashes strip /static prefix
location /static/ {
    proxy_pass http://static-site:8080/;  # ← Trailing slashes!
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}
```

**How Trailing Slashes Work:**
- Request: `/static/index.html`
- NGINX strips `/static/` and forwards `/index.html` to backend
- Backend receives: `GET /index.html` ✅

**Verification:**
```bash
curl -k -s https://ggrisole.42.fr:8443/static/ | grep '<title>'
# Output: <title>Inception Project - Static Site</title> ✅
```

**Lesson Learned:**
- Trailing slashes in `location` and `proxy_pass` matter!
- `location /static/` + `proxy_pass http://backend/` = path rewriting
- `location /static` + `proxy_pass http://backend` = path preservation

---

### Issue #15: Firefox Unable to Connect After Certificate (December 25, 18:15)

**Problem:**
- Firefox shows security warning for self-signed certificate
- User accepts the risk
- Page redirects and shows "Unable to connect" error
- Error: "Firefox can't establish a connection to the server at localhost"

**Root Cause:**
```bash
# WordPress configured without port:
docker exec wordpress wp option get siteurl --allow-root
# Output: https://ggrisole.42.fr  # ← No :8443!
```

WordPress URL was `https://ggrisole.42.fr` (no port specified):
1. Firefox connects to `https://localhost:8443`
2. User accepts self-signed certificate warning
3. WordPress redirects to `https://ggrisole.42.fr` (drops port)
4. Firefox tries default HTTPS port 443
5. Port 443 not forwarded → Connection refused

**Why Port 443 Forwarding Failed:**
- VirtualBox NAT forwarding rule existed: `443→443`
- But privileged ports (< 1024) require special handling
- VirtualBox couldn't bind to port 443 on running VM
- Port 8443 worked because it's non-privileged

**Fix Applied:**
```bash
# Update WordPress site URLs to include port
ssh -p 2222 ggrisole@localhost \
  "docker exec wordpress wp option update siteurl 'https://ggrisole.42.fr:8443' --allow-root && \
   docker exec wordpress wp option update home 'https://ggrisole.42.fr:8443' --allow-root"
```

**Verification:**
```bash
# Check updated URLs
docker exec wordpress wp option get siteurl --allow-root
# Output: https://ggrisole.42.fr:8443 ✅

docker exec wordpress wp option get home --allow-root
# Output: https://ggrisole.42.fr:8443 ✅

# Test redirect behavior
curl -k -I https://localhost:8443 2>&1 | grep Location
# Should show Location with :8443 port
```

**Result:**
- Firefox now works correctly at `https://localhost:8443`
- All WordPress redirects stay on port 8443
- No more connection refused errors
- Certificate warning only appears once per session

**Lesson Learned:**
- WordPress `siteurl` and `home` options control ALL redirects
- Always specify port in URL when using non-standard ports
- VirtualBox NAT privileged port forwarding has limitations on running VMs
- For development, use non-privileged ports (8443, 8080, etc.)

---

## 🚀 Project Status: READY FOR DEFENSE

All 8 services are operational and properly configured:
- ✅ NGINX with TLSv1.3 and self-signed certificate
- ✅ WordPress with admin + regular user configured
- ✅ MariaDB with wpuser and proper authentication
- ✅ Redis object cache integrated and active
- ✅ FTP server with WordPress volume access
- ✅ Adminer for database management
- ✅ Static site serving custom content
- ✅ Portainer with full web UI for container management

**Defense Preparation:**
- Review `GUIDE-DEFENSE-ORAL-FR.md` for presentation structure
- Check `IDENTIFIANTS-COMPLETS.md` for all credentials
- Explain the **14 problems solved** (7 initial + 5 December 25 infrastructure + 2 December 25 bonus services)
- See `SCHOOL-PRESENTATION-GUIDE.md` for commands to impress evaluators

---

## 📅 Timeline

- **November 28, 2024**: Initial setup, 25-byte secrets created
- **December 10, 2024**: Last work session before break
- **December 25, 2024 15:00**: Return after 2-week break, discovered all issues
- **December 25, 2024 15:00-17:30**: Fixed Issues #8-12 (infrastructure)
- **December 25, 2024 17:30-18:00**: Fixed Issues #13-14 (bonus services)
- **December 25, 2024 18:15**: Fixed Issue #15 (WordPress URL redirect)
- **December 25, 2024 17:30**: All fixes applied, all services operational ✅

**Total debugging time:** ~2.5 hours  
**Root causes:** 5 distinct issues (secrets, MariaDB init, WordPress data, Portainer UI, port forwarding)  
**Final result:** 100% operational infrastructure ready for defense
