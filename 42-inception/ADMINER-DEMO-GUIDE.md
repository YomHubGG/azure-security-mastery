# Adminer Quick Demo Guide

## 🐳 Base Images Used: 100% Debian

### **All Containers Use `debian:bookworm`**

```bash
NGINX:       FROM debian:bookworm
WordPress:   FROM debian:bookworm
MariaDB:     FROM debian:bookworm
Redis:       FROM debian:bookworm
FTP:         FROM debian:bookworm
Adminer:     FROM debian:bookworm
Static Site: FROM debian:bookworm
Portainer:   FROM debian:bookworm
```

### **Why Debian Instead of Alpine?**

#### **Project Requirements:**
- 42 School subject specifies: **"Debian Bookworm or Alpine (penultimate stable)"**
- Both are acceptable, you chose Debian

#### **Debian Advantages for This Project:**

1. **Package Availability**
   ```bash
   # Debian has more packages in official repos
   apt-get install mariadb-server    # ✅ Works perfectly
   apt-get install php-fpm php-mysql # ✅ All WordPress dependencies
   apt-get install vsftpd            # ✅ FTP server ready
   ```

2. **Documentation & Tutorials**
   - Most LAMP/LEMP stack guides use Debian/Ubuntu
   - MariaDB official docs target Debian
   - WordPress hosting guides assume Debian

3. **Stability**
   - Bookworm = Debian 12 (released June 2023)
   - Well-tested for production use
   - Predictable behavior

4. **Learning Value**
   - Same base as Ubuntu (most common production OS)
   - Skills transfer directly to real-world jobs

#### **Alpine Advantages (Not Used Here):**

1. **Smaller Images**
   ```
   alpine:3.19  → ~7 MB
   debian:bookworm → ~117 MB
   ```

2. **Security**
   - Minimal attack surface
   - No unnecessary packages

3. **Speed**
   - Faster downloads
   - Faster container startup

#### **Why You Chose Debian:**

✅ **Consistency** - Same base for all containers (easier debugging)  
✅ **Familiarity** - Debian is what 42 VMs use  
✅ **Compatibility** - No musl vs glibc issues (Alpine uses musl)  
✅ **Package Management** - apt-get is well-known  
✅ **42 Subject Compliance** - Explicitly allowed

---

## 🗃️ Adminer Quick Demo Guide

### **What is Adminer?**

