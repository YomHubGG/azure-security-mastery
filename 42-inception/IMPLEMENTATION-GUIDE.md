# Inception Project - Complete Implementation Guide

**Project Timeline**: November 17-30, 2025 (13 days)  
**Target**: Portable VM on external disk for campus evaluation  
**Strategy**: Build on local PC → Export VM to external disk

---

## 🎯 Project Requirements Summary

### Mandatory Services (3 containers)
1. **NGINX** - TLSv1.2/1.3 only, port 443, entry point
2. **WordPress** - php-fpm only (no nginx)
3. **MariaDB** - Database only (no nginx)

### Infrastructure Requirements
- ✅ All in a Virtual Machine
- ✅ Custom Dockerfiles (Alpine or Debian penultimate stable)
- ✅ Docker Compose orchestration
- ✅ Makefile to build everything
- ✅ Docker network connecting containers
- ✅ 2 volumes: WordPress DB + WordPress files
- ✅ Domain: `yom.42.fr` → localhost
- ✅ Auto-restart containers on crash
- ✅ Environment variables in `.env`
- ✅ Secrets management (no passwords in Dockerfiles)
- ✅ 2 WordPress users (one admin, username NOT 'admin')

### Bonus Services (5 additional containers)
1. **Redis** - Cache for WordPress
2. **FTP** - Access to WordPress volume
3. **Adminer** - Database management UI
4. **Static Website** - Portfolio/resume (not PHP)
5. **Custom Service** - Your choice (justify during defense)

### Prohibited Practices
- ❌ No `tail -f`, `sleep infinity`, `while true` hacks
- ❌ No `network: host` or `--link`
- ❌ No pre-built images from DockerHub (except Alpine/Debian)
- ❌ No passwords in Dockerfiles
- ❌ No `latest` tag
- ❌ No infinite loops in entrypoints

---

## 📅 13-Day Implementation Timeline

### **Phase 1: VM Setup & Environment** (Days 1-2: Nov 17-18)

#### Day 1 - Nov 17 (TODAY)
**Morning: VM Creation**
```bash
# VirtualBox setup on external disk
- Create VM: "inception-yom"
- OS: Debian 12 Bookworm (penultimate stable)
- RAM: 4GB minimum (8GB recommended)
- CPU: 2 cores minimum (4 recommended)
- Disk: 30GB dynamically allocated on EXTERNAL DISK
- Network: Bridged Adapter
- Enable USB 3.0 for external disk performance
```

**Afternoon: Base System Setup**
```bash
# Inside VM after Debian installation
sudo apt update && sudo apt upgrade -y

# Install Docker + Docker Compose
sudo apt install -y docker.io docker-compose make git vim net-tools

# Add user to docker group
sudo usermod -aG docker $USER
newgrp docker

# Verify installation
docker --version
docker-compose --version
```

**Evening: Project Structure**
```bash
# Create project structure
mkdir -p ~/inception
cd ~/inception

# Create directory structure
mkdir -p secrets
mkdir -p srcs/requirements/{nginx,wordpress,mariadb,bonus}/{conf,tools}
mkdir -p srcs/requirements/bonus/{redis,ftp,adminer,static-site,custom}

# Create .gitignore
cat > .gitignore << 'EOF'
secrets/
.env
*.log
data/
EOF

# Create files
touch Makefile
touch srcs/docker-compose.yml
touch srcs/.env
```

