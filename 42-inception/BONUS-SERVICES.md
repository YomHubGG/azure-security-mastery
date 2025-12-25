# Inception Bonus Services - Implementation Guide

**Date**: November 30, 2025  
**Last Updated**: December 25, 2025 18:00  
**Status**: ✅ ALL 8 SERVICES OPERATIONAL

> **🎄 December 25 Final Update**: Fixed Adminer FastCGI and Static Site proxy issues. All bonus services now fully functional!

---

## 🎁 Bonus Services Implemented

| Service | Purpose | Port/Access | Status |
|---------|---------|-------------|--------|
| **Redis** | WordPress object caching | Internal:6379 | ✅ Operational |
| **FTP** | File access to WordPress volume | 21, 21000-21010 | ✅ Operational |
| **Adminer** | Database management UI | https://ggrisole.42.fr:8443/adminer | ✅ Operational (Fixed Dec 25) |
| **Static Site** | Custom HTML/CSS showcase | https://ggrisole.42.fr:8443/static/ | ✅ Operational (Fixed Dec 25) |
| **Portainer** | Docker container management | http://localhost:9443 | ✅ Operational (Fixed Dec 25) |

**Updates**:
- December 25, 17:40: Fixed Adminer 502 error (HTTP proxy → FastCGI)
- December 25, 17:45: Fixed Static Site 404 error (added trailing slashes)
- December 25, 17:30: Fixed Portainer 404 error (full archive extraction)

---

## 📋 Files Created

### Bonus Services Structure
```
~/inception/srcs/requirements/bonus/
├── redis/
│   └── Dockerfile                    # Redis 7.x for object caching
├── ftp/
│   ├── Dockerfile                    # vsftpd FTP server
│   └── conf/vsftpd.conf             # FTP configuration
├── adminer/
│   └── Dockerfile                    # Adminer 4.8.1 DB manager
├── static-site/
│   ├── Dockerfile                    # NGINX static server
│   └── tools/
│       ├── index.html               # Beautiful static page
│       └── style.css                # Gradient purple theme
└── portainer/
    └── Dockerfile                    # Portainer 2.19.4
```

---

## 🚀 Deployment Instructions

### Step 1: Update docker-compose.yml

You need to replace `~/inception/srcs/docker-compose.yml` with the bonus version.

**Option A - Manual edit:**
Add these services to your existing docker-compose.yml after the nginx service.

**Option B - Use the pre-made file:**
```bash
# On your host machine
scp -P 2222 /tmp/inception-bonus/srcs/docker-compose-bonus.yml \
    ggrisole@localhost:~/inception/srcs/docker-compose.yml
```

### Step 2: Update NGINX Configuration

Update NGINX to proxy Adminer and static site:

```bash
# On your host
scp -P 2222 /tmp/inception-bonus/nginx-bonus.conf \
    ggrisole@localhost:~/inception/srcs/requirements/nginx/conf/nginx.conf
```

### Step 3: Update WordPress for Redis

**Option 1 - Fresh install (recommended):**
```bash
# SSH into VM
ssh -p 2222 ggrisole@localhost

# Clean WordPress data
cd ~/inception
docker compose down
sudo rm -rf ~/data/wordpress/*

# Copy new wp-setup script
# (On host, run:)
scp -P 2222 /tmp/inception-bonus/wp-setup-redis.sh \
    ggrisole@localhost:~/inception/srcs/requirements/wordpress/tools/wp-setup.sh
```

**Option 2 - Existing installation:**
Install Redis plugin manually after deployment:
```bash
docker exec wordpress wp plugin install redis-cache --activate --allow-root
docker exec wordpress wp config set WP_REDIS_HOST redis --allow-root
docker exec wordpress wp config set WP_REDIS_PORT 6379 --raw --allow-root
docker exec wordpress wp redis enable --allow-root
```

### Step 4: Add Port Forwarding (for host access)

```bash
# On host machine
VBoxManage modifyvm inception --natpf1 "ftp,tcp,,2121,,21"
VBoxManage modifyvm inception --natpf1 "portainer,tcp,,9443,,9000"
```

### Step 5: Deploy!

```bash
# SSH into VM
ssh -p 2222 ggrisole@localhost
cd ~/inception

# Rebuild and start with bonuses
docker compose down
docker compose up -d --build

# Watch the magic happen
docker compose logs -f
```

---

## 🧪 Testing Each Bonus Service

### 1. Redis Cache

```bash
# Check Redis is running
docker exec redis redis-cli ping
# Should return: PONG

# Check WordPress connection
docker exec wordpress wp redis status --allow-root
# Should show: Connected

# View Redis keys
docker exec redis redis-cli KEYS '*'
```

### 2. FTP Server

