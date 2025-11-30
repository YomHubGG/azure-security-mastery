# 🏃 INCEPTION 48-HOUR MARATHON

**Start**: November 28, 2025 (NOW)  
**Deadline**: November 30, 2025  
**Status**: LET'S GO! 💪

---

## ⚡ ESSENTIAL LINKS (BOOKMARK ALL)

### Debian & Docker
| Resource | Link |
|----------|------|
| **Debian 12 ISO** | https://cdimage.debian.org/debian-cd/current/amd64/iso-cd/debian-12.8.0-amd64-netinst.iso |
| **Docker Install (Debian)** | https://docs.docker.com/engine/install/debian/ |
| **Docker Compose Install** | https://docs.docker.com/compose/install/linux/ |
| **Dockerfile Reference** | https://docs.docker.com/engine/reference/builder/ |
| **Docker Compose Reference** | https://docs.docker.com/compose/compose-file/ |

### Services Documentation
| Service | Link |
|---------|------|
| **NGINX** | https://nginx.org/en/docs/ |
| **NGINX SSL Config** | https://nginx.org/en/docs/http/configuring_https_servers.html |
| **MariaDB Docker** | https://mariadb.com/kb/en/installing-and-using-mariadb-via-docker/ |
| **WordPress CLI** | https://developer.wordpress.org/cli/commands/ |
| **PHP-FPM Config** | https://www.php.net/manual/en/install.fpm.configuration.php |
| **Redis** | https://redis.io/docs/getting-started/ |
| **vsftpd** | https://security.appspot.com/vsftpd/vsftpd_conf.html |
| **Adminer** | https://www.adminer.org/ |

### Security & TLS
| Resource | Link |
|----------|------|
| **OpenSSL Cert Generation** | https://www.openssl.org/docs/man1.1.1/man1/req.html |
| **Mozilla SSL Config Generator** | https://ssl-config.mozilla.org/ |
| **TLS Best Practices** | https://wiki.mozilla.org/Security/Server_Side_TLS |

### 42-Specific
| Resource | Link |
|----------|------|
| **Docker PID 1 Problem** | https://blog.phusion.nl/2015/01/20/docker-and-the-pid-1-zombie-reaping-problem/ |
| **Docker Secrets** | https://docs.docker.com/compose/use-secrets/ |
| **Docker Healthchecks** | https://docs.docker.com/compose/compose-file/05-services/#healthcheck |

---

## 📅 48-HOUR BATTLE PLAN

### ⏰ DAY 1 - November 28 (Today)

#### BLOCK 1: VM Setup (2 hours) - START NOW
```
□ 09:00-09:30  Download Debian 12 ISO (if not already)
□ 09:30-10:00  Create VM in VirtualBox (4GB RAM, 2 CPUs, 30GB disk)
□ 10:00-11:00  Install Debian (minimal, SSH server only)
```

#### BLOCK 2: Environment (2 hours)
```
□ 11:00-11:30  Install Docker + Docker Compose in VM
□ 11:30-12:00  Create project structure (srcs/, secrets/, Makefile)
□ 12:00-12:30  Generate SSL certificates + secrets
□ 12:30-13:00  Setup /etc/hosts (ggrisole.42.fr) + .env file
```

**🍽️ LUNCH BREAK: 13:00-14:00**

#### BLOCK 3: MariaDB (2 hours)
```
□ 14:00-14:30  Write MariaDB Dockerfile
□ 14:30-15:00  Write init-db.sh script
□ 15:00-15:30  Write my.cnf config
□ 15:30-16:00  Test MariaDB container standalone
```

#### BLOCK 4: WordPress (2 hours)
```
□ 16:00-16:30  Write WordPress Dockerfile
□ 16:30-17:00  Write wp-setup.sh script
□ 17:00-17:30  Write www.conf (PHP-FPM)
□ 17:30-18:00  Test WordPress container standalone
```

**🍽️ DINNER BREAK: 18:00-19:00**

#### BLOCK 5: NGINX (2 hours)
```
□ 19:00-19:30  Write NGINX Dockerfile
□ 19:30-20:00  Write nginx.conf + default.conf
□ 20:00-20:30  Write docker-compose.yml (all 3 services)
□ 20:30-21:00  Complete Makefile
```

#### BLOCK 6: Integration (2 hours)
```
□ 21:00-21:30  First full build: make
□ 21:30-22:00  Debug any issues
□ 22:00-22:30  Test WordPress accessible via HTTPS
□ 22:30-23:00  Create WordPress users (admin + regular)
```

**🛏️ DAY 1 TARGET: Mandatory part WORKING**

---

### ⏰ DAY 2 - November 29

#### BLOCK 7: Bonus - Redis (1 hour)
```
□ 09:00-09:30  Write Redis Dockerfile + config
□ 09:30-10:00  Integrate with WordPress (redis-cache plugin)
```

