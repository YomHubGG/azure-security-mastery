# Inception Project - Commands Reference

**Updated**: November 28, 2025  
**VM**: inception (VirtualBox, Debian 12 Bookworm)
**42 Login**: ggrisole
**Domain**: ggrisole.42.fr

---

## 1. VM Creation & Setup

### VirtualBox VM Creation
```bash
# Create VM on external drive
VBoxManage createvm --name "inception" --ostype Debian_64 --register \
    --basefolder /mnt/vmlab/inception

# Configure VM resources
VBoxManage modifyvm inception --memory 4096 --cpus 2 --vram 16

# Create and attach disk
VBoxManage createmedium disk --filename /mnt/vmlab/inception/inception.vdi \
    --size 30720 --format VDI
VBoxManage storagectl inception --name "SATA Controller" --add sata
VBoxManage storageattach inception --storagectl "SATA Controller" --port 0 \
    --device 0 --type hdd --medium /mnt/vmlab/inception/inception.vdi

# Attach ISO
VBoxManage storageattach inception --storagectl "SATA Controller" --port 1 \
    --device 0 --type dvddrive --medium /path/to/debian-12.8.0-amd64-netinst.iso

# Port forwarding
VBoxManage modifyvm inception --natpf1 "ssh,tcp,,2222,,22"
VBoxManage modifyvm inception --natpf1 "https,tcp,,8443,,443"

# Start VM
VBoxManage startvm inception --type headless
```

### SSH Access
```bash
# Connect to VM
ssh -p 2222 ggrisole@localhost

# With host key checking disabled (first time)
ssh -o StrictHostKeyChecking=no -p 2222 ggrisole@localhost
```

---

## 2. Docker Installation (Inside VM)

```bash
# Update system
sudo apt-get update
sudo apt-get upgrade -y

# Install Docker
sudo apt-get install -y docker.io docker-compose

# Add user to docker group
sudo usermod -aG docker ggrisole

# Logout and login again, then verify
docker --version
docker compose version
docker run hello-world
```

---

## 3. Project Structure Setup

```bash
# Create project directory
mkdir -p ~/inception/srcs/requirements/{mariadb,wordpress,nginx}/{conf,tools}

# Create data directories for bind mounts
mkdir -p ~/data/mariadb ~/data/wordpress

# Add domain to hosts
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts
```

---

## 4. SSL Certificate Generation

```bash
# Generate self-signed certificate
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout ~/inception/srcs/requirements/nginx/conf/nginx.key \
    -out ~/inception/srcs/requirements/nginx/conf/nginx.crt \
    -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=Student/CN=ggrisole.42.fr"
```

---

## 5. Secret Generation

```bash
# Generate random passwords
cd ~/inception/srcs

# Create secrets directory
mkdir -p secrets

# Generate passwords (use -n to avoid trailing newline!)
openssl rand -base64 16 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 16 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 16 | tr -d '\n' > secrets/wp_admin_password.txt
```

---

## 6. Docker Compose Commands

### Basic Operations
```bash
cd ~/inception

# Build and start (using Makefile)
make

# Or directly with docker compose
docker compose -f srcs/docker-compose.yml up -d --build

# View logs
docker compose -f srcs/docker-compose.yml logs -f
docker compose -f srcs/docker-compose.yml logs mariadb
docker compose -f srcs/docker-compose.yml logs wordpress
docker compose -f srcs/docker-compose.yml logs nginx

# Stop containers
docker compose -f srcs/docker-compose.yml down

# Stop and remove volumes
docker compose -f srcs/docker-compose.yml down -v
```

### Makefile Targets
```bash
make all        # Setup, build, and start
make setup      # Create directories and generate certs
make build      # Build images
make up         # Start containers
make down       # Stop containers
make clean      # Stop and remove containers
make fclean     # Full cleanup including data
make re         # Rebuild everything
```

---

## 7. Container Debugging

### Container Status
```bash
# List running containers
docker ps

# List all containers (including stopped)
docker ps -a

# Container details
docker inspect mariadb
docker inspect wordpress
docker inspect nginx
```

### Logs
```bash
# View logs
docker logs mariadb
docker logs wordpress
docker logs nginx

# Follow logs
docker logs -f mariadb

# Last 50 lines
docker logs --tail 50 mariadb
```

