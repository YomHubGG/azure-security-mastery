# Inception Project - Installation Timeline

**Marathon Session**: November 28, 2025
**Duration**: ~4 hours of debugging
**Result**: ✅ Success

---

## Timeline of Events

### Phase 1: VM Setup (Success)
```
✅ Downloaded Debian 12.8.0 Bookworm (from archive - "penultimate stable")
✅ Created VirtualBox VM "inception" on /mnt/vmlab
✅ Configured: 4GB RAM, 2 CPUs, 30GB disk, NAT networking
✅ Set up port forwarding: 2222→22 (SSH), 8443→443 (HTTPS)
✅ Installed Debian 12 with user "ggrisole"
```

### Phase 2: Docker Installation (Success)
```
✅ Installed docker.io and docker-compose
✅ Added ggrisole to docker group
✅ Tested with docker run hello-world
```

### Phase 3: Project File Creation (Success)
```
✅ Created Makefile
✅ Created srcs/docker-compose.yml with 3 services
✅ Created srcs/.env with environment variables
✅ Created MariaDB Dockerfile and init-db.sh
✅ Created WordPress Dockerfile and wp-setup.sh
✅ Created NGINX Dockerfile and nginx.conf
✅ Generated SSL certificates for ggrisole.42.fr
✅ Generated random passwords in secrets/
✅ Transferred all files to VM via SCP
```

### Phase 4: MariaDB Debugging (Multiple Failures → Success)

#### Attempt 1: ❌ MariaDB not initializing
```
Problem:  "Starting MariaDB" but never "Initializing database"
Cause:    Data directory had old corrupted data from previous VM
Fix:      sudo rm -rf ~/data/mariadb/* && restart
```

#### Attempt 2: ❌ wpuser not created
```
Problem:  "ERROR 1130: Host not allowed to connect"
Cause:    User created with 'localhost' instead of '%'
Fix:      Changed CREATE USER to use '%' wildcard
```

#### Attempt 3: ❌ User creation fails silently
```
Problem:  wpuser still doesn't exist after rebuild
Cause:    Root password set BEFORE creating users, locking out mysql commands
Fix:      Reordered script - create users FIRST, secure root LAST
```

#### Attempt 4: ❌ Access denied despite user existing
```
Problem:  "ERROR 1045: Access denied for user 'wpuser'"
Cause:    User exists but password doesn't match!
Debug:    Checked with: cat /run/secrets/db_password | xxd
Found:    Password had trailing NEWLINE (\n = 0x0a)!
```

#### Attempt 5: ✅ THE FIX
```bash
# Added to init-db.sh:
DB_PASS=$(cat /run/secrets/db_password | tr -d '\n')
```

After applying this fix, MariaDB authentication worked perfectly.

### Phase 5: WordPress Setup (Quick Fix)

#### Issue: Files already present loop
```
Problem:  "Error: WordPress files seem to already be present here"
Cause:    Previous failed attempt left files but no wp-config.php
Fix:      Cleaned ~/data/wordpress/* and let it reinstall fresh
```

#### Result: ✅ WordPress installed successfully
```
- WordPress 6.8.3 installed
- Admin user created
- Site title: "Inception Project"
- Database connection verified
```

### Phase 6: Final Verification (Success)
```
✅ All 3 containers running (nginx, wordpress, mariadb)
✅ MariaDB healthcheck passing
✅ WordPress accessible at https://ggrisole.42.fr
✅ TLSv1.3 working with self-signed certificate
✅ Database queries working
```

---

## Root Cause Analysis

### The Invisible Bug: Trailing Newlines

The core issue that caused hours of debugging:

```bash
# How passwords were generated:
openssl rand -base64 16 > secrets/db_password.txt

# What this actually stores:
V5srgpVFTi+4+hGh040eig==\n  # <-- invisible newline!

# When MariaDB sets the password:
CREATE USER 'wpuser'@'%' IDENTIFIED BY 'V5srgpVFTi+4+hGh040eig==
';  # Password literally includes the newline!

# When WordPress tries to connect with:
'V5srgpVFTi+4+hGh040eig=='  # No newline - DOESN'T MATCH!

# Result:
ERROR 1045 (28000): Access denied for user 'wpuser'@'172.18.0.3'
```

### The Fix

```bash
# Strip newlines when reading secrets:
DB_PASS=$(cat /run/secrets/db_password | tr -d '\n')

# Or prevent them when creating:
openssl rand -base64 16 | tr -d '\n' > secrets/db_password.txt
```

---

## Lessons Learned

1. **Invisible characters kill authentication** - Always strip `\n`, `\r`, `\0`
2. **Order of operations matters** - In init scripts, create users before securing root
3. **Bind mounts persist** - Always clean data directories between rebuilds
4. **Use xxd to debug** - `cat file | xxd` reveals hidden characters
5. **Healthchecks are essential** - They reveal issues early
6. **Test from inside first** - External port forwarding adds complexity

---

## Final State

```
CONTAINER ID   STATUS                   NAMES
3acabd7ef40d   Up 2 hours               nginx
be5e47258b41   Up (after restart)       wordpress
771e6b5ec75a   Up 2 hours (healthy)     mariadb

https://ggrisole.42.fr → "Inception Project" WordPress site
```

**Marathon Complete! 🎉**