#### BLOCK 8: Bonus - Adminer (1 hour)
```
□ 10:00-10:30  Write Adminer Dockerfile
□ 10:30-11:00  Add NGINX location for Adminer, test
```

#### BLOCK 9: Bonus - Static Site (1 hour)
```
□ 11:00-11:30  Write Static Site Dockerfile (Python HTTP server)
□ 11:30-12:00  Create simple HTML portfolio + test
```

#### BLOCK 10: Bonus - FTP (1 hour)
```
□ 12:00-12:30  Write FTP Dockerfile + config
□ 12:30-13:00  Test FTP access to WordPress volume
```

**🍽️ LUNCH BREAK: 13:00-14:00**

#### BLOCK 11: Bonus - Custom Service (1 hour)
```
□ 14:00-14:30  Write Portainer Dockerfile (container monitoring)
□ 14:30-15:00  Test Portainer UI
```

#### BLOCK 12: Security & Testing (2 hours)
```
□ 15:00-15:30  Verify no passwords in Dockerfiles
□ 15:30-16:00  Test container auto-restart
□ 16:00-16:30  Full clean rebuild: make fclean && make
□ 16:30-17:00  Verify all services accessible
```

#### BLOCK 13: Polish & Defense Prep (2 hours)
```
□ 17:00-17:30  Create architecture diagram
□ 17:30-18:00  Write defense notes
□ 18:00-18:30  Practice elevator pitch
□ 18:30-19:00  Final testing + screenshots
```

#### BLOCK 14: VM Export (1 hour)
```
□ 19:00-19:30  Export VM to external disk
□ 19:30-20:00  Test import on fresh system (if time)
```

**🎯 DAY 2 TARGET: Full bonus + ready for evaluation**

---

## 🎤 ELEVATOR PITCH (30 seconds)

> "My Inception project implements a **production-grade containerized infrastructure** with NGINX as a reverse proxy handling TLS 1.3, WordPress on PHP-FPM for dynamic content, and MariaDB for data persistence.
>
> What makes my implementation stand out: I've applied **enterprise security practices** - all secrets use Docker secrets management, containers run as non-root users, and I've implemented proper PID 1 handling without hacky workarounds.
>
> For bonus services, I added **Redis caching** to reduce database load, **Adminer** for database administration, an **FTP server** for content management, a **static portfolio site** using Python, and **Portainer** for container monitoring.
>
> The entire infrastructure is **reproducible** with a single `make` command and **properly documented** for maintainability."

---

## 🎤 ELEVATOR PITCH (FRENCH VERSION)

> "Mon projet Inception implémente une **infrastructure conteneurisée de qualité production** avec NGINX comme reverse proxy gérant TLS 1.3, WordPress sur PHP-FPM pour le contenu dynamique, et MariaDB pour la persistance des données.
>
> Ce qui distingue mon implémentation : j'ai appliqué des **pratiques de sécurité entreprise** - tous les secrets utilisent Docker secrets, les conteneurs tournent sans privilèges root, et j'ai implémenté une gestion correcte du PID 1 sans workarounds.
>
> Pour les bonus, j'ai ajouté **Redis** pour le cache, **Adminer** pour l'administration BDD, un **serveur FTP** pour la gestion de contenu, un **site portfolio statique** en Python, et **Portainer** pour le monitoring.
>
> Toute l'infrastructure est **reproductible** avec une simple commande `make` et **correctement documentée**."

---

## 📁 PROJECT STRUCTURE (Copy-Paste Ready)

```bash
# Run this in your VM to create the entire structure:
mkdir -p ~/inception/{secrets,srcs/requirements/{nginx,wordpress,mariadb,bonus/{redis,ftp,adminer,static-site,portainer}}/{conf,tools}}
cd ~/inception
touch Makefile
touch srcs/{docker-compose.yml,.env}
touch srcs/requirements/nginx/{Dockerfile,.dockerignore}
touch srcs/requirements/wordpress/{Dockerfile,.dockerignore}
touch srcs/requirements/mariadb/{Dockerfile,.dockerignore}
touch srcs/requirements/bonus/redis/{Dockerfile,.dockerignore}
touch srcs/requirements/bonus/ftp/{Dockerfile,.dockerignore}
touch srcs/requirements/bonus/adminer/{Dockerfile,.dockerignore}
touch srcs/requirements/bonus/static-site/{Dockerfile,.dockerignore}
touch srcs/requirements/bonus/portainer/{Dockerfile,.dockerignore}
```

---

## 🔐 SECRETS GENERATION (Run in VM)

```bash
# Generate all passwords
cd ~/inception
openssl rand -base64 24 > secrets/db_root_password.txt
openssl rand -base64 24 > secrets/db_password.txt
openssl rand -base64 24 > secrets/wp_admin_password.txt
openssl rand -base64 24 > secrets/ftp_password.txt

# Remove newlines
for f in secrets/*.txt; do tr -d '\n' < "$f" > "$f.tmp" && mv "$f.tmp" "$f"; done

# Verify
cat secrets/db_password.txt && echo ""
```

