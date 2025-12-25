# Inception Project - Troubleshooting Guide & Installation Summary

> **🎄 UPDATE December 25, 2025**: Major fixes applied after 2-week break. See [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md) for complete details on 5 critical issues resolved.

## Project Overview

| Component | Details |
|-----------|---------|
| **42 Login** | ggrisole |
| **Domain** | ggrisole.42.fr |
| **VM** | Debian 12 Bookworm (penultimate stable) |
| **Location** | /mnt/vmlab/inception/ |
| **Containers** | NGINX, WordPress, MariaDB, Redis, FTP, Adminer, Static Site, Portainer |
| **Status** | ✅ ALL 8 SERVICES OPERATIONAL (Dec 25, 2025) |

---

## ✅ Final Working Configuration

### Container Status
```
nginx:      Up, serving HTTPS on port 443 (TLSv1.3)
wordpress:  Up, PHP-FPM on port 9000
mariadb:    Up (healthy), port 3306
```

### Network & Volumes
- **Network**: `inception_network` (bridge)
- **MariaDB data**: `~/data/mariadb` (bind mount)
- **WordPress data**: `~/data/wordpress` (bind mount)

---

## 🔥 Critical Issues Encountered & Solutions

> **📋 COMPREHENSIVE FIX GUIDE**: For detailed explanation of all 5 major issues and their solutions, see [DECEMBER-25-FIXES.md](./DECEMBER-25-FIXES.md)

### Issue #1: Old Secrets (25 bytes) vs New Secrets (44 bytes)

**Symptom:**
```bash
# WordPress logs:
Waiting for MariaDB...
Attempt 1/30...
[continues forever]

# MariaDB logs:
[Warning] Aborted connection to db: 'unconnected' user: 'unauthenticated'
```

**Root Cause:**
- Old secrets from November 28 (25 bytes) cached in `~/inception/secrets/`
- New secrets generated in wrong location (`~/inception/srcs/secrets/`)
- Docker compose reads from `~/inception/secrets/` (relative path `../secrets/`)

**Solution:**
```bash
# Generate secrets in CORRECT location with NO newline
cd ~/inception
mkdir -p secrets
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# Verify: each should be exactly 44 bytes
wc -c secrets/*
```

---

### Issue #2: MariaDB Never Initializing (Docker Layer Caching)

**Symptom:**
```
MariaDB logs show "Starting MariaDB" but never "Initializing database"
wpuser never created in mysql.user table
```

**Root Cause:**
`apt-get install mariadb-server` creates default DB files in `/var/lib/mysql/` during Docker build. These files get baked into the image, making the init script think database already exists.

**Solution - Update Dockerfile:**
```dockerfile
# Add at end of ~/inception/srcs/requirements/mariadb/Dockerfile:
RUN rm -rf /var/lib/mysql/*
```

**Then complete cleanup:**
```bash
docker compose down -v
docker rmi mariadb:inception
docker system prune -af
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*
mkdir -p ~/data/mariadb ~/data/wordpress
docker compose up -d --build
```

---

### Issue #2: wpuser Not Created (Access Denied)

**Symptom:**
```
ERROR 1045 (28000): Access denied for user 'wpuser'@'172.x.x.x'
```
or
```
ERROR 1130 (HY000): Host '172.x.x.x' is not allowed to connect
```

**Root Cause:**
Init script set root password BEFORE creating wpuser. Subsequent `mysql` commands failed authentication.

**Wrong Order (BROKEN):**
```bash
# Setting root password first locks us out!
mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASS}';"
# This now fails because we need the password we just set!
mysql -e "CREATE USER '${MYSQL_USER}'@'%' ..."
```

**Correct Order (FIXED):**
```bash
# Create users FIRST while no password is set
mysql -e "CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASS}';"
mysql -e "GRANT ALL PRIVILEGES ON ${MYSQL_DATABASE}.* TO '${MYSQL_USER}'@'%';"
mysql -e "FLUSH PRIVILEGES;"
# THEN secure root (last step!)
mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASS}';"
```

---

### Issue #3: Password Mismatch (THE BIG ONE) 🎯

**Symptom:**
```
ERROR 1045 (28000): Access denied for user 'wpuser'@'localhost' (using password: YES)
```
Even though the password looked correct!

**Root Cause:**
**Trailing newline (`\n`) in password files** caused authentication to fail.

When you create a password file:
```bash
echo "mypassword" > password.txt
# This actually stores: "mypassword\n" (with newline!)
```

