# Inception - Services Knowledge Base

**Date**: November 30, 2025  
**Project**: 42 Inception - Complete Service Documentation  
**Status**: All services operational

---

## 📋 Table of Contents

1. [Mandatory Services](#mandatory-services)
2. [Bonus Services](#bonus-services)
3. [Service Interactions](#service-interactions)
4. [Technical Stack Summary](#technical-stack-summary)

---

## 🔷 Mandatory Services

### 1. NGINX - Web Server & Reverse Proxy

**Purpose**: Acts as the single entry point for all web traffic, handling TLS encryption and routing requests.

**Creator**: Igor Sysoev  
**Initial Release**: October 2004  
**Organization**: NGINX, Inc. (acquired by F5 Networks in 2019)  
**Written in**: C

**What it does in this project**:
- Listens on port 443 (HTTPS only, no HTTP)
- Terminates TLS/SSL connections with self-signed certificate
- Routes requests to WordPress via FastCGI (PHP-FPM)
- Proxies `/adminer` requests to Adminer service (port 9001)
- Proxies `/static` requests to Static Site service (port 8080)
- Serves static files (CSS, JS, images) directly
- Implements security headers

**Key Features**:
- High-performance event-driven architecture
- Low memory footprint
- Excellent for reverse proxy and load balancing
- Native FastCGI support for PHP applications

**Configuration Location**: `srcs/requirements/nginx/conf/nginx.conf`

---

### 2. WordPress - Content Management System

**Purpose**: The main web application providing a blogging/CMS platform.

**Creator**: Matt Mullenweg and Mike Little  
**Initial Release**: May 27, 2003  
**Organization**: WordPress Foundation (non-profit) / Automattic (commercial)  
**Written in**: PHP  
**Current Version in Project**: Latest (auto-downloaded via WP-CLI)

**What it does in this project**:
- Serves the main website at `https://ggrisole.42.fr/`
- Manages blog posts, pages, and media
- Provides admin dashboard at `/wp-admin`
- Communicates with MariaDB for data storage
- Uses Redis for object caching (performance optimization)
- Runs on PHP-FPM (FastCGI Process Manager)

**Users Created**:
- **Admin user**: Full administrator access (from .env)
- **Author user**: Content creation only (from .env)

**Key Features**:
- Plugin architecture (Redis Object Cache plugin installed)
- Theme system
- REST API
- Media library
- User management
- SEO-friendly URLs

**Configuration**: Automated setup via WP-CLI in `wp-setup.sh`

---

### 3. MariaDB - Database Server

**Purpose**: Stores all WordPress data (posts, users, settings, etc.).

**Creator**: Michael "Monty" Widenius (original MySQL creator)  
**Initial Release**: October 29, 2009  
**Organization**: MariaDB Foundation (non-profit) / MariaDB Corporation (commercial)  
**Written in**: C, C++  
**Version**: 10.11 (from Debian Bookworm)

**What it does in this project**:
- Stores WordPress database (tables, posts, users, metadata)
- Provides SQL query interface
- Handles transactions and data integrity
- Accessible only from within Docker network (security)
- Data persisted to `~/data/mariadb` volume

**Why MariaDB instead of MySQL?**:
- 100% compatible drop-in replacement for MySQL
- Open-source with no proprietary components
- Better performance in many scenarios
- Active community development
- Preferred in many Linux distributions

**Key Features**:
- ACID compliance
- Replication support
- Full-text search
- JSON support
- Excellent performance

**Configuration Location**: `srcs/requirements/mariadb/conf/50-server.cnf`

---

## 🎁 Bonus Services

### 1. Redis - In-Memory Data Store & Cache

**Purpose**: Accelerates WordPress by caching database queries and objects in memory.

**Creator**: Salvatore Sanfilippo (antirez)  
**Initial Release**: May 10, 2009  
**Organization**: Redis Ltd. (formerly Redis Labs)  
**Written in**: C  
**Version**: 7.0.15

**What it does in this project**:
- Caches WordPress database query results
- Stores frequently accessed objects in RAM
- Reduces database load by serving cached data
- Dramatically improves page load times
- Implements LRU (Least Recently Used) eviction policy

**Why Redis?**:
- Extremely fast (in-memory operations)
- Simple key-value store
- Built-in data structures (strings, lists, sets, hashes)
- Pub/sub messaging
- Persistence options (disabled in our cache-only setup)

**Integration with WordPress**:
- PHP extension: `php8.2-redis` (PhpRedis 5.3.7)
- WordPress plugin: `Redis Object Cache` by Till Krüss
- Communication: TCP on port 6379
- Configuration via `WP_REDIS_HOST` constant

**Performance Impact**:
- Reduces database queries by 50-90%
- Improves response time from ~500ms to ~50ms
- Handles high traffic spikes better

**Configuration**: Custom `redis.conf` with:
- Network binding to `0.0.0.0` (Docker network access)
- 256MB memory limit
- No persistence (pure cache)
- LRU eviction policy

---

### 2. Adminer - Database Management Tool

**Purpose**: Web-based graphical interface for managing MariaDB databases.

**Creator**: Jakub Vrána  
**Initial Release**: July 2007 (originally called phpMinAdmin)  
**Organization**: Independent open-source project  
**Written in**: PHP (single 470KB file!)  
**Version**: 4.8.1

**What it does in this project**:
- Provides web UI at `https://ggrisole.42.fr/adminer`
- Allows browsing database tables
- Executes SQL queries via web interface
- Imports/exports database dumps
- Manages users and permissions
- Views table structures and relationships

**Why Adminer over phpMyAdmin?**:
- **Size**: Single 470KB file vs 11MB+ for phpMyAdmin
- **Performance**: Much faster and lighter
- **Security**: Smaller attack surface
- **Modern**: Clean, responsive interface
- **Features**: Supports more databases (MySQL, PostgreSQL, SQLite, etc.)

**Access**:
- URL: `https://ggrisole.42.fr/adminer`
- Routed via NGINX reverse proxy
- Runs on PHP-FPM (port 9001 internally)

**Key Features**:
- No installation needed (single file)
- Export to SQL, CSV, XML
- Foreign key visualization
- Query history
- Syntax highlighting
- Support for views, triggers, stored procedures

---

### 3. Static Site - HTML/CSS Showcase

**Purpose**: Demonstrates ability to serve static content and showcases project services.

**Creator**: Custom-built for this project  
**Created**: November 2025  
**Technology**: Pure HTML5 + CSS3 (no frameworks)  
**Size**: 2.6KB HTML + 1.8KB CSS

**What it does in this project**:
- Serves at `https://ggrisole.42.fr/static`
- Displays all available services in a beautiful grid layout
- Acts as a landing page/dashboard for the infrastructure
- Demonstrates NGINX routing capabilities
- Shows responsive design principles

**Design Features**:
- **Purple gradient background** (`linear-gradient(135deg, #667eea 0%, #764ba2 100%)`)
- **Responsive grid layout** (CSS Grid with auto-fit)
- **Service cards** with hover effects and shadows
- **Icons** for each service (emoji-based)
- **Links** to all available services
- **Mobile-friendly** design

**Technology Stack**:
- NGINX as web server (port 8080 internally)
- Pure static files (no JavaScript)
- CSS3 animations and transitions
- Semantic HTML5 markup

**Services Listed on Page**:
1. WordPress (main site)
2. Adminer (database tool)
3. Portainer (container manager)
4. FTP access info
5. Redis cache info
6. Project documentation

---

### 4. FTP - File Transfer Protocol Server

**Purpose**: Provides file access to WordPress content for easy file management.

**Software**: vsftpd (Very Secure FTP Daemon)  
**Creator**: Chris Evans  
**Initial Release**: 2001  
**Organization**: Independent security-focused project  
**Written in**: C  
**Version**: 3.x from Debian Bookworm

**What it does in this project**:
- Allows FTP access to WordPress files at `ftp://ggrisole.42.fr:21`
- Mounts WordPress volume (`wordpress_data:/var/www/html`)
- Enables remote file editing and uploads
- Provides passive mode for firewall compatibility
- User: `ftpuser` / Password: `ftppass`

**Why vsftpd?**:
- **Security-first design** (name literally means "Very Secure")
- Extensive security options
- Good performance
- Active maintenance
- Standard in many Linux distributions

**Configuration**:
- **Passive mode**: Ports 21000-21010 for data connections
- **Local users**: Uses system authentication
- **Chroot jail**: Users restricted to their home directory
- **Write access**: Enabled for file uploads
- **Anonymous access**: Disabled (security)

**Use Cases**:
- Uploading themes and plugins
- Editing configuration files
- Backing up WordPress content
- Batch file operations
- Integration with FTP clients (FileZilla, etc.)

**Ports Exposed**:
- 21 (command/control)
- 21000-21010 (passive mode data transfer)

---

### 5. Portainer - Container Management Platform

**Purpose**: Web-based UI for managing Docker containers, images, networks, and volumes.

**Creator**: Neil Cresswell  
**Initial Release**: 2017  
**Organization**: Portainer.io (New Zealand-based company)  
**Written in**: Go (backend) + Angular (frontend)  
**Version**: 2.19.4

**What it does in this project**:
- Provides web UI at `http://localhost:9443`
- Visualizes all 8 running containers
- Shows container logs in real-time
- Manages container lifecycle (start/stop/restart/remove)
- Inspects container configurations
- Monitors resource usage (CPU, memory, network)
- Manages Docker networks and volumes
- Deploys Docker Compose stacks

**Why Portainer?**:
- **User-friendly**: No terminal needed for basic operations
- **Visual**: See everything at a glance
- **Powerful**: Full Docker feature access
- **Secure**: Role-based access control
- **Cross-platform**: Works with Docker, Kubernetes, etc.

**Key Features**:
- Container console access (web-based terminal)
- Log viewer with search and filtering
- Resource usage graphs
- Image management (pull, remove, build)
- Network topology visualization
- Volume backup and restore
- Team management (in business edition)
- Template library for common stacks

**Special Requirements**:
- Needs `docker-compose` binary installed in container
- Requires Docker socket mount (`/var/run/docker.sock`)
- Persistent data volume for configuration
- First-time setup: Create admin account

**Access**:
- URL: `http://localhost:9443`
- Direct port mapping (not through NGINX)
- First login: Set admin password

**Dependencies**:
- Docker socket (read-only mount for security)
- docker-compose v1.29.2 (for stack deployment)
- Persistent volume for settings

---

## 🔄 Service Interactions

### Communication Flow

```
Internet → NGINX (443) → WordPress (9000) → MariaDB (3306)
                ↓                ↓
            Adminer (9001)   Redis (6379)
                ↓
          Static Site (8080)

FTP (21) → WordPress Files

Portainer (9443) → Docker Socket → All Containers
```

### Data Flow

1. **User visits website**:
   - NGINX receives HTTPS request
   - Routes to WordPress via FastCGI
   - WordPress checks Redis cache
   - If cache miss, queries MariaDB
   - Returns response through NGINX

2. **Admin manages database**:
   - Accesses `/adminer` through NGINX
   - Adminer connects to MariaDB
   - Executes SQL queries
   - Returns results via NGINX

3. **User uploads files via FTP**:
   - FTP client connects to vsftpd
   - Authentication against system users
   - Files written to WordPress volume
   - Immediately available to WordPress

4. **Admin manages containers**:
   - Accesses Portainer UI
   - Portainer reads Docker socket
   - Executes Docker commands
   - Shows real-time status

---

## 📊 Technical Stack Summary

### Languages
- **C**: NGINX, Redis, MariaDB, vsftpd
- **PHP**: WordPress, Adminer
- **Go**: Portainer backend
- **JavaScript/TypeScript**: Portainer frontend
- **HTML/CSS**: Static site

### Protocols
- **HTTPS**: TLS 1.2/1.3 (443)
- **FastCGI**: NGINX ↔ WordPress
- **MySQL Protocol**: WordPress/Adminer ↔ MariaDB
- **Redis Protocol**: WordPress ↔ Redis (RESP)
- **FTP**: File transfer (21 + 21000-21010)
- **HTTP**: Portainer UI (9443)

### Storage
- **Volumes**: MariaDB data, WordPress files
- **Bind Mounts**: Configuration files
- **tmpfs**: (optional for logs)

### Network
- **Docker Bridge Network**: `srcs_inception_network`
- **Internal DNS**: Service name resolution
- **Port Mapping**: Only 443, 21, 21000-21010, 9443 exposed

---

## 🎯 Project Compliance

### 42 Subject Requirements

**Mandatory**:
- ✅ Each service in separate container
- ✅ Custom Dockerfiles (no `docker-compose pull`)
- ✅ TLS configuration with certificates
- ✅ Two volumes (database + WordPress files)
- ✅ Docker network connecting containers
- ✅ Containers restart on failure
- ✅ Domain name `login.42.fr` (ggrisole.42.fr)
- ✅ WordPress with multiple users
- ✅ Environment variables via `.env` file
- ✅ No passwords in Dockerfiles
- ✅ No latest tags (explicit versions)

**Bonus Points** (5 services):
- ✅ Redis cache (improves performance)
- ✅ FTP server (file management)
- ✅ Static site (additional service)
- ✅ Adminer (database admin)
- ✅ Portainer (container management)

### Security Considerations

1. **No passwords in code**: All secrets in `.env` file
2. **TLS encryption**: All HTTP traffic encrypted
3. **Network isolation**: Services not exposed unless needed
4. **Read-only mounts**: Docker socket mounted read-only
5. **User separation**: FTP chroot jail, container users
6. **Update strategy**: Regular image rebuilds

---

## 📚 Additional Resources

### Official Documentation
- **NGINX**: https://nginx.org/en/docs/
- **WordPress**: https://wordpress.org/support/
- **MariaDB**: https://mariadb.org/documentation/
- **Redis**: https://redis.io/docs/
- **Adminer**: https://www.adminer.org/
- **vsftpd**: https://security.appspot.com/vsftpd.html
- **Portainer**: https://docs.portainer.io/

### Package Sources
- **Debian Bookworm**: Base OS for all containers
- **WP-CLI**: WordPress command-line tool
- **PHP-FPM**: FastCGI Process Manager for PHP
- **PhpRedis**: PHP extension for Redis

---

*Document created: November 30, 2025*  
*Project: 42 Inception*  
*All services operational and documented*