---

## 🔒 SSL CERTIFICATE GENERATION

```bash
cd ~/inception/srcs/requirements/nginx/tools

# Generate self-signed certificate
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout ggrisole.42.fr.key \
  -out ggrisole.42.fr.crt \
  -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=Student/CN=ggrisole.42.fr"

# Verify
openssl x509 -in ggrisole.42.fr.crt -text -noout | head -20
```

---

## 📋 DEFENSE CHECKLIST

### Mandatory Requirements
- [ ] All in VM with Docker Compose
- [ ] Custom Dockerfiles (no pre-built images except Debian base)
- [ ] NGINX with TLS 1.2/1.3 ONLY on port 443
- [ ] WordPress with php-fpm (no nginx)
- [ ] MariaDB (no nginx)
- [ ] 2 volumes: DB + WordPress files at /home/ggrisole/data/
- [ ] Docker network connecting containers
- [ ] Containers auto-restart on crash
- [ ] Domain ggrisole.42.fr points to localhost
- [ ] 2 WordPress users (admin name ≠ 'admin')
- [ ] No passwords in Dockerfiles
- [ ] Environment variables in .env
- [ ] No infinite loops (tail -f, sleep infinity, while true)
- [ ] Proper PID 1 handling

### Bonus Requirements
- [ ] Redis cache for WordPress
- [ ] FTP server → WordPress volume
- [ ] Adminer for DB management
- [ ] Static website (not PHP)
- [ ] Custom service (Portainer - justified)

### Technical Demo Points
- [ ] Show `make re` rebuilds everything
- [ ] Show `docker exec nginx cat /etc/nginx/nginx.conf` (TLS config)
- [ ] Show `docker exec mariadb mysql -u wpuser -p` (DB connection)
- [ ] Show WordPress admin panel (ggrisole.42.fr)
- [ ] Show container restart: `docker kill nginx && sleep 5 && docker ps`
- [ ] Show volumes: `ls -la /home/ggrisole/data/`
- [ ] Show no passwords: `grep -r "password" srcs/requirements/*/Dockerfile`

---

## 🚨 COMMON ISSUES & QUICK FIXES

### MariaDB won't start
```bash
# Check logs
docker logs mariadb

# Permission fix
sudo chown -R 999:999 /home/ggrisole/data/mariadb
```

### WordPress can't connect to DB
```bash
# Test from WordPress container
docker exec wordpress mysqladmin -h mariadb -u wpuser -p ping
# Enter password from secrets/db_password.txt
```

### NGINX 502 Bad Gateway
```bash
# Check if PHP-FPM is running
docker exec wordpress ps aux | grep php-fpm

# Check WordPress container is up
docker logs wordpress
```

### SSL/TLS errors
```bash
# Check certificate
openssl s_client -connect ggrisole.42.fr:443 -tls1_3

# Verify files exist
docker exec nginx ls -la /etc/ssl/certs/
```

### Container keeps restarting
```bash
# Check exit code
docker inspect nginx --format='{{.State.ExitCode}}'

# Check full logs
docker logs --tail 100 nginx
```

---

## ⏱️ TIME-SAVING TIPS

1. **Copy-paste from IMPLEMENTATION-GUIDE.md** - All configs are ready
2. **Test each container BEFORE combining** - Saves debug time
3. **Use `docker logs -f container_name`** - Real-time debugging
4. **Don't skip health checks** - They prevent cascade failures
5. **Commit working states** - Easy rollback if something breaks

---

## 🏁 FINAL VERIFICATION SCRIPT

```bash
#!/bin/bash
# Save as verify.sh and run before evaluation

echo "=== Inception Project Verification ==="

echo -e "\n[1] Checking containers..."
docker ps --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}"

echo -e "\n[2] Checking volumes..."
docker volume ls
ls -la /home/ggrisole/data/

echo -e "\n[3] Testing HTTPS..."
curl -sk https://ggrisole.42.fr | head -5

echo -e "\n[4] Checking TLS version..."
echo | openssl s_client -connect ggrisole.42.fr:443 2>/dev/null | grep "Protocol"

echo -e "\n[5] Checking Dockerfiles for passwords..."
grep -rn "password\|PASSWORD" srcs/requirements/*/Dockerfile && echo "WARNING: Password found!" || echo "OK: No passwords in Dockerfiles"

echo -e "\n[6] Checking WordPress users..."
docker exec wordpress wp user list --allow-root

echo -e "\n[7] Testing container restart..."
docker kill nginx
sleep 5
docker ps | grep nginx && echo "OK: Auto-restart works"

echo -e "\n=== Verification Complete ==="
```

---

## 💪 LET'S GO!

**You have 48 hours. The plan is tight but doable.**

Start with Block 1 RIGHT NOW:
1. Download Debian ISO (if needed)
2. Create VM in VirtualBox
3. Begin installation

**The implementation guide has ALL the code. Copy, paste, adapt, test, move on.**

**You've got this! 🚀**
