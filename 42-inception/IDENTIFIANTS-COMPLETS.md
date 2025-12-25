# Inception VM - Liste des Identifiants et Mots de Passe

**Projet**: 42 Inception  
**VM**: inception (Debian 12 Bookworm)  
**Localisation**: /mnt/vmlab/inception/  
**Date**: Novembre 2025

---

## 🖥️ Accès Système (VM)

### Système d'exploitation Debian

| Type | Login | Mot de passe | Notes |
|------|-------|--------------|-------|
| **Utilisateur VM** | `ggrisole` | *[défini lors installation Debian]* | Utilisateur principal de la VM |
| **Root VM** | `root` | *[défini lors installation Debian]* | Accès sudo disponible pour ggrisole |

### Accès SSH

```bash
# Depuis l'hôte local
ssh -p 2222 ggrisole@localhost
```

**Port forwarding VirtualBox**:
- Host port 2222 → VM port 22 (SSH)
- Host port 8443 → VM port 443 (HTTPS)

---

## 🗄️ Base de Données (MariaDB)

### Utilisateurs MariaDB

| Utilisateur | Hôte | Mot de passe | Fichier secret | Privilèges |
|-------------|------|--------------|----------------|------------|
| **root** | localhost | *[généré aléatoirement]* | `secrets/db_root_password.txt` | Tous (super admin) |
| **wpuser** | % (tout hôte) | *[généré aléatoirement]* | `secrets/db_password.txt` | Base `wordpress` uniquement |

### Base de données

| Nom | Propriétaire | Usage |
|-----|--------------|-------|
| **wordpress** | wpuser | Données WordPress (posts, users, settings) |

### Variables d'environnement (.env)

```bash
# MariaDB
MYSQL_ROOT_PASSWORD_FILE=/run/secrets/db_root_password
MYSQL_DATABASE=wordpress
MYSQL_USER=wpuser
MYSQL_PASSWORD_FILE=/run/secrets/db_password
```

### Génération des secrets

```bash
# Root password (32 caractères base64, sans newline)
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt

# WordPress user password (32 caractères base64, sans newline)
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
```

### Test de connexion

```bash
# Depuis conteneur MariaDB (root)
mysql -u root -p$(cat /run/secrets/db_root_password) -e "SHOW DATABASES;"

# Depuis conteneur WordPress (wpuser)
mysql -h mariadb -u wpuser -p$(cat /run/secrets/db_password) wordpress -e "SHOW TABLES;"
```

---

## 🌐 WordPress

### Utilisateurs WordPress (Web)

| Utilisateur | Rôle | Email | Mot de passe | Fichier secret |
|-------------|------|-------|--------------|----------------|
| **ggrisoleadmin** | Administrateur | yom@student.42.fr | *[généré aléatoirement]* | `secrets/wp_admin_password.txt` |
| **ggrisoleuser** | Auteur | ggrisoleuser@student.42.fr | *[défini en variable .env]* | N/A (en clair dans .env) |

**Note importante**: Le nom de l'admin est **ggrisoleadmin** (pas "admin" - interdit par le sujet 42).

### Variables d'environnement (.env)

```bash
# WordPress Admin
WP_ADMIN_USER=ggrisoleadmin
WP_ADMIN_PASSWORD_FILE=/run/secrets/wp_admin_password
WP_ADMIN_EMAIL=yom@student.42.fr

# WordPress Regular User
WP_USER=ggrisoleuser
WP_USER_EMAIL=ggrisoleuser@student.42.fr
WP_USER_PASSWORD=RegularUserPass123  # En clair dans .env (utilisateur non-critique)

# Configuration
WP_TITLE="Inception Project"
WP_URL=https://ggrisole.42.fr
```

### Génération des secrets

```bash
# Admin password (32 caractères base64, sans newline)
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt
```

### Accès WordPress

```
URL:      https://ggrisole.42.fr/
Admin:    https://ggrisole.42.fr/wp-admin
Login 1:  ggrisoleadmin / [voir secrets/wp_admin_password.txt]
Login 2:  ggrisoleuser / [voir .env WP_USER_PASSWORD]
```

### Création des utilisateurs (automatisée dans wp-setup.sh)

```bash
# Admin (avec secret file)
WP_ADMIN_PASS=$(cat /run/secrets/wp_admin_password | tr -d '\n')
wp core install \
    --url="${WP_URL}" \
    --title="${WP_TITLE}" \
    --admin_user="${WP_ADMIN_USER}" \
    --admin_password="${WP_ADMIN_PASS}" \
    --admin_email="${WP_ADMIN_EMAIL}" \
    --allow-root

# Regular user (avec variable env)
wp user create "${WP_USER}" "${WP_USER_EMAIL}" \
    --role=author \
    --user_pass="${WP_USER_PASSWORD}" \
    --allow-root
```

