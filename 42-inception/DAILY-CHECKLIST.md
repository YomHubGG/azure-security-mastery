# Inception Project - Daily Checklist

**Project Period**: November 17-30, 2025 (13 days)  
**Goal**: Portable VM on external disk, ready for campus evaluation

---

## Day 1 - Nov 17 (TODAY) ✅

**Morning: VM Setup**
- [ ] Download Debian 12 Bookworm ISO
- [ ] Create VirtualBox VM on external disk
  - Name: inception-yom
  - RAM: 4-8 GB
  - CPU: 2-4 cores
  - Disk: 30 GB on external disk
  - Network: Bridged
- [ ] Install Debian (minimal install + SSH server)

**Afternoon: Base System**
- [ ] Update system: `sudo apt update && sudo apt upgrade -y`
- [ ] Install Docker: `sudo apt install -y docker.io docker-compose make git vim`
- [ ] Add user to docker group: `sudo usermod -aG docker $USER`
- [ ] Verify: `docker --version && docker-compose --version`

**Evening: Project Structure**
- [ ] Create directory tree (nginx, wordpress, mariadb, bonus folders)
- [ ] Create Makefile skeleton
- [ ] Create .gitignore (secrets/, .env, data/)
- [ ] Initialize git repo (optional, local only)

**End of Day**: VM ready, Docker installed, project structure created

---

## Day 2 - Nov 18

**Morning: Security Foundation**
- [ ] Generate SSL certificates (self-signed for yom.42.fr)
- [ ] Generate secrets (DB passwords, WP admin password)
- [ ] Create secrets/ folder with password files
- [ ] Verify secrets NOT in git

**Afternoon: Environment Configuration**
- [ ] Create srcs/.env with all variables
- [ ] Add domain to /etc/hosts: `127.0.0.1 yom.42.fr`
- [ ] Test domain resolution: `ping yom.42.fr`

**Evening: Makefile Completion**
- [ ] Complete Makefile with all targets (all, build, up, down, clean, fclean, re)
- [ ] Test: `make setup` creates /home/yom/data directories
- [ ] Document Makefile usage

**End of Day**: Secrets ready, environment configured, Makefile working

---

## Day 3 - Nov 19

**Focus: MariaDB Container**
- [ ] Write mariadb/Dockerfile (FROM debian:bookworm)
- [ ] Create mariadb/conf/my.cnf configuration
- [ ] Write mariadb/tools/init-db.sh script
  - Read secrets from /run/secrets/
  - Initialize database if not exists
  - Create WordPress database + user
- [ ] Test build: `docker build -t mariadb:test srcs/requirements/mariadb`
- [ ] Test run standalone (verify logs, no errors)

**End of Day**: MariaDB container builds and initializes database

---

## Day 4 - Nov 20

**Focus: WordPress Container**
- [ ] Write wordpress/Dockerfile (php-fpm, WP-CLI, no nginx)
- [ ] Create wordpress/conf/www.conf (PHP-FPM pool)
- [ ] Write wordpress/tools/wp-setup.sh script
  - Wait for MariaDB health check
  - Download WordPress via WP-CLI
  - Create wp-config.php
  - Install WordPress with admin user (NOT named 'admin')
  - Create second user (regular author)
- [ ] Test build: `docker build -t wordpress:test srcs/requirements/wordpress`

**End of Day**: WordPress container builds and connects to MariaDB

---

## Day 5 - Nov 21

**Focus: NGINX Container + Docker Compose**
- [ ] Write nginx/Dockerfile (FROM debian:bookworm)
- [ ] Create nginx/conf/nginx.conf (main config)
- [ ] Create nginx/conf/default.conf (server block)
  - Listen 443 ssl only
  - TLS 1.2/1.3 protocols only
  - Proxy to wordpress:9000 (FastCGI)
  - Security headers
- [ ] Complete srcs/docker-compose.yml
  - All 3 mandatory services
  - Networks, volumes, secrets defined
  - Health checks, restart policies
- [ ] Test: `make` to build and start all services
- [ ] Verify: `curl -k https://yom.42.fr` shows WordPress

**End of Day**: Mandatory part complete and working! 🎉

---

## Day 6 - Nov 22

**Focus: Redis Cache (Bonus 1/5)**
- [ ] Write bonus/redis/Dockerfile
- [ ] Create bonus/redis/conf/redis.conf
- [ ] Add redis service to docker-compose.yml
- [ ] Update WordPress to use Redis
  - Install redis-cache plugin via WP-CLI
  - Configure WP_REDIS_HOST in wp-config.php
  - Enable Redis cache