**Adminer** = Single-file PHP database management tool (like phpMyAdmin but simpler)
- **Size:** 500 KB (vs phpMyAdmin's 50+ MB)
- **Features:** Browse tables, run SQL queries, import/export data
- **Security:** One file, easy to audit

### **Your Implementation:**

```dockerfile
FROM debian:bookworm
RUN apt-get install -y php-fpm php-mysql php-mysqli
RUN wget "https://github.com/vrana/adminer/releases/download/v4.8.1/adminer-4.8.1.php" -O adminer.php
RUN sed -i 's/listen = \/run\/php\/php.*-fpm.sock/listen = 9001/' /etc/php/*/fpm/pool.d/www.conf
EXPOSE 9001
CMD ["php-fpm8.2", "-F"]
```

**Key Points:**
- PHP-FPM listens on port **9001** (not socket)
- NGINX proxies with `fastcgi_pass adminer:9001`
- Single file: `adminer.php` (489 KB)

---

## 🎯 Adminer Demo for Evaluators

### **Step 1: Access Adminer**

```bash
# From browser:
https://localhost:8443/adminer
# or
https://ggrisole.42.fr:8443/adminer
```

**What you'll see:** Login page with 4 fields

---

### **Step 2: Login with wpuser (Recommended for Demo)**

Fill in the form:

| Field       | Value           | Why This Value?                          |
|-------------|-----------------|------------------------------------------|
| **System**  | MySQL           | MariaDB is MySQL-compatible              |
| **Server**  | mariadb         | Docker service name (DNS resolution)     |
| **Username**| wpuser          | WordPress database user                  |
| **Password**| *(from secret)* | Read from `/run/secrets/db_password`     |
| **Database**| wordpress       | WordPress database name                  |

**Get the password:**
```bash
ssh -p 2222 ggrisole@localhost
docker exec mariadb cat /run/secrets/db_password
# Copy the output (44 characters)
```

Click **Login** ✅

---

### **Step 3: Show Evaluators the WordPress Database**

#### **A) Browse Tables (Left Sidebar)**

You'll see all WordPress tables:
```
wp_commentmeta
wp_comments
wp_links
wp_options         ← Show this one!
wp_postmeta
wp_posts
wp_term_relationships
wp_term_taxonomy
wp_termmeta
wp_terms
wp_usermeta
wp_users           ← Show this one!
```

#### **B) Click `wp_users` Table**

Shows WordPress users in a visual table:

| user_id | user_login      | user_email           | user_registered      |
|---------|-----------------|----------------------|----------------------|
| 1       | ggrisoleadmin   | yom@student.42.fr    | 2025-12-25 16:16:36 |
| 2       | ggrisoleuser    | ggrisoleuser@42.fr   | 2025-12-25 16:16:38 |

**Points to highlight:**
- ✅ Two users created (admin + regular, as per subject)
- ✅ User `ggrisoleadmin` (NOT "admin" - forbidden by subject)
- ✅ Passwords hashed (click **show** to see WordPress hash format)

#### **C) Click `wp_options` Table**

Find row with `option_name = 'siteurl'`:

| option_id | option_name | option_value                    |
|-----------|-------------|---------------------------------|
| 1         | siteurl     | https://ggrisole.42.fr:8443    |
| 2         | home        | https://ggrisole.42.fr:8443    |

**Points to highlight:**
- ✅ This is why WordPress redirects work correctly
- ✅ Port 8443 included (Issue #15 fix from DECEMBER-25-FIXES.md)

---

### **Step 4: Run a Custom SQL Query (Impressive!)**

Click **SQL command** (top left)

**Demo Query 1: Count Posts**
```sql
SELECT COUNT(*) as total_posts FROM wp_posts WHERE post_status = 'publish';
```

**Demo Query 2: Show All Users**
```sql
SELECT user_login, user_email, user_registered 
FROM wp_users 
ORDER BY user_registered ASC;
```

**Demo Query 3: Check Redis Cache Plugin Status**
```sql
SELECT option_value 
FROM wp_options 
WHERE option_name = 'redis-cache';
```

**Why this is impressive:**
- Shows you understand SQL (not just clicking buttons)
- Demonstrates database relationships
- Proves Redis integration works

---

### **Step 5: Export Database (Show Backup Capability)**

1. Click **Export** (top right)
2. Select format: **SQL**
3. Click **Export**
4. Browser downloads `localhost-wordpress.sql`

**Why show this:**
- ✅ Demonstrates data portability
- ✅ Shows you understand database backups
- ✅ Proves Adminer is fully functional

---

### **Step 6: Login as Root (Advanced Demo)**

Logout from Adminer, login again:

| Field       | Value               |
|-------------|---------------------|
| **System**  | MySQL               |
| **Server**  | mariadb             |
| **Username**| root                |
| **Password**| *(from root secret)*|
| **Database**| *(leave empty)*     |

**Get root password:**
```bash
docker exec mariadb cat /run/secrets/db_root_password
```

**After login, show:**
- All databases (wordpress, mysql, sys, performance_schema)
- Click **mysql** database → **user** table
- Show that **wpuser** has limited permissions (only `%` host, only wordpress DB)

**Security point:**
> "Root has full access to all databases, but wpuser is restricted to only the wordpress database. This follows the principle of least privilege - WordPress doesn't need root access."

---

## 🎤 Demo Script for Evaluators

### **Opening (30 seconds)**

> "Adminer is a lightweight database management tool - just 489 KB compared to phpMyAdmin's 50 MB. It's running in its own container with PHP-FPM on port 9001, and NGINX proxies to it using FastCGI protocol."

### **Login (30 seconds)**

> "I'll login with wpuser, which is the dedicated WordPress database user. The password comes from a Docker secret mounted at /run/secrets/db_password - never hardcoded in the Dockerfile."

*Login and show wp_users table*

### **Show Tables (1 minute)**

> "These are the standard WordPress tables. Let me show wp_users - you can see the admin user is called 'ggrisoleadmin' not 'admin', which was forbidden by the subject. And here's the regular user with author role."

*Click wp_options table*

> "The siteurl option shows 'https://ggrisole.42.fr:8443' - this is why WordPress redirects stay on port 8443. This was Issue #15 I fixed on December 25."

### **SQL Query (1 minute)**

> "Let me run a custom SQL query to show the Redis cache integration..."

*Run:*
```sql
SELECT option_name, option_value 
FROM wp_options 
WHERE option_name LIKE 'redis%' 
LIMIT 5;
```

> "You can see Redis Object Cache is active and configured to use the redis service on port 6379."

### **Security Discussion (30 seconds)**

> "Adminer connects to MariaDB using the internal Docker network - the database isn't exposed to the internet. And if I login as root..."

*Logout, login as root, show mysql.user table*

> "...you can see wpuser only has access from '%' (any host within the Docker network) and is restricted to the wordpress database only. This follows least privilege principle."

---

## 📊 Impressive Points to Make

### **1. FastCGI vs HTTP Proxy**
> "Adminer uses PHP-FPM, which speaks FastCGI protocol. Initially I had NGINX using proxy_pass, which sends HTTP requests. This caused 502 errors because PHP-FPM doesn't speak HTTP. I fixed it by changing to fastcgi_pass - this was Issue #13."

### **2. Single-File Philosophy**
> "Adminer is just one PHP file - 489 KB. This makes it easier to audit for security vulnerabilities compared to phpMyAdmin's complex structure. For production, you'd disable or remove it after setup."

### **3. Docker Secrets in Action**
> "Notice I'm reading passwords from mounted secrets, not environment variables. Secrets are more secure because they're not visible in 'docker inspect' and aren't baked into image layers."

### **4. Network Isolation**
> "MariaDB port 3306 isn't exposed to the host - only accessible through the Docker network. Adminer can connect because it's on the same 'inception_network' bridge network."

---

## 🔒 Security Note for Defense

**Question:** "Isn't Adminer a security risk?"

**Answer:**
> "In production, yes - you'd want to remove it after initial setup or protect it with HTTP authentication. For this project, it's acceptable because:
> 1. It's only accessible via HTTPS through the NGINX proxy
> 2. MariaDB requires authentication (no anonymous access)
> 3. The database isn't exposed directly to the internet
> 4. It demonstrates understanding of database management tools
> 
> In a real production environment, I'd either remove it entirely or add these protections:
> - HTTP Basic Auth in NGINX
> - IP whitelist (only allow from specific IPs)
> - Different port that's not publicly documented
> - Or use SSH tunneling instead of web access"

---

## 🎯 Quick Reference

### **Access URLs:**
```
Adminer:  https://localhost:8443/adminer
          https://ggrisole.42.fr:8443/adminer
```

### **Credentials (wpuser):**
```
System:    MySQL
Server:    mariadb
Username:  wpuser
Password:  [from /run/secrets/db_password]
Database:  wordpress
```

### **Credentials (root):**
```
System:    MySQL
Server:    mariadb
Username:  root
Password:  [from /run/secrets/db_root_password]
Database:  [leave empty to see all]
```

### **Useful SQL Queries:**
```sql
-- Count WordPress posts
SELECT COUNT(*) FROM wp_posts WHERE post_status = 'publish';

-- Show users
SELECT user_login, user_email FROM wp_users;

-- Check Redis config
SELECT * FROM wp_options WHERE option_name LIKE 'redis%';

-- Show site URLs
SELECT option_name, option_value FROM wp_options 
WHERE option_name IN ('siteurl', 'home');

-- List all tables
SHOW TABLES;

-- Show database size
SELECT 
    table_schema AS 'Database',
    ROUND(SUM(data_length + index_length) / 1024 / 1024, 2) AS 'Size (MB)'
FROM information_schema.tables
WHERE table_schema = 'wordpress'
GROUP BY table_schema;
```

---

## ✅ Checklist Before Demo

Before your defense, verify:

```bash
# 1. Adminer is accessible
curl -k -s https://localhost:8443/adminer | grep '<title>'
# Expected: <title>Login - Adminer</title>

# 2. Get wpuser password
ssh -p 2222 ggrisole@localhost "docker exec mariadb cat /run/secrets/db_password"
# Copy this for login

# 3. Get root password (optional)
ssh -p 2222 ggrisole@localhost "docker exec mariadb cat /run/secrets/db_root_password"

# 4. Verify database exists
ssh -p 2222 ggrisole@localhost "docker exec mariadb mysql -u wpuser -p\$(docker exec mariadb cat /run/secrets/db_password) -e 'SHOW DATABASES;' 2>/dev/null"
# Should show: wordpress

# 5. Verify WordPress tables exist
ssh -p 2222 ggrisole@localhost "docker exec mariadb mysql -u wpuser -p\$(docker exec mariadb cat /run/secrets/db_password) wordpress -e 'SHOW TABLES;' 2>/dev/null"
# Should show: wp_users, wp_posts, wp_options, etc.
```

---

## 🚀 Pro Tip for Defense

If an evaluator asks **"Can you show me the database structure?"**, use Adminer to:

1. Click **wordpress** database (left sidebar)
2. Click any table (e.g., **wp_posts**)
3. Click **Show structure** (top right)

This displays:
- Column names and types
- Indexes (primary keys, foreign keys)
- Auto-increment settings
- Character encoding

Then say:
> "WordPress uses a well-designed relational schema. For example, wp_posts stores all content (posts, pages, custom types), and wp_postmeta provides extensible key-value metadata. The wp_users table links to wp_usermeta for additional user fields. This design allows plugins to extend WordPress without modifying core tables."

**This shows you understand database design, not just Docker!** 🎓