---

## 📁 FTP (vsftpd)

### Utilisateur FTP

| Utilisateur | Mot de passe | Home directory | Permissions |
|-------------|--------------|----------------|-------------|
| **ftpuser** | ftppass | /var/www/html | Lecture/Écriture |

**Note**: Ces identifiants sont codés en dur dans le Dockerfile FTP (environnement de développement local).

### Configuration vsftpd

```bash
# Ports
Control:  21
Passive:  21000-21010

# Sécurité
anonymous_enable=NO
local_enable=YES
write_enable=YES
chroot_local_user=YES
```

### Accès FTP

```bash
# Depuis l'hôte
ftp localhost -P 21
# Login: ftpuser
# Password: ftppass

# Ou avec client GUI (FileZilla)
Host: localhost
Port: 21
User: ftpuser
Password: ftppass
```

### Volume monté

Le conteneur FTP monte le volume WordPress (`~/data/wordpress`) permettant l'upload/modification des fichiers du site.

---

## 🔍 Adminer (Interface DB)

### Accès Adminer

```
URL:        https://ggrisole.42.fr/adminer
Serveur:    mariadb
Utilisateur: wpuser
Mot de passe: [voir secrets/db_password.txt]
Base:       wordpress
```

**Pas d'authentification Adminer** : L'interface demande directement les credentials de la base de données.

### Connexion

1. Ouvrir `https://ggrisole.42.fr/adminer`
2. Sélectionner "System: MySQL"
3. Serveur: `mariadb`
4. Utilisateur: `wpuser`
5. Mot de passe: `[contenu de secrets/db_password.txt]`
6. Base de données: `wordpress`

---

## 🐳 Portainer (Gestion Conteneurs)

### Premier accès

```
URL: http://localhost:9443
```

**Création compte admin** : Au premier lancement, Portainer demande de créer un compte administrateur.

| Champ | Valeur suggérée |
|-------|-----------------|
| Username | admin |
| Password | [choisir mot de passe fort] |

**Note**: Ces identifiants ne sont **pas** pré-configurés, tu dois les créer lors de la première visite.

### Configuration persistante

Le mot de passe admin est stocké dans le volume Docker `portainer_data`.

---

## 🔐 Certificats TLS/SSL

### Certificat auto-signé NGINX

| Fichier | Localisation | Type |
|---------|--------------|------|
| Certificat | `srcs/requirements/nginx/conf/nginx.crt` | Certificat public |
| Clé privée | `srcs/requirements/nginx/conf/nginx.key` | Clé privée (PROTÉGER) |

### Génération

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout srcs/requirements/nginx/conf/nginx.key \
  -out srcs/requirements/nginx/conf/nginx.crt \
  -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=Student/CN=ggrisole.42.fr"
```

**Paramètres**:
- CN (Common Name): ggrisole.42.fr
- Validité: 365 jours
- Algorithme: RSA 2048 bits

---

## 📊 Récapitulatif des Secrets

### Fichiers secrets (NE PAS COMMIT DANS GIT)

```
secrets/
├── db_root_password.txt      # MariaDB root
├── db_password.txt            # MariaDB wpuser (utilisé par WordPress)
└── wp_admin_password.txt      # WordPress admin (ggrisoleadmin)
```

### Variables en clair (.env - NE PAS COMMIT)

```bash
# Utilisateur WordPress regular
WP_USER_PASSWORD=RegularUserPass123

# FTP (codé en dur dans Dockerfile, pas dans .env)
# Voir: requirements/bonus/ftp/Dockerfile
```

### .gitignore Configuration

```bash
# Secrets
secrets/
.env

# Certificats
*.key
*.crt
*.pem

# Data
data/
```

---

## 🔍 Retrouver les Mots de Passe

### Sur la VM

```bash
# Connexion SSH
ssh -p 2222 ggrisole@localhost

# Voir les secrets (depuis ~/inception/srcs/)
cat secrets/db_root_password.txt     # MariaDB root
cat secrets/db_password.txt          # MariaDB wpuser
cat secrets/wp_admin_password.txt    # WordPress admin

# Voir les variables .env
cat .env | grep PASSWORD
```

### Depuis les conteneurs

```bash
# MariaDB root password (depuis conteneur mariadb)
docker exec mariadb cat /run/secrets/db_root_password

