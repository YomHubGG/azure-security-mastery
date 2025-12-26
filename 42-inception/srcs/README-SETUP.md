# Inception Project - Setup Guide

## 📋 Project Overview

This project creates a Docker infrastructure with the following services:
- **NGINX** (TLSv1.3, self-signed certificate)
- **WordPress** (with PHP-FPM)
- **MariaDB** (database)
- **Redis** (object cache)
- **FTP Server** (vsftpd)
- **Adminer** (database management)
- **Static Site** (custom NGINX)
- **Portainer** (container management)

All services run in separate Docker containers orchestrated with docker-compose.

---

## 🚀 Quick Start

### Prerequisites

- Debian/Ubuntu-based Linux system
- Docker and Docker Compose installed
- Make utility
- Sudo privileges

### Installation

```bash
# 1. Clone the repository
git clone <your-repo>
cd inception

# 2. Generate secrets (REQUIRED - see below)
mkdir -p secrets
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# 3. Verify secrets are 44 bytes each
wc -c secrets/*
# Expected output: 44 secrets/db_root_password.txt
#                  44 secrets/db_password.txt
#                  44 secrets/wp_admin_password.txt

# 4. Configure environment variables
cd srcs
# Edit .env file with your domain and credentials
# Default domain: ggrisole.42.fr

# 5. Add domain to /etc/hosts
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts

# 6. Create data directories
mkdir -p ~/data/mariadb ~/data/wordpress

# 7. Build and start services
cd ..
make

# 8. Verify all containers are running
docker ps
# Should show 8 running containers
```

---

## 🔑 Secrets Generation (CRITICAL)

**⚠️ Secrets must be exactly 44 bytes (base64-encoded 32 random bytes with no newline)**

### Why 44 bytes?
- `openssl rand -base64 32` generates 32 random bytes
- Base64 encoding: 32 bytes × 4/3 = 42.67 → padded to 44 characters
- No trailing newline (use `tr -d '\n'`)

### Generate Secrets

```bash
cd inception  # Project root
mkdir -p secrets

# Generate all three secrets
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# VERIFY sizes (must be 44 bytes each)
wc -c secrets/*
```

**❌ Common Mistakes:**
```bash
# WRONG: Creates 45-byte secret (includes newline)
openssl rand -base64 32 > secrets/db_password.txt

# WRONG: Creates 33-byte secret (not base64 encoded)
openssl rand 32 > secrets/db_password.txt

# ✅ CORRECT: 44-byte base64-encoded secret with no newline
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
```

---

## ⚙️ Environment Configuration

### Edit srcs/.env

```bash
# Domain name (add to /etc/hosts pointing to 127.0.0.1)
DOMAIN_NAME=ggrisole.42.fr

# MariaDB Configuration
MYSQL_ROOT_PASSWORD_FILE=/run/secrets/db_root_password
MYSQL_DATABASE=wordpress_db
MYSQL_USER=wpuser
MYSQL_PASSWORD_FILE=/run/secrets/db_password

# WordPress Configuration
WP_URL=https://ggrisole.42.fr:8443
WP_TITLE=Inception Project
WP_ADMIN_USER=admin
WP_ADMIN_PASSWORD_FILE=/run/secrets/wp_admin_password
WP_ADMIN_EMAIL=admin@example.com

# WordPress Additional User
WP_USER=author
WP_USER_EMAIL=author@example.com
WP_USER_PASSWORD=author_secure_password

# FTP Configuration
FTP_USER=ftpuser
FTP_PASSWORD=ftppass
```

**⚠️ Important:** If using non-standard HTTPS port (8443), make sure `WP_URL` includes the port!

---

## 🏗️ Project Structure

```
inception/
├── Makefile                              # Build and control commands
├── secrets/                              # Docker secrets (NEVER commit!)
│   ├── db_root_password.txt
│   ├── db_password.txt
│   └── wp_admin_password.txt
└── srcs/
    ├── .env                              # Environment variables
    ├── docker-compose.yml                # Service orchestration
    └── requirements/
        ├── nginx/
        │   ├── Dockerfile
        │   └── conf/nginx.conf
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── conf/www.conf
        │   └── tools/wp-setup.sh
        ├── mariadb/
        │   ├── Dockerfile
        │   └── tools/init-db.sh
        └── bonus/
            ├── redis/
            │   ├── Dockerfile
            │   └── conf/redis.conf
            ├── ftp/
            │   ├── Dockerfile
            │   └── conf/vsftpd.conf
            ├── adminer/
            │   └── Dockerfile
            ├── portainer/
            │   └── Dockerfile
            └── static-site/
                ├── Dockerfile
                └── tools/
                    ├── index.html
                    └── style.css
```

---

## 🎯 Makefile Commands

```bash
make        # Build and start all services
make up     # Start services (without rebuilding)
make down   # Stop and remove all containers
make clean  # Remove containers and images
make fclean # Complete cleanup (containers, images, volumes, data)
make re     # Full rebuild (clean + make)
```

---

## 🌐 Service Access

### From Host Machine

| Service | URL | Credentials |
|---------|-----|-------------|
| **WordPress** | `https://ggrisole.42.fr:8443` | admin / (secret file) |
| **WordPress** | `https://ggrisole.42.fr` | Standard HTTPS port |
| **Adminer** | `https://ggrisole.42.fr:8443/adminer` | System: MySQL<br>Server: mariadb<br>User: wpuser<br>Password: (secret file)<br>Database: wordpress_db |
| **Static Site** | `https://ggrisole.42.fr:8443/static` | Public access |
| **Portainer** | `http://localhost:9443` | Create admin on first visit |
| **FTP** | `ftp://localhost:21` | ftpuser / ftppass |