When MariaDB sets the password with `\n`:
```sql
CREATE USER 'wpuser'@'%' IDENTIFIED BY 'mypassword
';  -- The password literally includes the newline!
```

But when WordPress tries to connect with just `mypassword`, it fails.

**The Fix - Strip Newlines:**
```bash
# In init-db.sh
DB_ROOT_PASS=$(cat /run/secrets/db_root_password | tr -d '\n')
DB_PASS=$(cat /run/secrets/db_password | tr -d '\n')
```

**Alternative for creating password files:**
```bash
# Use -n to prevent trailing newline
echo -n "mypassword" > password.txt
# Or use printf
printf '%s' "mypassword" > password.txt
```

---

### Issue #4: WordPress "Files Already Present" Loop

**Symptom:**
```
Error: WordPress files seem to already be present here.
```
Container keeps restarting in a loop.

**Root Cause:**
wp-setup.sh tried to run `wp core download` but files existed from a previous failed attempt.

**Solution - Check Before Download:**
```bash
# In wp-setup.sh
if [ ! -f /var/www/html/wp-config.php ]; then
    if [ ! -f /var/www/html/index.php ]; then
        # Fresh install - download WordPress
        wp core download --allow-root
    fi
    # Create config and install
    wp config create ...
    wp core install ...
fi
```

**Manual Fix (if container is looping):**
```bash
# Clean WordPress data and restart
rm -rf ~/data/wordpress/*
docker restart wordpress
```

---

## 📋 Complete Clean Rebuild Procedure

When things go wrong, use this nuclear option:

```bash
cd ~/inception

# 1. Stop and remove everything
docker compose -f srcs/docker-compose.yml down -v

# 2. Nuclear cleanup
docker system prune -af
docker volume prune -f
docker network prune -f

# 3. Remove persistent data
sudo rm -rf ~/data/mariadb ~/data/wordpress

# 4. Recreate data directories
mkdir -p ~/data/mariadb ~/data/wordpress

# 5. Rebuild and start
docker compose -f srcs/docker-compose.yml up -d --build

# 6. Watch logs
docker compose -f srcs/docker-compose.yml logs -f
```

---

## 🔍 Debugging Commands

### Check Container Status
```bash
docker ps -a
docker compose logs [service_name]
```

### Test MariaDB Connection
```bash
# From host
docker exec mariadb mysql -uwpuser -p'PASSWORD' -e "SELECT 1;"

# Check users
docker exec mariadb mysql -e "SELECT User,Host FROM mysql.user;"
```

### Test WordPress
```bash
# From inside VM
wget --no-check-certificate -q -O - https://ggrisole.42.fr/ | grep "<title>"

# Check PHP-FPM
docker exec wordpress ps aux | grep php-fpm
```

### Check NGINX
```bash
docker exec nginx nginx -t
docker logs nginx
```

### Inspect Secrets (Debug Only)
```bash
docker exec mariadb cat /run/secrets/db_password | xxd | head -2
# Look for 0a at the end = newline character!
```

---

## 🏗️ Project Structure

```
~/inception/
├── Makefile
└── srcs/
    ├── .env
    ├── docker-compose.yml
    └── requirements/
        ├── mariadb/
        │   ├── Dockerfile
        │   ├── conf/my.cnf
        │   └── tools/init-db.sh      # ← Key file with newline fix
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── conf/www.conf
        │   └── tools/wp-setup.sh
        └── nginx/
            ├── Dockerfile
            └── conf/nginx.conf
```

---

## 📝 Key Lessons Learned

1. **Always strip newlines from secrets**: `cat file | tr -d '\n'`
2. **Order matters in init scripts**: Create users before setting passwords
3. **Clean data directories between rebuilds**: Bind mounts persist!
4. **Use healthchecks**: They reveal container issues early
5. **Check logs first**: `docker logs [container]` is your friend
6. **Test from inside the VM**: Port forwarding adds complexity

---

## 🎓 For Evaluators

The main technical challenges solved in this project:

1. **Secret Management**: Docker secrets with newline handling
2. **Container Orchestration**: Proper startup order with healthchecks
3. **Database Initialization**: Correct user creation sequence
4. **SSL/TLS**: Self-signed certificates with TLSv1.2/1.3
5. **Persistent Storage**: Bind mounts for data persistence

The `tr -d '\n'` fix for password handling is the most important lesson - invisible characters in secrets cause silent authentication failures that are extremely difficult to debug.