#### Day 2 - Nov 18
**Morning: Secrets & Environment Setup**
```bash
# Generate strong passwords
openssl rand -base64 32 > secrets/db_root_password.txt
openssl rand -base64 32 > secrets/db_password.txt
openssl rand -base64 32 > secrets/wp_admin_password.txt

# Create .env file
cat > srcs/.env << 'EOF'
# Domain Configuration
DOMAIN_NAME=yom.42.fr

# MariaDB Configuration
MYSQL_ROOT_PASSWORD_FILE=/run/secrets/db_root_password
MYSQL_DATABASE=wordpress
MYSQL_USER=wpuser
MYSQL_PASSWORD_FILE=/run/secrets/db_password

# WordPress Configuration
WP_ADMIN_USER=yomadmin
WP_ADMIN_PASSWORD_FILE=/run/secrets/wp_admin_password
WP_ADMIN_EMAIL=yom@student.42.fr
WP_USER=yomuser
WP_USER_EMAIL=yomuser@student.42.fr
WP_USER_PASSWORD=UserPassword123!
WP_TITLE="Yom's Inception Project"
WP_URL=https://yom.42.fr

# Network Configuration
NETWORK_NAME=inception_network

# Volume Configuration
DB_VOLUME_PATH=/home/yom/data/mariadb
WP_VOLUME_PATH=/home/yom/data/wordpress
EOF
```

**Afternoon: Domain & SSL Setup**
```bash
# Add domain to /etc/hosts
echo "127.0.0.1 yom.42.fr" | sudo tee -a /etc/hosts

# Generate self-signed SSL certificate
mkdir -p srcs/requirements/nginx/tools
cd srcs/requirements/nginx/tools

openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout nginx-selfsigned.key \
  -out nginx-selfsigned.crt \
  -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=Student/CN=yom.42.fr"
```

**Evening: Start Makefile**
```makefile
# Create Makefile at project root
cat > Makefile << 'EOF'
COMPOSE_FILE = srcs/docker-compose.yml
ENV_FILE = srcs/.env
DATA_DIR = /home/yom/data

all: setup up

setup:
	@echo "Creating data directories..."
	@sudo mkdir -p $(DATA_DIR)/mariadb
	@sudo mkdir -p $(DATA_DIR)/wordpress
	@sudo chown -R $(USER):$(USER) $(DATA_DIR)

build:
	@echo "Building Docker images..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) build --no-cache

up: setup
	@echo "Starting services..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) up -d

down:
	@echo "Stopping services..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) down

clean: down
	@echo "Cleaning containers and images..."
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) down -v --rmi all

fclean: clean
	@echo "Removing data directories..."
	@sudo rm -rf $(DATA_DIR)

re: fclean all

logs:
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) logs -f

ps:
	docker-compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE) ps

.PHONY: all setup build up down clean fclean re logs ps
EOF
```

---

### **Phase 2: Core Services** (Days 3-5: Nov 19-21)

#### Day 3 - Nov 19: MariaDB Container

**Morning: MariaDB Dockerfile**
```dockerfile
# srcs/requirements/mariadb/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    mariadb-server \
    mariadb-client \
    && rm -rf /var/lib/apt/lists/*

# Copy configuration and scripts
COPY conf/my.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/init-db.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/init-db.sh

# Create directory for socket
RUN mkdir -p /run/mysqld && chown -R mysql:mysql /run/mysqld

EXPOSE 3306

ENTRYPOINT ["/usr/local/bin/init-db.sh"]
CMD ["mysqld"]
```

**Afternoon: MariaDB Configuration**
```ini
# srcs/requirements/mariadb/conf/my.cnf
[mysqld]
user = mysql
port = 3306
bind-address = 0.0.0.0
datadir = /var/lib/mysql
socket = /run/mysqld/mysqld.sock

# Enable logging
log_error = /var/log/mysql/error.log

# InnoDB settings
innodb_buffer_pool_size = 256M
innodb_log_file_size = 64M
innodb_flush_method = O_DIRECT

# Character set
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
```