- [ ] Test: Verify caching works (check Redis keys)

**End of Day**: Redis integrated with WordPress

---

## Day 7 - Nov 23

**Focus: FTP Server (Bonus 2/5)**
- [ ] Write bonus/ftp/Dockerfile (vsftpd)
- [ ] Create bonus/ftp/conf/vsftpd.conf
- [ ] Write bonus/ftp/tools/ftp-setup.sh
  - Create FTP user
  - Link to WordPress volume
- [ ] Add ftp service to docker-compose.yml
- [ ] Test: Connect via FTP client, access WordPress files

**End of Day**: FTP server accessible

---

## Day 8 - Nov 24

**Focus: Adminer + Static Site (Bonus 3/5 + 4/5)**

**Morning: Adminer**
- [ ] Write bonus/adminer/Dockerfile
- [ ] Download Adminer PHP file
- [ ] Add adminer service to docker-compose.yml
- [ ] Add NGINX location block for Adminer
- [ ] Test: Access Adminer UI, connect to MariaDB

**Afternoon: Static Site**
- [ ] Write bonus/static-site/Dockerfile (Python SimpleHTTPServer or nginx)
- [ ] Create simple HTML/CSS portfolio page
- [ ] Add static-site service to docker-compose.yml
- [ ] Test: Access static site via browser

**End of Day**: Adminer and static site working

---

## Day 9 - Nov 25

**Focus: Custom Service (Bonus 5/5)**

**Choose and implement ONE of:**
- [ ] **Option A: Portainer** (container management UI)
  - Easy to justify: "monitoring and managing Docker infrastructure"
  - Official lightweight container available
- [ ] **Option B: Grafana + Prometheus** (monitoring)
  - Justification: "performance monitoring and alerting"
- [ ] **Option C: GitLab Runner** (CI/CD)
  - Justification: "automated testing and deployment"

**Implementation:**
- [ ] Write bonus/[service]/Dockerfile
- [ ] Add service to docker-compose.yml
- [ ] Test functionality
- [ ] Prepare defense justification (why this service is useful)

**End of Day**: All 5 bonus services complete! 🚀

---

## Day 10 - Nov 26

**Focus: Security Hardening**

**Morning: Apply Azure Security Knowledge**
- [ ] Day 35 - Container Security:
  - [ ] Multi-stage builds (reduce image size)
  - [ ] Non-root users in containers
  - [ ] Alpine base images where possible
- [ ] Day 53 - Secrets Management:
  - [ ] Verify no passwords in Dockerfiles
  - [ ] Test Docker secrets mounting
  - [ ] Check .gitignore coverage
- [ ] Day 55 - Application Security:
  - [ ] Security headers in NGINX
  - [ ] TLS configuration hardening
  - [ ] Disable unnecessary PHP functions
- [ ] Day 9 - Network Security:
  - [ ] Verify network isolation
  - [ ] Check open ports (only 443 external)

**Afternoon: Security Scanning**
- [ ] Install Trivy: `docker pull aquasec/trivy`
- [ ] Scan all images for vulnerabilities
- [ ] Fix HIGH/CRITICAL vulnerabilities
- [ ] Document security measures

**Evening: Documentation**
- [ ] Create architecture diagram (draw.io, ASCII art, or hand-drawn)
- [ ] Write SECURITY.md explaining measures
- [ ] Prepare defense talking points

**End of Day**: Project hardened, documented, secure

---

## Day 11 - Nov 27

**Focus: Testing & Validation**

**Morning: Functional Testing**
- [ ] Test 1: Clean rebuild (`make fclean && make`)
- [ ] Test 2: All services start without errors
- [ ] Test 3: WordPress login (admin + regular user)
- [ ] Test 4: Create post, verify Redis caching
- [ ] Test 5: FTP file operations
- [ ] Test 6: Adminer database queries
- [ ] Test 7: All bonus services accessible

**Afternoon: Resilience Testing**
- [ ] Test container restart: `docker stop nginx && sleep 10 && docker ps`
- [ ] Test volume persistence: `make down && make up` (data intact?)
- [ ] Test resource limits: `docker stats`
- [ ] Test build time: `time make re` (should be < 5 min)

**Evening: Create Test Report**
- [ ] Document all tests performed
- [ ] Note any issues found and fixes applied
- [ ] Create troubleshooting guide

**End of Day**: Fully tested, issues resolved

---

## Day 12 - Nov 28

**Focus: Final Polish**

