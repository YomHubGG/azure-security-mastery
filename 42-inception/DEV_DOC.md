# Inception - Developer Documentation

This document provides technical details for developers working on or extending the Inception infrastructure.

## Table of Contents
- [Architecture Overview](#architecture-overview)
- [Prerequisites](#prerequisites)
- [Development Setup](#development-setup)
- [Project Structure](#project-structure)
- [Service Details](#service-details)
- [Docker Compose Configuration](#docker-compose-configuration)
- [Makefile Commands](#makefile-commands)
- [Data Persistence](#data-persistence)
- [Networking](#networking)
- [Security Implementation](#security-implementation)
- [Extending the Project](#extending-the-project)

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│                   Host System                        │
│  ┌───────────────────────────────────────────────┐  │
│  │         Docker Bridge Network                 │  │
│  │         (inception_network)                   │  │
│  │                                               │  │
│  │  ┌──────┐  ┌──────────┐  ┌────────┐         │  │
│  │  │NGINX │──│WordPress │──│MariaDB │         │  │
│  │  │:443  │  │php-fpm   │  │:3306   │         │  │
│  │  └──┬───┘  └────┬─────┘  └───┬────┘         │  │
│  │     │           │            │               │  │
│  │     │      ┌────┴────┐       │               │  │
│  │     │      │ Redis   │       │               │  │
│  │     │      │ :6379   │       │               │  │
│  │     │      └─────────┘       │               │  │
│  │     │                        │               │  │
│  │  ┌──┴────┐  ┌─────────┐  ┌──┴─────┐         │  │
│  │  │Adminer│  │  FTP    │  │Static  │         │  │
│  │  │:8080  │  │ :21     │  │Site    │         │  │
│  │  └───────┘  └─────────┘  └────────┘         │  │
│  └───────────────────────────────────────────────┘  │
│                                                     │
│  Volumes:                                           │
│  /home/ggrisole/data/mariadb  → MariaDB data       │
│  /home/ggrisole/data/wordpress → WordPress files   │
└─────────────────────────────────────────────────────┘
```

## Prerequisites

### Software Requirements
- Docker Engine 20.10+
- Docker Compose 2.0+
- Make
- Git
- Text editor (vim, nano, VS Code)

### System Requirements
- Linux/Unix OS (Debian Bookworm recommended)
- 2GB RAM minimum (4GB recommended)
- 10GB free disk space
- sudo/root access for initial setup

### Knowledge Requirements
- Docker & containerization concepts
- Docker Compose syntax
- Basic networking (ports, protocols)
- Shell scripting (bash)
- Web server configuration (NGINX)
- PHP-FPM configuration
- SQL/MariaDB basics

## Development Setup

### 1. Clone Repository
```bash
git clone <repository-url>
cd 42-inception
```

### 2. Create Secrets Directory
```bash
mkdir -p secrets
echo "your_secure_root_password" > secrets/db_root_password.txt
echo "your_secure_db_password" > secrets/db_password.txt
echo "your_secure_admin_password" > secrets/wp_admin_password.txt
chmod 600 secrets/*.txt
```

### 3. Configure Environment
```bash
cd srcs
cp .env.example .env
# Edit .env with your configuration
vim .env
```

### 4. Create Data Directories
```bash
sudo mkdir -p /home/ggrisole/data/{mariadb,wordpress}
sudo chown -R $USER:$USER /home/ggrisole/data
```

### 5. Add Domain to Hosts
```bash
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts
```

### 6. Build and Start
```bash
make
```

## Project Structure

```
42-inception/
├── Makefile                    # Build automation
├── README.md                   # Project overview
├── USER_DOC.md                 # End-user documentation
├── DEV_DOC.md                  # This file
├── secrets/                    # Secret files (gitignored)
│   ├── db_root_password.txt
│   ├── db_password.txt
│   └── wp_admin_password.txt
└── srcs/
    ├── docker-compose.yml      # Service orchestration
    ├── .env                    # Environment variables
    └── requirements/
        ├── mariadb/
        │   ├── Dockerfile
        │   ├── conf/
        │   │   └── my.cnf
        │   └── tools/
        │       └── init-db.sh
        ├── nginx/
        │   ├── Dockerfile
        │   ├── conf/
        │   │   └── nginx.conf
        │   └── tools/
        │       └── generate-cert.sh
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── conf/
        │   │   └── www.conf
        │   └── tools/
        │       └── wp-setup.sh
        └── bonus/
            ├── redis/
            ├── adminer/
            ├── ftp/
            ├── static-site/
            └── portainer/
```

## Service Details

### NGINX (Port 443)

**Purpose**: Reverse proxy with TLS termination

**Base Image**: `debian:bookworm`

**Configuration**:
- TLSv1.2 and TLSv1.3 only
- Self-signed SSL certificate
- Proxy pass to WordPress (FastCGI)
- Virtual hosts for bonus services

**Key Files**:
- `conf/nginx.conf` - Main configuration
- `tools/generate-cert.sh` - SSL certificate generation

**Environment Variables**: None

**Exposed Ports**: 443 (HTTPS)

### WordPress (Port 9000)

**Purpose**: Content management system

**Base Image**: `debian:bookworm`

**Configuration**:
- PHP-FPM 8.2
- WP-CLI for automation
- Redis Object Cache plugin
- Connects to MariaDB and Redis

**Key Files**:
- `conf/www.conf` - PHP-FPM pool configuration
- `tools/wp-setup.sh` - WordPress installation script

**Environment Variables**:
```
MYSQL_DATABASE        # Database name
MYSQL_USER           # Database user
WP_URL               # Site URL (must match NGINX)
WP_TITLE             # Site title
WP_ADMIN_USER        # Admin username (no 'admin')
WP_ADMIN_EMAIL       # Admin email
WP_USER              # Second user name
WP_USER_EMAIL        # Second user email
WP_USER_PASSWORD     # Second user password
```

**Secrets**:
- `/run/secrets/db_password` - Database password
- `/run/secrets/wp_admin_password` - Admin password

**Volumes**: `/var/www/html` (WordPress files)

**Dependencies**: mariadb, redis

### MariaDB (Port 3306)

**Purpose**: MySQL-compatible database server

**Base Image**: `debian:bookworm`

**Configuration**:
- Binds to all interfaces (0.0.0.0)
- InnoDB storage engine
- Character set: utf8mb4
- Collation: utf8mb4_unicode_ci

**Key Files**:
- `conf/my.cnf` - MariaDB server configuration
- `tools/init-db.sh` - Database initialization script

**Environment Variables**:
```
MYSQL_DATABASE           # Database to create
MYSQL_USER              # Non-root user
```

**Secrets**:
- `/run/secrets/db_root_password` - Root password
- `/run/secrets/db_password` - User password

**Volumes**: `/var/lib/mysql` (Database files)

**Health Check**: `mysqladmin ping`

### Redis (Port 6379)

**Purpose**: Caching layer for WordPress

**Base Image**: `alpine:3.19`

**Configuration**:
- Persistence enabled (AOF)
- Max memory policy: allkeys-lru
- No authentication (internal network only)

**Volumes**: None (cache can be ephemeral)

**Health Check**: `redis-cli ping`

### Adminer (Port 8080)

**Purpose**: Web-based database management

**Base Image**: `alpine:3.19`

**Configuration**:
- PHP 8.2
- Connects to MariaDB service

**Accessed via**: NGINX reverse proxy at `/adminer`

### FTP (Port 21, 21000-21010)

**Purpose**: File transfer to WordPress volume

**Base Image**: `alpine:3.19`

**Configuration**:
- vsftpd server
- Passive mode ports: 21000-21010
- User: ftpuser
- Home: `/var/www/html`

**Volumes**: Shared with WordPress

### Static Site (Port 80)

**Purpose**: Custom static website showcase

**Base Image**: `alpine:3.19`

**Configuration**:
- NGINX web server
- HTML/CSS/JS files
- No backend processing

**Accessed via**: NGINX reverse proxy at `/portfolio`

### Portainer (Port 9000)

**Purpose**: Docker container management UI

**Base Image**: `alpine:3.19`

**Configuration**:
- Monitors Docker socket
- Web interface for container management

**Volumes**:
- `/var/run/docker.sock` (read-only)
- `portainer_data` (persistent config)

**Accessed via**: https://localhost:9443

## Docker Compose Configuration

### Networks

```yaml
networks:
  inception_network:
    driver: bridge
```

All services connect to this custom bridge network, enabling service discovery by container name.

### Volumes

```yaml
volumes:
  mariadb_data:
    driver: local
    driver_opts:
      type: none
      o: bind
      device: /home/ggrisole/data/mariadb
  wordpress_data:
    driver: local
    driver_opts:
      type: none
      o: bind
      device: /home/ggrisole/data/wordpress
  portainer_data:
    driver: local
```

**Important**: Bind mounts to `/home/<login>/data/` are required by the evaluation criteria.

### Secrets

```yaml
secrets:
  db_root_password:
    file: ../secrets/db_root_password.txt
  db_password:
    file: ../secrets/db_password.txt
  wp_admin_password:
    file: ../secrets/wp_admin_password.txt
```

Secrets are mounted as files in `/run/secrets/` inside containers.

### Restart Policy

All services use `restart: unless-stopped` to ensure availability after crashes or reboots.

### Health Checks

Critical services (MariaDB, Redis) have health checks to ensure dependent services start in correct order.

## Makefile Commands

### Available Targets

```makefile
make            # Build and start all services
make up         # Start services (alias for make)
make down       # Stop services (preserves data)
make restart    # Restart all services
make logs       # Follow logs from all services
make clean      # Stop and remove containers
make fclean     # Clean + remove volumes and images
make re         # fclean + rebuild everything
make ps         # Show container status
make help       # Show available commands
```

### Implementation Details

```makefile
COMPOSE = docker compose -f srcs/docker-compose.yml

up:
    $(COMPOSE) up -d --build

down:
    $(COMPOSE) down

fclean: down
    docker system prune -af --volumes
    sudo rm -rf /home/ggrisole/data/*
```

## Data Persistence

### How It Works

1. **Bind Mounts**: Volumes are mounted from host directories to containers
2. **Initialization**: Scripts check if data exists before initializing
3. **Persistence**: Data survives container restarts and rebuilds
4. **Permissions**: Containers run as specific users (mysql, www-data)

### MariaDB Persistence

```bash
# Data location on host
/home/ggrisole/data/mariadb/

# Mounted in container
/var/lib/mysql/

# Initialization check
if [ ! -d "/var/lib/mysql/mysql" ]; then
    # Initialize database
fi
```

### WordPress Persistence

```bash
# Data location on host
/home/ggrisole/data/wordpress/

# Mounted in container
/var/www/html/

# Initialization check
if [ ! -f /var/www/html/wp-config.php ]; then
    # Install WordPress
fi
```

## Networking

### Service Discovery

Services communicate using container names as hostnames:

```php
// WordPress connecting to MariaDB
define('DB_HOST', 'mariadb:3306');

// WordPress connecting to Redis
define('WP_REDIS_HOST', 'redis');
```

### Port Mapping

Only NGINX exposes ports to the host:
- Host port 443 → Container port 443

All other services are accessible only within the Docker network.

### DNS Resolution

Docker's embedded DNS server resolves:
- `mariadb` → MariaDB container IP
- `wordpress` → WordPress container IP
- `redis` → Redis container IP
- etc.

## Security Implementation

### Secret Management

✅ **Implemented**:
- Secrets in separate files (not in Git)
- Mounted via Docker secrets
- Environment variables reference secrets
- File permissions restricted (600)

❌ **Not Implemented** (for production):
- Secrets rotation
- Encrypted secrets at rest
- Secret vault integration

### Container Security

✅ **Implemented**:
- Custom Dockerfiles (no DockerHub images)
- Minimal base images (Debian Bookworm)
- Non-root users where possible
- Health checks for availability
- Isolated network

❌ **Not Implemented** (for production):
- Read-only root filesystem
- Capability dropping
- AppArmor/SELinux profiles
- Resource limits (CPU/memory)
- Security scanning (Trivy)

### TLS/SSL

✅ **Implemented**:
- TLSv1.2 and TLSv1.3 only
- Self-signed certificate for development
- HTTPS-only access (no HTTP)

❌ **Not Implemented** (for production):
- Valid CA-signed certificate
- Certificate renewal automation
- HSTS headers
- Strong cipher suite configuration

## Extending the Project

### Adding a New Service

1. **Create Service Directory**:
```bash
mkdir -p srcs/requirements/bonus/myservice/{conf,tools}
```

2. **Create Dockerfile**:
```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y mypackage \
    && rm -rf /var/lib/apt/lists/*

COPY conf/myconfig.conf /etc/myservice/
COPY tools/init.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/init.sh

EXPOSE 8080

ENTRYPOINT ["/usr/local/bin/init.sh"]
CMD ["myservice"]
```

3. **Add to docker-compose.yml**:
```yaml
  myservice:
    container_name: myservice
    build:
      context: ./requirements/bonus/myservice
      dockerfile: Dockerfile
    image: myservice:inception
    networks:
      - inception_network
    restart: unless-stopped
```

4. **Update NGINX** (if web accessible):
```nginx
location /myservice/ {
    proxy_pass http://myservice:8080/;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
}
```

5. **Rebuild**:
```bash
make re
```

### Modifying Existing Services

1. **Edit Configuration**: Modify files in `conf/` or `tools/`
2. **Rebuild Service**: `docker compose -f srcs/docker-compose.yml build <service>`
3. **Restart Service**: `docker compose -f srcs/docker-compose.yml up -d <service>`

### Testing Changes

```bash
# Test specific service
docker compose -f srcs/docker-compose.yml logs -f wordpress

# Enter container for debugging
docker exec -it wordpress bash

# Check network connectivity
docker exec wordpress ping mariadb
docker exec wordpress nc -zv mariadb 3306

# Verify WordPress configuration
docker exec wordpress wp config list --allow-root

# Check database
docker exec mariadb mysql -uroot -p$(cat secrets/db_root_password.txt) -e "SHOW DATABASES;"
```

## Common Development Tasks

### Changing WordPress URL

1. Update `.env`:
```bash
WP_URL=https://newdomain.42.fr
```

2. Update `/etc/hosts`:
```bash
127.0.0.1 newdomain.42.fr
```

3. Rebuild:
```bash
make restart
```

WordPress startup script automatically updates URLs in database.

### Resetting Database

```bash
# Stop services
make down

# Remove database files
sudo rm -rf /home/ggrisole/data/mariadb/*

# Restart (will reinitialize)
make up
```

### Debugging Container Startup Issues

```bash
# View build output
docker compose -f srcs/docker-compose.yml build --no-cache mariadb

# Run container interactively
docker run -it --entrypoint /bin/bash mariadb:inception

# Check initialization script
docker compose -f srcs/docker-compose.yml logs mariadb
```

### Performance Optimization

```bash
# Check Redis cache hit rate
docker exec wordpress wp redis status --allow-root

# Monitor database queries
docker exec mariadb mysql -uroot -p$(cat secrets/db_root_password.txt) -e "SHOW PROCESSLIST;"

# Check resource usage
docker stats
```

## Troubleshooting Development Issues

### Build Failures

**Issue**: Docker build fails with package errors

**Solution**:
```bash
# Update package lists
docker compose -f srcs/docker-compose.yml build --no-cache --pull
```

### Permission Errors

**Issue**: Cannot write to volumes

**Solution**:
```bash
# Fix ownership
sudo chown -R $USER:$USER /home/ggrisole/data

# Check container user
docker exec wordpress id
```

### Network Connectivity

**Issue**: Services can't communicate

**Solution**:
```bash
# Verify network exists
docker network ls | grep inception

# Check service network membership
docker network inspect srcs_inception_network

# Test connectivity
docker exec wordpress ping mariadb
```

### WordPress Installation Loop

**Issue**: WordPress keeps showing installation page

**Solution**:
```bash
# Check if wp-config.php exists
docker exec wordpress ls -la /var/www/html/wp-config.php

# Verify database connection
docker exec wordpress wp db check --allow-root

# Check URL configuration
docker exec wordpress wp option get siteurl --allow-root
```

## Best Practices

### Development Workflow

1. Make changes in feature branch
2. Test locally with `make re`
3. Verify all services start correctly
4. Test functionality manually
5. Check logs for errors
6. Commit changes
7. Create pull request

### Code Quality

- Comment complex configurations
- Use meaningful variable names
- Follow shell script best practices (shellcheck)
- Document assumptions
- Handle errors gracefully

### Version Control

- Never commit `.env` files
- Never commit secrets
- Keep `.gitignore` updated
- Document manual setup steps
- Tag releases

## References

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Reference](https://docs.docker.com/compose/compose-file/)
- [NGINX Documentation](https://nginx.org/en/docs/)
- [WordPress Developer Resources](https://developer.wordpress.org/)
- [WP-CLI Documentation](https://wp-cli.org/)
- [MariaDB Knowledge Base](https://mariadb.com/kb/en/)
- [Redis Documentation](https://redis.io/documentation)
- [42 Inception Subject](https://cdn.intra.42.fr/pdf/pdf/191261/en.subject.pdf)