**Evening: MariaDB Init Script**
```bash
# srcs/requirements/mariadb/tools/init-db.sh
#!/bin/bash
set -e

# Read secrets
DB_ROOT_PASS=$(cat /run/secrets/db_root_password)
DB_PASS=$(cat /run/secrets/db_password)

# Initialize database if not exists
if [ ! -d "/var/lib/mysql/mysql" ]; then
    echo "Initializing MariaDB database..."
    mysql_install_db --user=mysql --datadir=/var/lib/mysql
    
    # Start temporary server
    mysqld --user=mysql --datadir=/var/lib/mysql --skip-networking &
    pid="$!"
    
    # Wait for server to start
    until mysqladmin ping &>/dev/null; do
        sleep 1
    done
    
    # Secure installation and create database
    mysql -e "DELETE FROM mysql.user WHERE User='';"
    mysql -e "DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');"
    mysql -e "DROP DATABASE IF EXISTS test;"
    mysql -e "DELETE FROM mysql.db WHERE Db='test' OR Db='test\\_%';"
    mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASS}';"
    
    # Create database and user
    mysql -uroot -p"${DB_ROOT_PASS}" -e "CREATE DATABASE IF NOT EXISTS ${MYSQL_DATABASE};"
    mysql -uroot -p"${DB_ROOT_PASS}" -e "CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASS}';"
    mysql -uroot -p"${DB_ROOT_PASS}" -e "GRANT ALL PRIVILEGES ON ${MYSQL_DATABASE}.* TO '${MYSQL_USER}'@'%';"
    mysql -uroot -p"${DB_ROOT_PASS}" -e "FLUSH PRIVILEGES;"
    
    # Stop temporary server
    mysqladmin -uroot -p"${DB_ROOT_PASS}" shutdown
    wait "$pid"
    
    echo "MariaDB initialization complete!"
fi

# Start MariaDB normally
exec "$@"
```

#### Day 4 - Nov 20: WordPress Container

**Morning: WordPress Dockerfile**
```dockerfile
# srcs/requirements/wordpress/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    php8.2-fpm \
    php8.2-mysql \
    php8.2-curl \
    php8.2-gd \
    php8.2-mbstring \
    php8.2-xml \
    php8.2-xmlrpc \
    php8.2-zip \
    php8.2-redis \
    curl \
    mariadb-client \
    && rm -rf /var/lib/apt/lists/*

# Download WP-CLI
RUN curl -O https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar \
    && chmod +x wp-cli.phar \
    && mv wp-cli.phar /usr/local/bin/wp

# Copy configuration
COPY conf/www.conf /etc/php/8.2/fpm/pool.d/www.conf
COPY tools/wp-setup.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/wp-setup.sh

# Create directories
RUN mkdir -p /var/www/html /run/php
RUN chown -R www-data:www-data /var/www/html /run/php

WORKDIR /var/www/html

EXPOSE 9000

ENTRYPOINT ["/usr/local/bin/wp-setup.sh"]
CMD ["/usr/sbin/php-fpm8.2", "-F"]
```

**Afternoon: WordPress PHP-FPM Config**
```ini
# srcs/requirements/wordpress/conf/www.conf
[www]
user = www-data
group = www-data
listen = 9000
listen.owner = www-data
listen.group = www-data
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
```

**Evening: WordPress Setup Script**
```bash
# srcs/requirements/wordpress/tools/wp-setup.sh
#!/bin/bash
set -e

# Read secrets
DB_PASS=$(cat /run/secrets/db_password)
WP_ADMIN_PASS=$(cat /run/secrets/wp_admin_password)

# Wait for MariaDB
until mariadb -h mariadb -u"${MYSQL_USER}" -p"${DB_PASS}" -e "SELECT 1" &>/dev/null; do
    echo "Waiting for MariaDB..."
    sleep 3
done

# Download WordPress if not exists
if [ ! -f /var/www/html/wp-config.php ]; then
    echo "Installing WordPress..."
    
    # Download WordPress
    wp core download --allow-root
    
    # Create wp-config.php
    wp config create \
        --dbname="${MYSQL_DATABASE}" \
        --dbuser="${MYSQL_USER}" \
        --dbpass="${DB_PASS}" \
        --dbhost=mariadb:3306 \
        --allow-root
    
    # Install WordPress
    wp core install \
        --url="${WP_URL}" \
        --title="${WP_TITLE}" \
        --admin_user="${WP_ADMIN_USER}" \
        --admin_password="${WP_ADMIN_PASS}" \
        --admin_email="${WP_ADMIN_EMAIL}" \
        --allow-root
    
    # Create second user
    wp user create "${WP_USER}" "${WP_USER_EMAIL}" \
        --role=author \
        --user_pass="${WP_USER_PASSWORD}" \
        --allow-root
    
    # Configure Redis (if bonus)
    wp config set WP_REDIS_HOST redis --allow-root
    wp config set WP_REDIS_PORT 6379 --raw --allow-root
    wp plugin install redis-cache --activate --allow-root || true
    wp redis enable --allow-root || true
    
    echo "WordPress installation complete!"
fi

# Start PHP-FPM
exec "$@"
```