```bash
# From host machine (after port forwarding)
ftp localhost 2121
# Login: ftpuser
# Password: ftppass

# Or from inside VM
ftp localhost
# Login: ftpuser / ftppass
# Navigate to WordPress files
```

### 3. Adminer (Database Manager)

```bash
# Access from inside VM
wget --no-check-certificate -q -O - https://ggrisole.42.fr/adminer

# Or from browser on VM
# Navigate to: https://ggrisole.42.fr/adminer

# Login credentials:
# System: MySQL
# Server: mariadb
# Username: wpuser
# Password: (from secrets/db_password.txt)
# Database: wordpress
```

### 4. Static Site

```bash
# Test from inside VM
wget --no-check-certificate -q -O - https://ggrisole.42.fr/static

# Should see beautiful HTML page with:
# - Purple gradient background
# - Service overview cards
# - Responsive design
```

### 5. Portainer (Docker UI)

```bash
# From host machine (after port forwarding)
# Browse to: http://localhost:9443

# Or from inside VM:
curl http://localhost:9000

# First-time setup:
# - Create admin password
# - Select "Local" Docker environment
# - View all containers!
```

---

## 📊 Service URLs Summary

| Service | URL | Credentials |
|---------|-----|-------------|
| **WordPress** | https://ggrisole.42.fr/ | Admin: ggrisoleadmin |
| **Adminer** | https://ggrisole.42.fr/adminer | Server: mariadb, User: wpuser |
| **Static Site** | https://ggrisole.42.fr/static | Public access |
| **Portainer** | http://localhost:9443 | Create on first access |
| **FTP** | ftp://localhost:21 | ftpuser:ftppass |
| **Redis** | Internal only (port 6379) | No auth required |

---

## 🔧 Troubleshooting

### Adminer shows 502 Bad Gateway (Fixed December 25, 17:40)

**Symptom:** Adminer container running but NGINX returns 502 Bad Gateway

**Root Cause:** NGINX configured with `proxy_pass` for PHP-FPM service
- PHP-FPM speaks **FastCGI protocol**, not HTTP
- Using `proxy_pass` sends HTTP requests to FastCGI listener

**Solution:**
```nginx
# WRONG - HTTP proxy to FastCGI service:
location /adminer {
    proxy_pass http://adminer:9001;  # ← PHP-FPM doesn't speak HTTP!
}

# CORRECT - FastCGI configuration:
location ~ ^/adminer(/.*\.php)?$ {
    fastcgi_pass adminer:9001;
    fastcgi_index adminer.php;
    include fastcgi_params;
    fastcgi_param SCRIPT_FILENAME /var/www/html/adminer.php;
    fastcgi_param SCRIPT_NAME /adminer;
}
```

**Fix steps:**
```bash
# Update nginx.conf with FastCGI configuration
vim ~/inception/srcs/requirements/nginx/conf/nginx.conf

# Rebuild NGINX
cd ~/inception/srcs
docker-compose stop nginx && docker-compose rm -f nginx
docker-compose build nginx && docker-compose up -d nginx

# Test
curl -k -s https://ggrisole.42.fr:8443/adminer | grep '<title>'
# Expected: <title>Login - Adminer</title> ✅
```

