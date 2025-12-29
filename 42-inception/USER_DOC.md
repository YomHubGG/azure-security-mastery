# Inception - User Documentation

This document provides instructions for end users and system administrators to operate the Inception infrastructure.

## Table of Contents
- [Quick Start](#quick-start)
- [Accessing Services](#accessing-services)
- [Managing the Stack](#managing-the-stack)
- [User Credentials](#user-credentials)
- [Troubleshooting](#troubleshooting)

## Quick Start

### Prerequisites
- Linux/Unix system (tested on Debian/Ubuntu)
- Docker Engine 20.10+
- Docker Compose 2.0+
- Minimum 2GB free disk space
- Port 443 available

### Initial Setup

1. **Add domain to hosts file**:
   ```bash
   echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts
   ```

2. **Create data directories**:
   ```bash
   sudo mkdir -p /home/ggrisole/data/{mariadb,wordpress}
   sudo chown -R $USER:$USER /home/ggrisole/data
   ```

3. **Start the infrastructure**:
   ```bash
   cd 42-inception
   make
   ```

   This command will:
   - Build all Docker images
   - Create necessary networks
   - Initialize databases
   - Start all services
   - Configure WordPress

   First launch takes 2-5 minutes.

## Accessing Services

### WordPress Website
- **URL**: https://ggrisole.42.fr
- **Note**: You'll see a security warning (self-signed certificate). Click "Advanced" → "Accept Risk and Continue"

### WordPress Admin Dashboard
- **URL**: https://ggrisole.42.fr/wp-admin
- **Username**: `ggrisole`
- **Password**: Stored in `secrets/wp_admin_password.txt`

### Adminer (Database Management)
- **URL**: https://ggrisole.42.fr/adminer
- **System**: MySQL
- **Server**: `mariadb`
- **Username**: `wpuser`
- **Password**: Stored in `secrets/db_password.txt`
- **Database**: `wordpress`

### FTP Server
- **Host**: `localhost`
- **Port**: `21`
- **Username**: `ftpuser`
- **Password**: `ftppass123`
- **Directory**: WordPress files (`/var/www/html`)

### Static Website
- **URL**: https://ggrisole.42.fr/portfolio

### Portainer (Container Management)
- **URL**: https://localhost:9443
- Create admin account on first access

## Managing the Stack

### Starting Services
```bash
make
# or
make up
```

### Stopping Services
```bash
make down
```
This stops all containers but preserves data.

### Restarting Services
```bash
make restart
```

### Viewing Logs
```bash
# All services
make logs

# Specific service
docker compose -f srcs/docker-compose.yml logs wordpress
docker compose -f srcs/docker-compose.yml logs mariadb
docker compose -f srcs/docker-compose.yml logs nginx
```

### Checking Service Status
```bash
docker compose -f srcs/docker-compose.yml ps
```

Expected output:
```
NAME         STATUS    PORTS
nginx        Up        0.0.0.0:443->443/tcp
wordpress    Up        9000/tcp
mariadb      Up        3306/tcp
redis        Up        6379/tcp
adminer      Up        8080/tcp
ftp          Up        21/tcp, 21000-21010/tcp
static-site  Up        80/tcp
portainer    Up        0.0.0.0:9443->9000/tcp
```

### Complete Cleanup (⚠️ Destroys All Data)
```bash
make fclean
```
This removes:
- All containers
- All images
- All volumes (DATABASE AND FILES WILL BE DELETED)
- All networks

## User Credentials

### WordPress Users

**Administrator**:
- Username: `ggrisole`
- Email: `yom@student.42.fr`
- Role: Administrator
- Password: See `secrets/wp_admin_password.txt`

**Author**:
- Username: `ggrisoleuser`
- Email: `ggrisoleuser@student.42.fr`
- Role: Author
- Password: `Author123`

### Database Access

**MariaDB Root**:
- Username: `root`
- Password: See `secrets/db_root_password.txt`
- Access: Only from within mariadb container

**WordPress Database User**:
- Username: `wpuser`
- Database: `wordpress`
- Password: See `secrets/db_password.txt`
- Access: From wordpress and adminer containers

### FTP Access
- Username: `ftpuser`
- Password: `ftppass123`
- Home: `/var/www/html`

## Basic Checks

### Verify All Services Are Running
```bash
docker ps
```
You should see 8 containers in "Up" status.

### Test WordPress Database Connection
```bash
docker exec wordpress wp db check --allow-root
```
Should output: "Success: Database connection is working."

### Test Redis Cache
```bash
docker exec redis redis-cli ping
```
Should output: "PONG"

### Check WordPress Cache Status
```bash
docker exec wordpress wp redis status --allow-root
```

## Troubleshooting

### Cannot Access Website

**Symptom**: Browser shows "This site can't be reached"

**Solutions**:
1. Check domain is in `/etc/hosts`:
   ```bash
   grep ggrisole.42.fr /etc/hosts
   ```

2. Check nginx is running:
   ```bash
   docker ps | grep nginx
   ```

3. Check nginx logs:
   ```bash
   docker logs nginx
   ```

### WordPress Shows "Error Establishing Database Connection"

**Solutions**:
1. Check MariaDB is healthy:
   ```bash
   docker exec mariadb mysqladmin ping
   ```

2. Verify credentials in `.env` file match secrets

3. Restart services:
   ```bash
   make restart
   ```

### SSL Certificate Warning

**Expected behavior**: Self-signed certificates trigger security warnings.

**Action**: Accept the security exception in your browser.

### FTP Connection Refused

**Solutions**:
1. Check FTP container is running:
   ```bash
   docker ps | grep ftp
   ```

2. Use passive mode in your FTP client

3. Check firewall allows ports 21 and 21000-21010

### Data Persistence After Reboot

**Verification**:
1. Reboot the system
2. Run `make` again
3. Access WordPress - your content should still be there

**If data is lost**:
- Check volume paths in `docker-compose.yml`
- Ensure `/home/ggrisole/data/` directories have correct permissions

## Getting Help

### View Available Make Commands
```bash
make help
```

### Container Shell Access
```bash
# WordPress
docker exec -it wordpress bash

# MariaDB
docker exec -it mariadb bash

# NGINX
docker exec -it nginx sh
```

### Inspect Volumes
```bash
# List volumes
docker volume ls

# Inspect volume
docker volume inspect srcs_wordpress_data
docker volume inspect srcs_mariadb_data
```

### Check Disk Usage
```bash
docker system df
```

## Maintenance

### Backup Data
```bash
# Create backup directory
mkdir -p ~/inception-backups/$(date +%Y%m%d)

# Backup database
docker exec mariadb mysqldump -uroot -p$(cat secrets/db_root_password.txt) wordpress > ~/inception-backups/$(date +%Y%m%d)/wordpress.sql

# Backup WordPress files
tar -czf ~/inception-backups/$(date +%Y%m%d)/wordpress-files.tar.gz /home/ggrisole/data/wordpress
```

### Restore Data
```bash
# Restore database
docker exec -i mariadb mysql -uroot -p$(cat secrets/db_root_password.txt) wordpress < ~/inception-backups/20251229/wordpress.sql

# Restore files
tar -xzf ~/inception-backups/20251229/wordpress-files.tar.gz -C /
```

### Update WordPress
WordPress updates are handled through the admin dashboard:
1. Login to https://ggrisole.42.fr/wp-admin
2. Navigate to Dashboard → Updates
3. Click "Update Now"

## Security Notes

⚠️ **This is a development/educational setup**. For production:
- Use valid SSL certificates (Let's Encrypt)
- Change all default passwords
- Implement proper firewall rules
- Regular security updates
- Proper backup strategy
- Enable WordPress security plugins
- Restrict database access
- Use strong FTP credentials or disable FTP