#### Day 5 - Nov 21: NGINX Container

**Morning: NGINX Dockerfile**
```dockerfile
# srcs/requirements/nginx/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    nginx \
    openssl \
    && rm -rf /var/lib/apt/lists/*

# Copy SSL certificates
COPY tools/nginx-selfsigned.crt /etc/ssl/certs/
COPY tools/nginx-selfsigned.key /etc/ssl/private/

# Copy NGINX configuration
COPY conf/nginx.conf /etc/nginx/nginx.conf
COPY conf/default.conf /etc/nginx/sites-available/default

RUN ln -sf /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default \
    && rm -f /etc/nginx/sites-enabled/default.old

EXPOSE 443

CMD ["nginx", "-g", "daemon off;"]
```

**Afternoon: NGINX Configuration**
```nginx
# srcs/requirements/nginx/conf/nginx.conf
user www-data;
worker_processes auto;
pid /run/nginx.pid;

events {
    worker_connections 1024;
}

http {
    include /etc/nginx/mime.types;
    default_type application/octet-stream;
    
    access_log /var/log/nginx/access.log;
    error_log /var/log/nginx/error.log;
    
    sendfile on;
    tcp_nopush on;
    tcp_nodelay on;
    keepalive_timeout 65;
    types_hash_max_size 2048;
    
    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;
    
    include /etc/nginx/sites-enabled/*;
}
```

```nginx
# srcs/requirements/nginx/conf/default.conf
server {
    listen 443 ssl;
    listen [::]:443 ssl;
    
    server_name yom.42.fr;
    
    # SSL Configuration
    ssl_certificate /etc/ssl/certs/nginx-selfsigned.crt;
    ssl_certificate_key /etc/ssl/private/nginx-selfsigned.key;
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    
    root /var/www/html;
    index index.php index.html index.htm;
    
    # WordPress permalinks
    location / {
        try_files $uri $uri/ /index.php?$args;
    }
    
    # PHP-FPM configuration
    location ~ \.php$ {
        include fastcgi_params;
        fastcgi_pass wordpress:9000;
        fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
        fastcgi_param PATH_INFO $fastcgi_path_info;
    }
    
    # Security: Deny access to hidden files
    location ~ /\. {
        deny all;
    }
}
```

