# Quick Fix Reference - December 25, 2025

> **Fast reference for the 8 critical fixes applied** (Issues #8-15). For full details, see [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md)

**Infrastructure Fixes (15:00-17:30)**: Issues #8-12  
**Bonus Service Fixes (17:30-18:00)**: Issues #13-14  
**Browser Access Fix (18:15)**: Issue #15

---

## 🔴 Issue #8: Wrong Secret Sizes (25 vs 44 bytes)

```bash
# Generate correct 44-byte secrets (NO newline!)
cd ~/inception
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# Verify (must be exactly 44 bytes each)
wc -c secrets/*
```

---

## 🔴 Issue 2: MariaDB Never Initializes

**Add to MariaDB Dockerfile:**
```dockerfile
# At the end of ~/inception/srcs/requirements/mariadb/Dockerfile
RUN rm -rf /var/lib/mysql/*
```

**Then rebuild:**
```bash
docker rmi mariadb:inception
sudo rm -rf ~/data/mariadb/*
make
```

---

## 🔴 Issue 3: WordPress Initialization Loop

```bash
# Clean WordPress data
sudo rm -rf ~/data/wordpress/*
docker restart wordpress
sleep 30 && docker logs wordpress
```

---

## 🔴 Issue 4: Portainer 404 on Web UI

**Update Portainer Dockerfile:**
```dockerfile
# Extract entire directory (not just binary)
RUN tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/ && \
    rm portainer-2.19.4-linux-amd64.tar.gz

WORKDIR /usr/local/bin/portainer
CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
```

**Rebuild:**
```bash
docker-compose stop portainer
docker-compose rm -f portainer
docker rmi portainer:inception
docker-compose build portainer
docker-compose up -d portainer
```

---

## 🔴 Issue 5: Port 443 Forwarding

```bash
# Add while VM is running
VBoxManage controlvm "inception" natpf1 "https443,tcp,,443,,443"

# Verify
VBoxManage showvminfo "inception" | grep Rule
```

---

## ✅ Complete Reset Procedure

If you need to start fresh:

```bash
# 1. Stop everything
cd ~/inception
make down

# 2. Remove images
docker rmi mariadb:inception portainer:inception

# 3. Clean all data
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*
docker volume prune -f

# 4. Regenerate secrets (44 bytes each)
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# 5. Update Dockerfiles (MariaDB + Portainer)
# - MariaDB: Add "RUN rm -rf /var/lib/mysql/*"
# - Portainer: Extract to /usr/local/bin/portainer/ with WORKDIR

# 6. Rebuild
make

# 7. Verify
docker ps
docker logs mariadb | grep "Initializing"
docker logs wordpress | grep "complete"
curl http://localhost:9443/ | grep Portainer
```

---

## 🔴 Issue #13: Adminer 502 Bad Gateway

**Update nginx.conf (FastCGI not HTTP proxy):**
```nginx
# Replace proxy_pass with fastcgi_pass
location ~ ^/adminer(/.*\.php)?$ {
    fastcgi_pass adminer:9001;
    fastcgi_index adminer.php;
    include fastcgi_params;
    fastcgi_param SCRIPT_FILENAME /var/www/html/adminer.php;
    fastcgi_param SCRIPT_NAME /adminer;
}
```

**Rebuild:**
```bash
cd ~/inception/srcs
docker-compose stop nginx && docker-compose rm -f nginx
docker-compose build nginx && docker-compose up -d nginx
```

**Test:**
```bash
curl -k -s https://ggrisole.42.fr:8443/adminer | grep '<title>'
# Expected: <title>Login - Adminer</title>
```

---

## 🔴 Issue #14: Static Site 404 Not Found

**Update nginx.conf (add trailing slashes):**
```nginx
# Change from:
location /static {
    proxy_pass http://static-site:8080;
}

# To:
location /static/ {
    proxy_pass http://static-site:8080/;  # ← Trailing slashes!
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}
```

**Rebuild:**
```bash
cd ~/inception/srcs
docker-compose stop nginx && docker-compose rm -f nginx
docker-compose build nginx && docker-compose up -d nginx
```

**Test:**
```bash
curl -k -s https://ggrisole.42.fr:8443/static/ | grep '<title>'
# Expected: <title>Inception Project - Static Site</title>
```

---

## 🔴 Issue #15: Firefox Connection Refused After Certificate

**Update WordPress URLs to include port:**
```bash
ssh -p 2222 ggrisole@localhost \
  "docker exec wordpress wp option update siteurl 'https://ggrisole.42.fr:8443' --allow-root && \
   docker exec wordpress wp option update home 'https://ggrisole.42.fr:8443' --allow-root"
```

**Test:**
```bash
# Verify URLs updated
docker exec wordpress wp option get siteurl --allow-root
# Expected: https://ggrisole.42.fr:8443

# Test in Firefox
firefox https://localhost:8443 &
# Accept certificate warning, site should load and stay on port 8443
```

---

## 📊 Verification Checklist

```bash
# ✅ Secrets are 44 bytes
wc -c ~/inception/secrets/*

# ✅ MariaDB initialized
docker logs mariadb | grep "Creating database and user"

# ✅ wpuser exists
docker exec mariadb mysql -u root -e "SELECT User FROM mysql.user WHERE User='wpuser';"

# ✅ WordPress installed
docker logs wordpress | grep "WordPress installation complete"

# ✅ Portainer web UI loads
curl -s http://localhost:9443/ | grep "<title>Portainer</title>"

# ✅ Adminer web UI loads
curl -k -s https://ggrisole.42.fr:8443/adminer | grep "Adminer"

# ✅ Static site loads
curl -k -s https://ggrisole.42.fr:8443/static/ | grep "Inception Project"

# ✅ WordPress URLs have correct port
docker exec wordpress wp option get siteurl --allow-root | grep ":8443"

# ✅ All containers running
docker ps --format 'table {{.Names}}\t{{.Status}}'
```

---

## 🚀 Access URLs

| Service | URL |
|---------|-----|
| WordPress | https://ggrisole.42.fr:8443 |
| Adminer | https://ggrisole.42.fr:8443/adminer |
| Static Site | https://ggrisole.42.fr:8443/static/ |
| Portainer | http://localhost:9443 |

---

## 🎯 Key Takeaways

1. **Secrets must be 44 bytes** - Use `openssl rand -base64 32 | tr -d '\n'`
2. **MariaDB needs clean slate** - Add `RUN rm -rf /var/lib/mysql/*` to Dockerfile
3. **WordPress data persists** - Delete `~/data/wordpress/*` for fresh install
4. **Portainer needs full extraction** - Don't just copy binary, extract entire directory
5. **Port forwarding can be added live** - Use `VBoxManage controlvm`
6. **PHP-FPM needs fastcgi_pass** - Not proxy_pass (FastCGI ≠ HTTP)
7. **Trailing slashes matter** - `location /path/` + `proxy_pass http://backend/` = path rewriting
8. **WordPress URLs need port** - Set `siteurl` and `home` to include `:8443` for non-standard ports

---

For detailed explanations, root cause analysis, and complete fix workflows, see [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md)