See [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md#issue-13-adminer-web-ui-502-bad-gateway) for complete details.

---

### Static Site shows 404 Not Found (Fixed December 25, 17:45)

**Symptom:** Static site container running but NGINX returns 404

**Root Cause:** Missing trailing slashes in NGINX proxy configuration
- Without trailing slash: `/static/page.html` → proxied as `/static/page.html` (wrong!)
- With trailing slash: `/static/page.html` → proxied as `/page.html` (correct!)

**Solution:**
```nginx
# WRONG - No trailing slashes (path preservation):
location /static {
    proxy_pass http://static-site:8080;
}

# CORRECT - Trailing slashes (path rewriting):
location /static/ {
    proxy_pass http://static-site:8080/;  # ← Trailing slashes!
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}
```

**Fix steps:**
```bash
# Update nginx.conf with trailing slashes
vim ~/inception/srcs/requirements/nginx/conf/nginx.conf

# Rebuild NGINX
cd ~/inception/srcs
docker-compose stop nginx && docker-compose rm -f nginx
docker-compose build nginx && docker-compose up -d nginx

# Test
curl -k -s https://ggrisole.42.fr:8443/static/ | grep '<title>'
# Expected: <title>Inception Project - Static Site</title> ✅
```

See [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md#issue-14-static-site-404-not-found) for complete details.

---

### Portainer shows 404 on web UI (Fixed December 25, 17:30)

**Symptom:** API responds (`/api/status` works) but web UI returns 404

**Root Cause:** Dockerfile only copies the binary without web UI files (public/ folder)

**Solution:**
```dockerfile
# WRONG - Only moves binary, deletes web files:
RUN tar -xzf portainer.tar.gz && \
    mv portainer/portainer /usr/local/bin/ && \
    rm -rf portainer  # ← Deletes public/ folder!

# CORRECT - Keeps entire directory structure:
RUN tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/ && \
    rm portainer-2.19.4-linux-amd64.tar.gz

WORKDIR /usr/local/bin/portainer
CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
```

**Fix steps:**
```bash
docker-compose stop portainer
docker-compose rm -f portainer
docker rmi portainer:inception
# Update Dockerfile
docker-compose build portainer
docker-compose up -d portainer
```

See [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md#issue-4-portainer-web-ui-404) for complete details.

---

### Portainer timeout after 5 minutes

**Symptom:** Portainer web UI shows timeout error

**Cause:** Security feature - must complete initial setup within 5 minutes

**Solution:**
```bash
docker restart portainer
# Quickly navigate to http://localhost:9443 and create admin user
```

---

### Redis not connecting
```bash
# Check Redis container
docker logs redis

# Test connection from WordPress
docker exec wordpress nc -zv redis 6379
```

### FTP connection refused
```bash
# Check FTP logs
docker logs ftp

# Verify passive ports
docker exec ftp netstat -tuln | grep 21
```

### Adminer shows blank page
```bash
# Check PHP-FPM
docker logs adminer

# Verify NGINX proxy
docker exec nginx nginx -t
```

### Static site not loading
```bash
# Check static-site container
docker logs static-site

# Test direct access
docker exec nginx curl http://static-site:8080
```

### Portainer won't start
```bash
# Check Docker socket permission
docker exec portainer ls -la /var/run/docker.sock

# View logs
docker logs portainer
```

---

## 💡 Bonus Features Explained

### Why Redis?
- **Caches database queries** → Faster page loads
- **Reduces MariaDB load** → Better performance
- **WordPress plugin support** → Easy integration

### Why FTP?
- **Direct file access** → Edit WordPress themes/plugins
- **File transfer** → Upload media files
- **Backup capability** → Download WordPress files

### Why Adminer?
- **Lightweight** → Single PHP file (vs phpMyAdmin)
- **Database management** → Run SQL queries visually
- **User-friendly** → Better UX than command-line mysql

### Why Static Site?
- **Showcase skills** → Custom HTML/CSS
- **Performance** → Pure NGINX, no PHP
- **Bonus requirement** → Non-PHP service

### Why Portainer?
- **Visual management** → See all containers at a glance
- **Easy debugging** → View logs, inspect containers
- **Professional tool** → Real-world Docker management

---

## 📈 Expected Container Count

After deploying bonuses:

```bash
docker ps
```

Should show **8 containers**:
1. ✅ nginx (mandatory)
2. ✅ wordpress (mandatory)
3. ✅ mariadb (mandatory)
4. 🎁 redis (bonus)
5. 🎁 ftp (bonus)
6. 🎁 adminer (bonus)
7. 🎁 static-site (bonus)
8. 🎁 portainer (bonus)

---

## ✅ Evaluation Checklist

- [ ] All mandatory services still working
- [ ] Redis caching active (check wp redis status)
- [ ] FTP accessible (test file upload)
- [ ] Adminer loads and connects to MariaDB
- [ ] Static site displays correctly
- [ ] Portainer shows all containers
- [ ] All containers restart on crash
- [ ] No hacky infinite loops (tail -f, sleep infinity)
- [ ] Dockerfiles use Debian Bookworm base
- [ ] No :latest tags used

---

## 🎯 Quick Deployment Commands

```bash
# Complete deployment from scratch
ssh -p 2222 ggrisole@localhost << 'EOF'
cd ~/inception
docker compose down -v
sudo rm -rf ~/data/mariadb/* ~/data/wordpress/*
mkdir -p ~/data/mariadb ~/data/wordpress
docker compose up -d --build
docker compose logs -f
EOF
```

---

## 📝 Notes for Evaluators

1. **Redis Verification**: `docker exec wordpress wp redis status --allow-root`
2. **FTP Verification**: Login via FTP and list `/var/www/html`
3. **Adminer Verification**: Access via browser, login to database
4. **Static Site Verification**: Beautiful custom page at /static
5. **Portainer Verification**: Docker management UI accessible

All bonus services follow the same rules as mandatory:
- ✅ Custom Dockerfiles (no ready-made images)
- ✅ Debian Bookworm base
- ✅ Proper CMD (no infinite loops)
- ✅ Auto-restart on crash
- ✅ Connected to inception_network

**Bonus services are PRODUCTION-READY!** 🚀