**Morning: Performance Optimization**
- [ ] Optimize Dockerfiles (multi-stage, layer caching)
- [ ] Prune unused images: `docker system prune -a`
- [ ] Measure final image sizes: `docker images`
- [ ] Test cold start time

**Afternoon: Defense Preparation**
- [ ] Create DEFENSE-NOTES.md:
  - Architecture explanation
  - Service interaction flow
  - Security measures
  - Bonus justification
  - Common questions + answers
- [ ] Practice explaining project (15 min presentation)
- [ ] Prepare for live coding challenge (add a new env var, rebuild)

**Evening: Final Verification**
- [ ] Run complete checklist from IMPLEMENTATION-GUIDE.md
- [ ] All 23 checkboxes ✅
- [ ] Project ready for export

**End of Day**: Defense-ready, polished project

---

## Day 13 - Nov 29/30

**Focus: VM Export & Campus Testing**

**Morning: VM Export**
- [ ] Shut down VM properly
- [ ] Export VM to external disk:
  ```bash
  VBoxManage export inception-yom -o /path/to/external/disk/inception.ova
  ```
- [ ] Verify export file size (~15-20 GB)
- [ ] Test import on host PC (ensure it boots)

**Afternoon: Campus Dry Run**
- [ ] If possible, test on campus PC with external disk
- [ ] Boot VM from external disk
- [ ] Verify all services start
- [ ] Test network connectivity
- [ ] Measure boot-to-running time (should be < 3 min)

**Evening: Final Preparation**
- [ ] Charge laptop, bring power adapter
- [ ] Prepare external disk (USB 3.0 cable)
- [ ] Print architecture diagram (optional)
- [ ] Review defense notes
- [ ] Get good sleep! 😴

**End of Day**: Ready for evaluation! 🎯

---

## Evaluation Day - Nov 30

**Pre-Defense Checklist** (30 min before)
- [ ] Arrive early, set up VM on evaluation PC
- [ ] Boot VM, verify Docker services running
- [ ] Test: `curl -k https://yom.42.fr` works
- [ ] Have DEFENSE-NOTES.md open in editor
- [ ] Calm, confident mindset

**During Defense**
- [ ] Explain architecture clearly (use diagram)
- [ ] Show clean code (no hacks, proper PID 1 handling)
- [ ] Demonstrate all mandatory + bonus services
- [ ] Handle live modification confidently
- [ ] Answer questions with security insights (Azure knowledge!)

**Post-Defense**
- [ ] Note any questions you struggled with
- [ ] Update documentation based on feedback
- [ ] Celebrate completion! 🎉

---

## Daily Time Estimates

- **Days 1-2**: 6-8 hours (VM setup, configuration)
- **Days 3-5**: 8-10 hours each (core services development)
- **Days 6-9**: 4-6 hours each (bonus services)
- **Days 10-11**: 6-8 hours (security, testing)
- **Days 12-13**: 4-6 hours (polish, export, prep)

**Total**: ~85-100 hours over 13 days (~7-8 hours/day)

---

## Quick Reference

**Build and Start**
```bash
make                    # Build and start all services
make logs              # View all container logs
make ps                # List running containers
```

**Rebuild Everything**
```bash
make fclean            # Stop, remove containers, volumes, images, data
make                   # Rebuild from scratch
```

**Testing**
```bash
curl -k https://yom.42.fr                    # Test NGINX
docker exec mariadb mysqladmin ping          # Test MariaDB
docker exec wordpress wp --info --allow-root # Test WordPress
docker logs -f nginx                          # Follow NGINX logs
```

**Troubleshooting**
```bash
docker ps -a                                 # List all containers
docker logs <container_name>                 # View container logs
docker exec -it <container_name> /bin/bash  # Enter container
docker-compose config                        # Validate docker-compose.yml
```

---

## Success Metrics

**Mandatory (100 points)**
- ✅ All 3 services running
- ✅ Custom Dockerfiles (no pre-built images)
- ✅ Docker Compose orchestration
- ✅ Makefile automation
- ✅ TLS 1.2/1.3 only
- ✅ 2 WordPress users (admin name != 'admin')
- ✅ Secrets management (no hardcoded passwords)
- ✅ Auto-restart on crash
- ✅ Volumes in /home/yom/data

**Bonus (up to 25 points)**
- ✅ Redis cache
- ✅ FTP server
- ✅ Adminer
- ✅ Static site
- ✅ Custom service (justified)

**Total Possible**: 125 points

---

**Let's crush this! 💪 You've got the Azure security expertise to make this exceptional.**