# WordPress database password (depuis conteneur wordpress)
docker exec wordpress cat /run/secrets/db_password

# WordPress admin password (depuis conteneur wordpress)
docker exec wordpress cat /run/secrets/wp_admin_password
```

### Test de connexion rapide

```bash
# MariaDB (wpuser)
docker exec mariadb mysql -u wpuser -p$(docker exec mariadb cat /run/secrets/db_password) -e "SELECT 1;"

# WordPress admin login
# URL: https://ggrisole.42.fr/wp-admin
# User: ggrisoleadmin
# Pass: $(ssh -p 2222 ggrisole@localhost 'cat ~/inception/srcs/secrets/wp_admin_password.txt')
```

---

## 🚨 Sécurité : Meilleures Pratiques Appliquées

### ✅ Ce qui est bien

1. **Secrets hors Git** : Aucun mot de passe dans le repository
2. **Génération aléatoire** : Mots de passe générés avec OpenSSL (32 caractères base64)
3. **Séparation des privilèges** : 
   - MariaDB root ≠ MariaDB wpuser
   - WordPress admin ≠ WordPress author
4. **Réseau isolé** : Les services communiquent sur réseau Docker privé
5. **Pas de remote root** : MariaDB root accessible uniquement depuis localhost du conteneur

### ⚠️ Points à améliorer en production

1. **FTP hardcodé** : Les identifiants FTP sont en dur dans le Dockerfile
   - Solution prod: Utiliser variables d'environnement ou secrets
2. **Certificat auto-signé** : Pas de confiance navigateur
   - Solution prod: Let's Encrypt avec certbot
3. **Rotation des secrets** : Pas de politique de rotation
   - Solution prod: Rotation automatique tous les 90 jours
4. **Portainer sans HTTPS** : Admin interface sur HTTP
   - Solution prod: Configurer TLS pour Portainer

---

## 📝 Checklist Avant Défense

### Vérifier les accès

- [ ] SSH fonctionne : `ssh -p 2222 ggrisole@localhost`
- [ ] Tous les conteneurs sont "Up" : `docker ps`
- [ ] MariaDB est "healthy" : `docker ps` (colonne STATUS)
- [ ] WordPress accessible : `https://ggrisole.42.fr/`
- [ ] WordPress admin login fonctionne : `/wp-admin`
- [ ] WordPress user login fonctionne : `/wp-admin` (ggrisoleuser)
- [ ] Adminer accessible : `https://ggrisole.42.fr/adminer`
- [ ] FTP fonctionne : `ftp localhost 21` (ftpuser/ftppass)
- [ ] Portainer accessible : `http://localhost:9443`

### Vérifier les secrets

- [ ] Fichiers secrets existent : `ls -la ~/inception/srcs/secrets/`
- [ ] Pas de newlines dans secrets : `cat secrets/* | xxd` (pas de 0x0a à la fin)
- [ ] .env existe : `ls -la ~/inception/srcs/.env`
- [ ] .env pas dans Git : `git status` (ne doit pas apparaître)

### Test de connexion bases de données

```bash
# MariaDB wpuser
docker exec mariadb mysql -u wpuser -p$(cat secrets/db_password.txt) wordpress -e "SHOW TABLES;"

# Redis (si configuré)
docker exec redis redis-cli ping
# PONG attendu
```

---

## 🎯 Points Clés pour Évaluateurs

### Question: "Comment sont gérés les mots de passe ?"

**Réponse** : 
- Générés aléatoirement avec OpenSSL (32 caractères base64)
- Stockés dans `secrets/` (exclu de Git via .gitignore)
- Passés aux conteneurs via Docker secrets (bind mounts dans /run/secrets/)
- Utilisés par les scripts d'initialisation qui les lisent depuis /run/secrets/
- Précaution prise pour supprimer les newlines invisibles (tr -d '\n')

### Question: "Quel est le login admin WordPress ?"

**Réponse** : 
- `ggrisoleadmin` (pas "admin" qui est interdit par le sujet)
- Mot de passe dans `secrets/wp_admin_password.txt`
- Deuxième utilisateur : `ggrisoleuser` (rôle auteur)

### Question: "Comment accéder à la base de données ?"

**Réponse** : 
- Via Adminer : `https://ggrisole.42.fr/adminer`
- Serveur: `mariadb`, User: `wpuser`, Pass: `[voir secrets/]`, DB: `wordpress`
- Ou en ligne de commande depuis conteneurs Docker

---

**Document complet et prêt pour défense !** ✅