**Evening: Docker Compose (Mandatory)**
```yaml
# srcs/docker-compose.yml
version: '3.8'

services:
  mariadb:
    container_name: mariadb
    build:
      context: ./requirements/mariadb
      dockerfile: Dockerfile
    image: mariadb:inception
    networks:
      - inception_network
    volumes:
      - mariadb_data:/var/lib/mysql
    secrets:
      - db_root_password
      - db_password
    env_file:
      - .env
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "mysqladmin", "ping", "-h", "localhost"]
      interval: 10s
      timeout: 5s
      retries: 5

  wordpress:
    container_name: wordpress
    build:
      context: ./requirements/wordpress
      dockerfile: Dockerfile
    image: wordpress:inception
    depends_on:
      mariadb:
        condition: service_healthy
    networks:
      - inception_network
    volumes:
      - wordpress_data:/var/www/html
    secrets:
      - db_password
      - wp_admin_password
    env_file:
      - .env
    restart: unless-stopped
    healthcheck:
      test: ["CMD-SHELL", "php-fpm8.2 -t"]
      interval: 10s
      timeout: 5s
      retries: 5

  nginx:
    container_name: nginx
    build:
      context: ./requirements/nginx
      dockerfile: Dockerfile
    image: nginx:inception
    depends_on:
      wordpress:
        condition: service_healthy
    networks:
      - inception_network
    volumes:
      - wordpress_data:/var/www/html:ro
    ports:
      - "443:443"
    env_file:
      - .env
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "nginx", "-t"]
      interval: 10s
      timeout: 5s
      retries: 5

networks:
  inception_network:
    driver: bridge

volumes:
  mariadb_data:
    driver: local
    driver_opts:
      type: none
      o: bind
      device: /home/yom/data/mariadb
  wordpress_data:
    driver: local
    driver_opts:
      type: none
      o: bind
      device: /home/yom/data/wordpress

secrets:
  db_root_password:
    file: ../secrets/db_root_password.txt
  db_password:
    file: ../secrets/db_password.txt
  wp_admin_password:
    file: ../secrets/wp_admin_password.txt
```

---

### **Phase 3: Bonus Services** (Days 6-9: Nov 22-25)

#### Day 6 - Nov 22: Redis Cache

**Redis Dockerfile**
```dockerfile
# srcs/requirements/bonus/redis/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    redis-server \
    && rm -rf /var/lib/apt/lists/*

COPY conf/redis.conf /etc/redis/redis.conf

EXPOSE 6379

CMD ["redis-server", "/etc/redis/redis.conf"]
```

**Redis Configuration**
```conf
# srcs/requirements/bonus/redis/conf/redis.conf
bind 0.0.0.0
port 6379
maxmemory 256mb
maxmemory-policy allkeys-lru
```

#### Day 7 - Nov 23: FTP Server

**FTP Dockerfile**
```dockerfile
# srcs/requirements/bonus/ftp/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    vsftpd \
    && rm -rf /var/lib/apt/lists/*

COPY conf/vsftpd.conf /etc/vsftpd.conf
COPY tools/ftp-setup.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/ftp-setup.sh

EXPOSE 21 21000-21010

ENTRYPOINT ["/usr/local/bin/ftp-setup.sh"]
CMD ["vsftpd", "/etc/vsftpd.conf"]
```

#### Day 8 - Nov 24: Adminer + Static Site

**Adminer Dockerfile**
```dockerfile
# srcs/requirements/bonus/adminer/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    php8.2-fpm \
    php8.2-mysql \
    wget \
    && rm -rf /var/lib/apt/lists/*

RUN wget "https://github.com/vrana/adminer/releases/download/v4.8.1/adminer-4.8.1.php" \
    -O /var/www/html/index.php

RUN mkdir -p /run/php && chown -R www-data:www-data /var/www/html

EXPOSE 9001

CMD ["/usr/sbin/php-fpm8.2", "-F"]
```

**Static Site (Portfolio)**
```dockerfile
# srcs/requirements/bonus/static-site/Dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    python3 \
    && rm -rf /var/lib/apt/lists/*

COPY tools/index.html /var/www/html/
COPY tools/style.css /var/www/html/

WORKDIR /var/www/html

EXPOSE 8080

CMD ["python3", "-m", "http.server", "8080"]
```

#### Day 9 - Nov 25: Custom Service (Monitoring)

**Portainer (Container Management UI)**
```yaml
# Add to docker-compose.yml bonus section
  portainer:
    container_name: portainer
    build:
      context: ./requirements/bonus/portainer
      dockerfile: Dockerfile
    image: portainer:inception
    networks:
      - inception_network
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock
      - portainer_data:/data
    ports:
      - "9443:9443"
    restart: unless-stopped
```

---

### **Phase 4: Security Hardening** (Days 10-11: Nov 26-27)

#### Day 10 - Nov 26: Security Implementation

**Morning: Apply Azure Security Knowledge**
- [ ] Day 35: Multi-stage builds, non-root users
- [ ] Day 53: Secrets management validation
- [ ] Day 55: Security headers, TLS hardening
- [ ] Day 9: Network isolation verification