### Execute Commands Inside Containers
```bash
# Shell access
docker exec -it mariadb bash
docker exec -it wordpress bash
docker exec -it nginx sh

# Run specific commands
docker exec mariadb mysql -e "SHOW DATABASES;"
docker exec wordpress wp option get siteurl --allow-root
docker exec nginx nginx -t
```

---

## 8. MariaDB Commands

### Connection Testing
```bash
# Test connection as wpuser
docker exec mariadb mysql -uwpuser -p'PASSWORD' -e "SELECT 1;"

# Connect as root
docker exec mariadb mysql -uroot -p'ROOT_PASSWORD'
```

### User Management
```bash
# List users
docker exec mariadb mysql -e "SELECT User,Host FROM mysql.user;"

# Reset password (emergency fix)
docker exec mariadb mysql -e "ALTER USER 'wpuser'@'%' IDENTIFIED BY 'newpassword'; FLUSH PRIVILEGES;"

# Check grants
docker exec mariadb mysql -e "SHOW GRANTS FOR 'wpuser'@'%';"
```

### Database Operations
```bash
# List databases
docker exec mariadb mysql -e "SHOW DATABASES;"

# Show tables
docker exec mariadb mysql -e "USE wordpress; SHOW TABLES;"
```

---

## 9. WordPress CLI Commands

```bash
# Check WordPress version
docker exec wordpress wp core version --allow-root

# Get/Set options
docker exec wordpress wp option get siteurl --allow-root
docker exec wordpress wp option get home --allow-root
docker exec wordpress wp option update siteurl 'https://ggrisole.42.fr' --allow-root

# List users
docker exec wordpress wp user list --allow-root

# Check if installed
docker exec wordpress wp core is-installed --allow-root
```

---

## 10. Cleanup & Troubleshooting

### Full Cleanup (Nuclear Option)
```bash
cd ~/inception

# Stop everything
docker compose -f srcs/docker-compose.yml down -v

# Remove all Docker resources
docker system prune -af
docker volume prune -f
docker network prune -f

# Remove data directories
sudo rm -rf ~/data/mariadb ~/data/wordpress

# Recreate
mkdir -p ~/data/mariadb ~/data/wordpress

# Rebuild
docker compose -f srcs/docker-compose.yml up -d --build
```

### Check for Newline Issues
```bash
# View file with special characters
cat secrets/db_password.txt | xxd | head -2
# Look for "0a" at end = newline

# Fix: Strip newlines
cat secrets/db_password.txt | tr -d '\n' > secrets/db_password_fixed.txt
mv secrets/db_password_fixed.txt secrets/db_password.txt
```

### Network Debugging
```bash
# List networks
docker network ls

# Inspect network
docker network inspect srcs_inception_network

# Check container IPs
docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mariadb
```

---

## 11. Testing from Host Machine

```bash
# Add domain to host's /etc/hosts
echo "127.0.0.1 ggrisole.42.fr" | sudo tee -a /etc/hosts

# Test HTTPS (via port forwarding 8443->443)
curl -k https://localhost:8443/
curl -k -v https://ggrisole.42.fr:8443/

# Test from inside VM
ssh -p 2222 ggrisole@localhost
wget --no-check-certificate -q -O - https://ggrisole.42.fr/
```

---

## 12. File Transfer (SCP)

```bash
# Copy single file to VM
scp -P 2222 localfile.txt ggrisole@localhost:~/inception/

# Copy directory recursively
scp -P 2222 -r ~/local/inception/ ggrisole@localhost:~/inception/

# Copy from VM to local
scp -P 2222 ggrisole@localhost:~/inception/file.txt ./
```

---

## Quick Reference Card

| Action | Command |
|--------|---------|
| Start project | \`make\` or \`docker compose up -d --build\` |
| View logs | \`docker compose logs -f\` |
| Stop project | \`make down\` |
| Full rebuild | \`make fclean && make\` |
| Shell into mariadb | \`docker exec -it mariadb bash\` |
| Shell into wordpress | \`docker exec -it wordpress bash\` |
| Test DB connection | \`docker exec mariadb mysql -uwpuser -p'PASS' -e "SELECT 1;"\` |
| Check container status | \`docker ps\` |
| Nuke everything | See "Full Cleanup" section |