### SSL Certificate Warning

The NGINX certificate is self-signed. Your browser will show a security warning:
1. Click "Advanced"
2. Click "Accept the Risk and Continue"
3. Certificate details: CN=ggrisole.42.fr, valid for 365 days

---

## 🔍 Troubleshooting

### Issue: MariaDB Never Initializes Database

**Symptom:**
```bash
docker logs mariadb
# Shows: "Starting MariaDB..." but never "Initializing MariaDB database..."
```

**Cause:** Old database files persisting in data directory or Docker image layers.

**Fix:**
```bash
make down
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
docker rmi mariadb:inception
make
```

### Issue: WordPress Redirects to Wrong URL

**Symptom:** Accessing `https://ggrisole.42.fr:8443` redirects to `https://ggrisole.42.fr` (drops port).

**Fix:**
```bash
# Update WordPress URLs to include port
docker exec wordpress wp option update siteurl 'https://ggrisole.42.fr:8443' --allow-root
docker exec wordpress wp option update home 'https://ggrisole.42.fr:8443' --allow-root
```

### Issue: Portainer Shows 404 Page

**Symptom:** Portainer API works (`/api/status`) but web UI returns 404.

**Cause:** Web UI files (`public/` directory) not extracted properly.

**Fix:** Dockerfile extracts entire portainer archive to `/usr/local/bin/portainer/` and sets `WORKDIR` to that directory.

### Issue: Secret Authentication Failures

**Symptom:** MariaDB or WordPress fail with "Access denied" errors.

**Fix:**
```bash
# Check secret file sizes
wc -c secrets/*
# Must be exactly 44 bytes each

# If wrong size, regenerate:
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
```

### Issue: Services Not Accessible from Host

**Symptom:** `curl https://localhost:8443` fails with "Connection refused".

**Fix:**
```bash
# Check containers are running
docker ps

# Check NGINX is listening
docker exec nginx netstat -tlnp | grep 443

# Check /etc/hosts has domain entry
grep ggrisole.42.fr /etc/hosts
# Should show: 127.0.0.1 ggrisole.42.fr
```

---

## 🧪 Testing & Validation

### 1. Container Health Check

```bash
docker ps --format 'table {{.Names}}\t{{.Status}}'
# Expected: 8 containers, all "Up" status
```

### 2. MariaDB Database Creation

```bash
docker exec mariadb mysql -u root -p$(cat secrets/db_root_password.txt) \
  -e "SELECT User, Host FROM mysql.user;"
# Expected: wpuser with '%' host present
```

### 3. WordPress Database Connection

```bash
docker exec wordpress mysql -h mariadb -u wpuser \
  -p$(cat secrets/db_password.txt) -e "SELECT 1;"
# Expected: 1
```

### 4. Redis Connection

```bash
docker exec wordpress redis-cli -h redis ping
# Expected: PONG
```

### 5. WordPress Installation

```bash
docker exec wordpress wp core is-installed --allow-root
# Expected: (no output, exit code 0)
```

### 6. Service HTTP Responses

```bash
# WordPress
curl -k -s https://localhost:8443 | grep '<title>'
# Expected: <title>Inception Project</title>

# Adminer
curl -k -s https://localhost:8443/adminer | grep '<title>'
# Expected: <title>Login - Adminer</title>

# Static Site
curl -k -s https://localhost:8443/static/ | grep '<title>'
# Expected: <title>Inception Project - Static Site</title>

# Portainer
curl -s http://localhost:9443/ | grep 'Portainer'
# Expected: HTML containing "Portainer"
```

---

## 📚 Technical Details

### TLS Configuration

NGINX uses TLSv1.3 with self-signed certificate:
```bash
# Certificate location: /etc/nginx/ssl/nginx.crt
# Private key: /etc/nginx/ssl/nginx.key
# Valid for: 365 days
# Subject: CN=ggrisole.42.fr
```

### Volume Persistence

- **MariaDB data:** `~/data/mariadb` (bind mount)
- **WordPress files:** `~/data/wordpress` (bind mount)
- Persistent across container restarts
- Backed up by host filesystem

### Network Architecture

- All containers on Docker bridge network: `inception-network`
- Internal DNS resolution (e.g., `mariadb` resolves to MariaDB container)
- Only NGINX, Portainer, and FTP expose ports to host

### Security Features

1. **Docker Secrets:** Passwords stored as Docker secrets, never in environment variables
2. **No Hardcoded Credentials:** All sensitive data in separate secret files
3. **TLS Encryption:** HTTPS with TLSv1.3 for web traffic
4. **Isolated Network:** Containers communicate via internal Docker network
5. **Non-Root Processes:** Services run as dedicated users (www-data, mysql, etc.)

---

## 🎓 42 School Requirements

This project fulfills all mandatory and bonus requirements:

### Mandatory Services
- ✅ NGINX with TLSv1.3
- ✅ WordPress with PHP-FPM
- ✅ MariaDB
- ✅ Volumes for database and website files
- ✅ Docker network
- ✅ Automatic restart on failure

### Bonus Services
- ✅ Redis cache for WordPress
- ✅ FTP server
- ✅ Adminer (database management)
- ✅ Static website
- ✅ Portainer (container management)

### Technical Requirements
- ✅ Each service in separate container
- ✅ Custom Dockerfiles for each service (no ready-made images)
- ✅ Docker Compose orchestration
- ✅ Secrets management
- ✅ Environment variables configuration

---

## 📝 Credits

**Project:** Inception (42 School System Administration)  
**Author:** ggrisole  
**Date:** December 2024

For detailed debugging history and issue resolution, see `DECEMBER-25-FIXES.md`.