**Security Checklist**:
```bash
# Run security scans
docker run --rm -v /var/run/docker.sock:/var/run/docker.sock \
  aquasec/trivy image mariadb:inception

docker run --rm -v /var/run/docker.sock:/var/run/docker.sock \
  aquasec/trivy image wordpress:inception

docker run --rm -v /var/run/docker.sock:/var/run/docker.sock \
  aquasec/trivy image nginx:inception

# Test TLS configuration
openssl s_client -connect yom.42.fr:443 -tls1_2
openssl s_client -connect yom.42.fr:443 -tls1_3

# Verify no passwords in Dockerfiles
grep -r "PASSWORD" srcs/requirements/*/Dockerfile || echo "✓ No hardcoded passwords"

# Check container user
docker exec nginx whoami  # Should not be root
docker exec wordpress whoami  # Should be www-data
```

**Afternoon: Documentation**
- [ ] Create architecture diagram
- [ ] Document setup process
- [ ] Write defense preparation notes

#### Day 11 - Nov 27: Testing & Validation

**Complete Testing Suite**:
```bash
# Test 1: Build and start
make re
sleep 30

# Test 2: Service accessibility
curl -k https://yom.42.fr
curl -k https://yom.42.fr:9443  # Portainer

# Test 3: WordPress functionality
# - Login with admin user (yomadmin)
# - Login with regular user (yomuser)
# - Create a post, verify Redis caching

# Test 4: Database access
docker exec mariadb mysql -u wpuser -p"$(cat secrets/db_password.txt)" -e "SHOW DATABASES;"

# Test 5: FTP connection
ftp yom.42.fr 21
# Login and test file operations

# Test 6: Container restart
docker stop nginx
sleep 5
docker ps | grep nginx  # Should auto-restart

# Test 7: Clean shutdown
make down
make up
```

---

### **Phase 5: VM Export & Documentation** (Days 12-13: Nov 28-30)

#### Day 12 - Nov 28: Final Polish

**Morning: Performance Optimization**
```bash
# Optimize Docker images
docker image prune -f

# Document resource usage
docker stats --no-stream

# Final security audit
bash srcs/requirements/tools/security-check.sh
```

**Afternoon: Create Defense Documentation**
```markdown
# Create DEFENSE-NOTES.md
- Architecture explanation
- Service interaction flow
- Security measures implemented
- Bonus services justification
- Common issues & solutions
```

#### Day 13 - Nov 29-30: VM Export & Campus Testing

**Export VM to External Disk**:
```bash
# On host PC (not in VM)
# Shut down VM first
VBoxManage list vms  # Get VM name

# Export to external disk
VBoxManage clonevm "inception-yom" \
  --name "inception-yom-export" \
  --register \
  --basefolder "/path/to/external/disk/VirtualBox VMs"

# Alternative: Export as OVA (more portable)
VBoxManage export "inception-yom" \
  -o "/path/to/external/disk/inception-yom.ova"
```

**Campus Testing Checklist**:
- [ ] Boot VM from external disk on campus PC
- [ ] Verify Docker services start
- [ ] Test all URLs (https://yom.42.fr, bonus services)
- [ ] Prepare for live modification during defense
- [ ] Time full rebuild: `make re` (should be < 5 minutes)

---

## 🎓 Defense Preparation

### Key Points to Explain

1. **Architecture**
   - 3-tier application (web, app, database)
   - Docker network isolation
   - Volume persistence strategy

2. **Security Measures**
   - TLS 1.2/1.3 only
   - Secrets management (Docker secrets)
   - Non-root containers
   - Security headers
   - No hardcoded credentials

3. **Docker Concepts**
   - PID 1 importance (no zombie processes)
   - Difference between CMD and ENTRYPOINT
   - Multi-stage builds (if implemented)
   - Health checks for orchestration

4. **Bonus Justification**
   - Redis: Performance (caching reduces DB load)
   - FTP: Content management convenience
   - Adminer: Database administration GUI
   - Static site: Portfolio demonstration
   - Portainer: Container management & monitoring

### Common Defense Questions

**Q: Why not use docker-compose up -d directly?**
A: Makefile provides abstraction, data directory setup, and easier operations.

**Q: Why Debian over Alpine?**
A: (Your choice - justify based on familiarity, package availability, or security)

**Q: How do containers communicate?**
A: Custom bridge network `inception_network`, DNS resolution by container name.

**Q: What happens if MariaDB crashes?**
A: `restart: unless-stopped` policy auto-restarts, health checks prevent WordPress from connecting too early.

**Q: Why separate volumes for DB and WordPress?**
A: Data persistence, backup strategies, different access patterns.

---

## 📊 Resource Requirements

### VM Specifications
- **CPU**: 2-4 cores
- **RAM**: 4-8 GB
- **Disk**: 30 GB (15 GB OS + 10 GB Docker + 5 GB buffer)
- **Network**: Bridged (for campus evaluation)

### External Disk Requirements
- **Size**: 50 GB minimum (for VM + export)
- **Speed**: USB 3.0+ recommended
- **Format**: NTFS/exFAT (cross-platform)

---

## ⚠️ Critical Success Factors

1. **No Shortcuts**: Every Dockerfile must be custom
2. **PID 1 Compliance**: Use exec in scripts, proper process management
3. **Secrets Security**: Never commit passwords to git
4. **Auto-Restart**: All services must have restart policies
5. **TLS Only**: NGINX must enforce TLS 1.2/1.3
6. **Domain Setup**: yom.42.fr must work in VM
7. **Volume Paths**: /home/yom/data (adjust to your VM login)
8. **Port 443 Only**: NGINX is sole entry point

---

## 🚀 Quick Start (After VM Setup)

```bash
# Clone or copy project structure
cd ~/inception

# Review .env and secrets
cat srcs/.env
ls -la secrets/

# Build everything
make

# Verify all containers running
make ps

# Access services
curl -k https://yom.42.fr
firefox https://yom.42.fr

# View logs
make logs

# Clean shutdown
make down
```

---

## 📞 Troubleshooting

### MariaDB won't start
```bash
# Check logs
docker logs mariadb

# Common fix: permissions
sudo chown -R 999:999 /home/yom/data/mariadb

# Reset database
make fclean && make
```

### WordPress installation loop
```bash
# Check MariaDB connection
docker exec wordpress ping -c 3 mariadb

# Verify secrets
docker exec wordpress cat /run/secrets/db_password
```

### NGINX 502 Bad Gateway
```bash
# Check WordPress PHP-FPM
docker exec wordpress ps aux | grep php-fpm

# Test PHP-FPM port
docker exec nginx nc -zv wordpress 9000
```

### SSL Certificate Issues
```bash
# Regenerate certificates
cd srcs/requirements/nginx/tools
./generate-certs.sh

# Rebuild NGINX
docker-compose build nginx
make up
```

---

## ✅ Final Checklist Before Defense

- [ ] VM boots from external disk
- [ ] All mandatory services running
- [ ] All 5 bonus services running
- [ ] Domain yom.42.fr resolves to localhost
- [ ] HTTPS works (self-signed warning is OK)
- [ ] WordPress accessible with 2 users
- [ ] Redis caching verified
- [ ] FTP server accessible
- [ ] Adminer connects to MariaDB
- [ ] Static site loads
- [ ] Portainer UI accessible
- [ ] No passwords in Dockerfiles
- [ ] .env and secrets/ in .gitignore
- [ ] All containers auto-restart on crash
- [ ] `make re` rebuilds everything successfully
- [ ] Architecture diagram ready
- [ ] Defense notes prepared

---

**Good luck! You've got this! 🎯**

Apply your Azure security knowledge (Days 35, 53, 55, 9) to make this the most secure Inception project in your cohort!
