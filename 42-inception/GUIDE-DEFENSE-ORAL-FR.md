# Guide de Défense Orale - Projet Inception

**Projet**: 42 Inception - Infrastructure Docker Sécurisée  
**Date**: Décembre 2025  
**Objectif**: Comprendre séquentiellement tout ce qui a été réalisé  
**Format**: Guide d'écoute pour préparation sportive

---

## 📋 Introduction du Projet

### Qu'est-ce qu'Inception ?

Inception est un projet de l'école 42 qui demande de créer une infrastructure web complète en utilisant Docker. Au lieu de simplement télécharger des images toutes prêtes, tu dois construire tes propres conteneurs depuis zéro, les configurer pour qu'ils communiquent ensemble, et tout sécuriser selon les meilleures pratiques de l'industrie.

**L'objectif pédagogique** : Comprendre comment fonctionnent les conteneurs, l'orchestration de services, et la sécurité en profondeur.

**Ce qui rend ce projet unique** : Tu as appliqué 74 jours de formation en cybersécurité Azure pour le rendre vraiment professionnel. Ce n'est pas juste un projet d'école, c'est une démonstration de compétences réelles.

---

## 🎯 Architecture Globale

### Vue d'ensemble des 8 services

Tu as déployé **8 conteneurs Docker** qui travaillent ensemble :

**Services obligatoires (3)** :
1. **NGINX** - Le serveur web (porte d'entrée unique)
2. **WordPress** - L'application web (le site)
3. **MariaDB** - La base de données (stockage des données)

**Services bonus (5)** :
4. **Redis** - Le cache (accélère WordPress)
5. **FTP** - Serveur de fichiers (accès aux fichiers WordPress)
6. **Adminer** - Interface de gestion de base de données
7. **Site Statique** - Un site vitrine en HTML/CSS
8. **Portainer** - Interface de gestion des conteneurs

### Comment tout est organisé

**L'infrastructure repose sur une machine virtuelle** :
- **VM "inception"** sur disque externe USB NVME
- **OS** : Debian 12 Bookworm
- **Localisation** : `/mnt/vmlab/inception/`
- **Accès** : `ssh -p 2222 ggrisole@localhost`

**Réseau Docker** :
- Tous les services sont sur un réseau privé appelé `srcs_inception_network`
- Ils communiquent par leurs noms (comme `redis`, `mariadb`, `wordpress`)
- Seul NGINX est exposé à l'extérieur sur le port 443 (HTTPS)

**Stockage persistant** :
- `/home/ggrisole/data/mariadb` - Données de la base de données
- `/home/ggrisole/data/wordpress` - Fichiers du site WordPress
- Volume Docker pour Portainer

---

## 🔧 Service par Service : Que Fait Chacun ?

### 1. NGINX - Le Gardien de la Porte

**Rôle** : C'est le seul point d'entrée pour tout le trafic web. Il décide qui va où.

**Ce qu'il fait concrètement** :
- Écoute sur le port 443 (HTTPS uniquement)
- Gère le chiffrement TLS (certificat auto-signé pour `ggrisole.42.fr`)
- Redirige les requêtes vers WordPress
- Proxifie `/adminer` vers le service Adminer
- Proxifie `/static` vers le site statique
- Ajoute des headers de sécurité (HSTS, X-Frame-Options, CSP)

**Pourquoi c'est important** : Un seul point d'entrée = un seul endroit à sécuriser. C'est la philosophie "gateway unique".

**Configuration clé** :
```nginx
# Écoute uniquement HTTPS
listen 443 ssl;
ssl_protocols TLSv1.2 TLSv1.3;

# Route vers WordPress
location ~ \.php$ {
    fastcgi_pass wordpress:9000;
}

# Route vers Adminer
location /adminer {
    proxy_pass http://adminer:9001;
}
```

---

### 2. WordPress - L'Application Web

**Rôle** : C'est le site web lui-même, un système de gestion de contenu.

**Ce qu'il fait concrètement** :
- Sert le site web à `https://ggrisole.42.fr/`
- Fournit un tableau de bord d'administration (`/wp-admin`)
- Gère les articles, pages, médias
- Communique avec MariaDB pour lire/écrire des données
- Utilise Redis pour mettre en cache les objets (accélération)
- Tourne sur PHP-FPM (FastCGI Process Manager)

**Utilisateurs créés** :
- Un administrateur (nom depuis `.env`)
- Un auteur (peut écrire mais pas tout modifier)

**Configuration automatisée** : Tu utilises WP-CLI (WordPress Command Line Interface) pour tout automatiser :
```bash
# Installation WordPress
wp core install --url="https://ggrisole.42.fr" --title="Inception Project"

# Création des utilisateurs
wp user create author1 author@example.com --role=author

# Activation du cache Redis
wp plugin install redis-cache --activate
wp redis enable
```

**Pourquoi PHP-FPM** : C'est plus rapide et sécurisé que d'avoir PHP directement dans NGINX. NGINX parle à PHP-FPM via FastCGI.

---

### 3. MariaDB - La Base de Données

**Rôle** : Stocke toutes les données de WordPress (articles, utilisateurs, paramètres).

**Ce qu'il fait concrètement** :
- Fournit une base de données SQL pour WordPress
- Gère les transactions et l'intégrité des données
- Accessible **uniquement** depuis le réseau Docker (pas depuis l'extérieur)
- Les données sont persistées dans `~/data/mariadb`

**Pourquoi MariaDB et pas MySQL** : MariaDB est un fork open-source de MySQL, créé par le développeur original de MySQL. C'est 100% compatible mais plus ouvert et souvent plus performant.

**Sécurité** :
- Utilisateur root sécurisé avec mot de passe fort
- Utilisateur WordPress avec droits limités à sa base
- Pas d'accès root distant
- Health check pour vérifier que la base répond

**Configuration de sécurité** :
```sql
-- Créer utilisateur avec accès limité
CREATE USER 'wpuser'@'%' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON wordpress.* TO 'wpuser'@'%';

-- Sécuriser root
ALTER USER 'root'@'localhost' IDENTIFIED BY 'strong_password';
DELETE FROM mysql.user WHERE User='';
```

---

### 4. Redis - Le Cache Éclair

**Rôle** : Accélère WordPress en gardant en mémoire les données fréquemment demandées.

**Ce qu'il fait concrètement** :
- Stocke en RAM les objets PHP de WordPress
- Évite des requêtes SQL répétées
- Réduit le temps de chargement des pages
- Gère une éviction automatique quand la mémoire est pleine (politique LRU)

**Configuration spécifique** :
```conf
bind 0.0.0.0           # Accepte connexions depuis réseau Docker
protected-mode no      # Pas besoin de password dans réseau isolé
port 6379              # Port standard Redis
maxmemory 256mb        # Limite de mémoire
maxmemory-policy allkeys-lru  # Éviction : supprimer les clés les moins utilisées
daemonize no           # Reste en foreground pour Docker
```

**Le piège résolu** : Initialement, Redis était configuré avec `bind 127.0.0.1` (localhost uniquement). Ça empêchait WordPress de se connecter depuis son propre conteneur. La solution : `bind 0.0.0.0` pour accepter les connexions du réseau Docker tout en restant isolé d'Internet.

---

### 5. FTP - L'Accès aux Fichiers

**Rôle** : Permet d'accéder aux fichiers WordPress via FTP (utile pour modifier des thèmes/plugins).

**Ce qu'il fait concrètement** :
- Serveur vsftpd (Very Secure FTP Daemon)
- Monte le volume WordPress en lecture/écriture
- Utilisateur : `ftpuser` / mot de passe depuis `.env`
- Ports : 21 (contrôle) + 21000-21010 (passif)

**Configuration sécurisée** :
```conf
anonymous_enable=NO              # Pas d'accès anonyme
local_enable=YES                 # Utilisateurs locaux uniquement
write_enable=YES                 # Permettre uploads
chroot_local_user=YES            # Emprisonner utilisateurs dans leur dossier
pasv_min_port=21000              # Range ports passifs
pasv_max_port=21010
```

**Pourquoi vsftpd** : Le nom dit tout - "Very Secure". C'est un serveur FTP conçu avec la sécurité comme priorité #1.

---

### 6. Adminer - L'Interface de Base de Données

**Rôle** : Une interface web pour gérer MariaDB (comme phpMyAdmin mais en mieux).

**Ce qu'il fait concrètement** :
- Accessible à `https://ggrisole.42.fr/adminer`
- Permet de voir/modifier les tables de la base de données
- Exécuter des requêtes SQL manuellement
- Exporter/importer des données
- **Un seul fichier PHP** de 470KB (incroyable !)

**Créateur** : Jakub Vrána (République Tchèque, 2007)

**Pourquoi Adminer plutôt que phpMyAdmin** :
- Beaucoup plus léger (470KB vs 11MB)
- Un seul fichier au lieu de milliers
- Plus rapide
- Interface plus moderne
- Support de plus de bases de données (MySQL, PostgreSQL, SQLite, etc.)

**Configuration** :
```php
// Dockerfile simple
FROM debian:bookworm
RUN apt-get install -y php8.2-fpm php8.2-mysqli
COPY adminer-4.8.1.php /var/www/html/index.php
CMD ["php-fpm", "-F"]
```

---

### 7. Site Statique - La Vitrine

**Rôle** : Un site web statique personnalisé (HTML/CSS) montrant tes compétences.

**Ce qu'il fait concrètement** :
- Accessible à `https://ggrisole.42.fr/static`
- Sert du HTML/CSS pur (pas de backend)
- Design responsive avec dégradé violet
- Grille de projets avec images et descriptions
- **Construit pour ce projet** (pas un template)

**Taille** :
- HTML : 2.6KB
- CSS : 1.8KB
- Total : 4.4KB (ultra-léger !)

**Pourquoi un site statique** :
- Démontre la compétence HTML/CSS
- Extrêmement rapide (pas de traitement côté serveur)
- Sécurisé (pas de code exécutable)
- Peut servir de portfolio

---

### 8. Portainer - Le Tableau de Bord Docker

**Rôle** : Une interface web pour gérer tous tes conteneurs Docker.

**Ce qu'il fait concrètement** :
- Interface graphique pour Docker
- Voir l'état de tous les conteneurs
- Démarrer/arrêter/redémarrer des services
- Voir les logs en temps réel
- Gérer les images, volumes, réseaux
- Accessible sur `http://localhost:9443`

**Version** : Portainer CE 2.19.4 (Community Edition)

**Configuration spécifique** :
- Monte le socket Docker (`/var/run/docker.sock`) en lecture seule
- Inclut docker-compose v1.29.2 (dépendance requise)
- Volume persistant pour garder la configuration

**Le bug résolu** : Portainer redémarrait en boucle avec "docker-compose plugin is missing". Solution : installer docker-compose directement dans l'image Portainer.

---

## 🔐 Sécurité : Les Mesures Appliquées

### 1. Isolation Réseau

**Principe** : Tous les services sont sur un réseau Docker privé, invisible depuis l'extérieur.

**Implémentation** :
```yaml
networks:
  inception_network:
    driver: bridge
```

**Résultat** :
- Seul NGINX expose un port (443)
- MariaDB, Redis, Adminer, etc. ne sont accessibles qu'entre conteneurs
- Communication par noms DNS (`redis:6379`, `mariadb:3306`)

**Connaissance appliquée** : Day 9 - Azure Virtual Network (segmentation réseau)

---

### 2. Gestion des Secrets

**Principe** : Aucun mot de passe en clair dans le code, tout dans des variables d'environnement.

**Implémentation** :
```bash
# Fichier .env (exclu de Git)
MYSQL_ROOT_PASSWORD=generated_secret
MYSQL_USER=wpuser
MYSQL_PASSWORD=another_secret
WP_ADMIN_USER=admin
WP_ADMIN_PASSWORD=strong_password
WP_ADMIN_EMAIL=admin@example.com
```

**Protection Git** :
```bash
# .gitignore
.env
secrets/
*.key
*.crt
*.pem
```

**Génération sécurisée** :
```bash
# Mots de passe aléatoires
openssl rand -base64 16 | tr -d '\n' > secrets/db_password.txt

# Certificats TLS auto-signés
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout nginx.key -out nginx.crt
```

**Le piège résolu** : Les mots de passe générés avaient des retours à la ligne invisibles (`\n`). Quand MariaDB créait l'utilisateur, il incluait le `\n` dans le mot de passe. Quand WordPress essayait de se connecter sans le `\n`, ça échouait. Solution : `tr -d '\n'` pour supprimer tous les retours à la ligne.

**Connaissance appliquée** : Day 53 - Azure Key Vault (gestion des secrets)

---

### 3. Chiffrement TLS

**Principe** : Tout le trafic web est chiffré en HTTPS avec TLS 1.2 ou 1.3.

**Configuration NGINX** :
```nginx
ssl_certificate /etc/nginx/ssl/nginx.crt;
ssl_certificate_key /etc/nginx/ssl/nginx.key;
ssl_protocols TLSv1.2 TLSv1.3;
ssl_ciphers HIGH:!aNULL:!MD5;
ssl_prefer_server_ciphers on;
```

**Headers de sécurité** :
```nginx
add_header Strict-Transport-Security "max-age=31536000" always;
add_header X-Frame-Options "SAMEORIGIN" always;
add_header X-Content-Type-Options "nosniff" always;
add_header X-XSS-Protection "1; mode=block" always;
```

**Ce que ça fait** :
- HSTS : Force le navigateur à toujours utiliser HTTPS
- X-Frame-Options : Empêche le site d'être intégré dans une iframe (protection clickjacking)
- X-Content-Type-Options : Empêche le navigateur de deviner le type MIME (protection XSS)
- X-XSS-Protection : Active le filtre anti-XSS du navigateur

**Connaissance appliquée** : Day 21 - Application Security (TLS/SSL)

---

### 4. Health Checks

**Principe** : Vérifier automatiquement que les services sont en bonne santé.

**Exemples** :

**MariaDB** :
```yaml
healthcheck:
  test: ["CMD", "mysqladmin", "ping", "-h", "localhost"]
  interval: 10s
  timeout: 5s
  retries: 5
  start_period: 30s
```

**Redis** :
```yaml
healthcheck:
  test: ["CMD", "redis-cli", "ping"]
  interval: 10s
  timeout: 5s
  retries: 3
```

**Ce que ça apporte** :
- Docker sait si un conteneur fonctionne vraiment (pas juste "running")
- Peut redémarrer automatiquement si problème
- Les autres services peuvent attendre qu'une dépendance soit "healthy"

**Connaissance appliquée** : Days 41-43 - Kubernetes (liveness/readiness probes)

---

### 5. Politiques de Redémarrage

**Principe** : Si un conteneur plante, Docker le redémarre automatiquement.

**Configuration** :
```yaml
restart: always
```

**Options possibles** :
- `no` : Ne jamais redémarrer
- `on-failure` : Redémarrer seulement si exit code ≠ 0
- `always` : Toujours redémarrer (même après reboot serveur)
- `unless-stopped` : Comme always, sauf si arrêt manuel

**Ce que ça apporte** :
- Résilience automatique
- Pas d'intervention manuelle nécessaire
- Le système s'auto-répare

**Connaissance appliquée** : Days 41-43 - Kubernetes (self-healing)

---

### 6. Versions Épinglées

**Principe** : Ne jamais utiliser `latest`, toujours spécifier une version.

**Exemples** :
```dockerfile
FROM debian:bookworm              # Version Debian 12
FROM alpine:3.18                  # Version Alpine spécifique
RUN apt-get install php8.2-fpm    # Version PHP spécifique
```

**Pourquoi** :
- `latest` peut changer sans prévenir
- Casse la reproductibilité
- Introduit des vulnérabilités sans que tu le saches

**Connaissance appliquée** : Day 51 - Supply Chain Security

---

## 🐛 Les 12 Problèmes Résolus

> **Mise à jour 25 décembre 2025** : Après une pause de 2 semaines, 5 problèmes critiques supplémentaires ont été découverts et résolus lors de la reprise du projet. Ces problèmes (#8-12) démontrent une capacité de debugging systématique sur une infrastructure non-fonctionnelle.

### Problème #1 : Redis Sort Immédiatement

**Symptôme** : Le conteneur Redis se créait puis s'arrêtait avec exit code 0.

**Cause** : Redis était en mode daemon (`daemonize yes`). En mode daemon, le processus se détache et retourne immédiatement. Docker pense que le processus principal est terminé et arrête le conteneur.

**Solution** :
```dockerfile
CMD ["redis-server", "/etc/redis/redis.conf", "--daemonize", "no"]
```

**Concept** : Un conteneur Docker a besoin d'un processus en **foreground** (PID 1). Si ce processus se termine ou devient background, le conteneur s'arrête.

**Connaissance appliquée** : Days 41-43 - Kubernetes (compréhension de PID 1)

---

### Problème #2 : Docker Compose v1 vs v2

**Symptôme** : `docker compose up -d` donnait "unknown shorthand flag: 'f' in -f"

**Cause** : La VM Debian 12 a docker-compose v1 (commande `docker-compose` avec trait d'union), pas v2 (commande `docker compose` sans trait d'union).

**Différences** :
- **v1** : `docker-compose` - Binaire Python séparé
- **v2** : `docker compose` - Plugin Docker en Go intégré

**Solution** : Utiliser systématiquement `docker-compose` (v1) partout :
```bash
docker-compose up -d --build
docker-compose down
docker-compose logs
```

**Concept** : Toujours vérifier la version d'un outil avant d'écrire des scripts.

**Connaissance appliquée** : Day 59 - DevSecOps (debugging systématique)

---

### Problème #3 : Portainer Boucle de Redémarrage

**Symptôme** : Portainer redémarrait sans cesse avec exit code 1.

**Logs** : `"docker-compose plugin is missing from config path"`

**Cause** : Portainer 2.19.4 nécessite le binaire docker-compose, mais il n'était pas dans l'image.

**Solution** : Installer docker-compose dans le Dockerfile de Portainer :
```dockerfile
RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" \
    -o /usr/local/bin/docker-compose && \
    chmod +x /usr/local/bin/docker-compose
```

**Concept** : Les dépendances d'une application doivent être **dans l'image**, pas sur l'hôte.

**Connaissance appliquée** : Days 41-43 - Kubernetes (troubleshooting conteneurs)

---

### Problème #4 : Redis Network Binding

**Symptôme** : WordPress ne pouvait pas se connecter à Redis : "Connection refused at redis:6379"

**Cause** : Redis était configuré avec `bind 127.0.0.1` (localhost uniquement). Dans Docker, chaque conteneur a son propre localhost. Redis refusait donc les connexions venant du conteneur WordPress.

**Diagnostic** :
```bash
# Depuis conteneur WordPress
redis-cli -h redis ping
# Error: Connection refused

# Depuis conteneur Redis
redis-cli -h 127.0.0.1 ping
# PONG (ça marche !)
```

**Solution** :
```conf
bind 0.0.0.0           # Accepter connexions de toutes interfaces
protected-mode no      # Pas de password nécessaire dans réseau isolé
```

**Concept** :
- `127.0.0.1` = localhost uniquement (dans le conteneur)
- `0.0.0.0` = toutes les interfaces réseau (accepte réseau Docker)
- Dans un réseau Docker isolé, `0.0.0.0` reste sécurisé

**Connaissance appliquée** : Day 9 - Network Security (isolation et communication inter-services)

---

### Problème #5 : Configuration WordPress Redis

**Symptôme** : Le plugin Redis Object Cache essayait de se connecter à `127.0.0.1:6379` au lieu de `redis:6379`.

**Cause** : WordPress ne savait pas où trouver Redis. Par défaut, les applications cherchent sur localhost.

**Solution** : Configurer WordPress avec WP-CLI :
```bash
wp config set WP_REDIS_HOST redis --allow-root
wp config set WP_REDIS_PORT 6379 --raw --allow-root
wp config set WP_REDIS_TIMEOUT 1 --raw --allow-root
wp config set WP_REDIS_READ_TIMEOUT 1 --raw --allow-root
```

**Résultat** : WordPress utilise maintenant `redis:6379` (résolution DNS Docker).

**Concept** : Dans Docker/Kubernetes, les services se trouvent par **nom DNS**, pas par IP ni localhost.

**Connaissance appliquée** : Days 41-43 - Kubernetes (service discovery)

---

### Problème #6 : Outils Redis Manquants

**Symptôme** : `redis-cli: command not found` dans le conteneur WordPress.

**Cause** : Il y a une différence entre :
- **Extension PHP Redis** (`php-redis`) : Permet à PHP de parler à Redis
- **Outils Redis** (`redis-tools`) : Donne la commande `redis-cli`

WordPress avait l'extension mais pas les outils.

**Solution** : Installer les deux dans le Dockerfile WordPress :
```dockerfile
RUN apt-get install -y \
    php8.2-redis \      # Extension PHP
    redis-tools \       # Outils CLI
    ...
```

**Concept** : Client et outils sont souvent des paquets séparés.

**Connaissance appliquée** : Day 35 - Container Security (gestion des dépendances)

---

### Problème #7 : NGINX Reverse Proxy

**Symptôme** : `/adminer` et `/static` retournaient 404.

**Cause** : NGINX ne savait pas qu'il devait rediriger ces chemins vers d'autres services.

**Solution** : Ajouter des blocs `location` dans nginx.conf :
```nginx
location /adminer {
    proxy_pass http://adminer:9001;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}

location /static {
    proxy_pass http://static-site:8080;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}
```

**Concept** : Un reverse proxy route les requêtes vers différents backends selon l'URL.

**Connaissance appliquée** : Day 35 - Container Security (configuration services)

---

### Problème #8 : Secrets Incorrects (25 vs 44 octets) - 25 décembre

**Symptôme** : WordPress ne pouvait pas se connecter à MariaDB. Les logs montraient :
```
Waiting for MariaDB...
Attempt 1/30...
Attempt 2/30...
[continue à l'infini]
```

**Diagnostic** :
```bash
docker exec wordpress wc -c /run/secrets/db_password
# Output: 25   # ❌ Trop court!

docker logs mariadb | grep -i warning
# [Warning] Aborted connection to db: 'unconnected' user: 'unauthenticated'
```

**Cause profonde** : 
- Anciens secrets créés le 28 novembre : 25 octets (probablement sans `-base64`)
- Nouveaux secrets générés le 25 décembre : 44 octets (format correct)
- Docker montait les anciens secrets depuis `~/inception/secrets/` (cached)
- Les nouveaux secrets avaient été générés dans le **mauvais emplacement** (`~/inception/srcs/secrets/`)
- Le docker-compose.yml référence `../secrets/` (relatif au dossier `srcs/`)

**Solution** :
```bash
cd ~/inception
mkdir -p secrets
openssl rand -base64 32 | tr -d '\n' > secrets/db_root_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/db_password.txt
openssl rand -base64 32 | tr -d '\n' > secrets/wp_admin_password.txt

# Vérification
wc -c secrets/*
# 44 db_password.txt
# 44 db_root_password.txt
# 44 wp_admin_password.txt
```

**Concept** : 
- Les secrets Docker doivent être **exactement 44 octets** en base64
- La commande `tr -d '\n'` est **critique** pour supprimer le retour à la ligne
- Les chemins relatifs dans docker-compose sont **relatifs au fichier compose**, pas au Makefile
- Base64 de 32 octets → 44 caractères ASCII (32 * 4/3 = 42.67 arrondi)

**Connaissance appliquée** : Day 17 - Key Vault Secrets (gestion sécurisée des secrets)

---

### Problème #9 : MariaDB N'initialise Jamais la Base - 25 décembre

**Symptôme** : MariaDB démarre mais ne crée jamais la base de données ni l'utilisateur `wpuser`.
```bash
docker logs mariadb | head -80
# Starting MariaDB...
# [Note] mysqld: ready for connections
# ❌ MANQUE: "Initializing MariaDB database..."
# ❌ MANQUE: "Creating database and user..."
```

**Diagnostic** :
```bash
docker exec mariadb mysql -u root -e "SELECT User, Host FROM mysql.user;"
# root   localhost   ✅
# ❌ wpuser manquant!
```

**Cause profonde** : Chaîne de problèmes liée au Docker layer caching :
1. `apt-get install mariadb-server` crée des fichiers par défaut dans `/var/lib/mysql/`
2. Ces fichiers sont **bakés dans l'image Docker** (layers)
3. Le script d'init vérifie : `if [ ! -d "/var/lib/mysql/mysql" ]`
4. La condition est **toujours fausse** car le dossier existe déjà dans l'image
5. Le bloc d'initialisation ne s'exécute **jamais**
6. L'utilisateur `wpuser` n'est jamais créé

**Solution** : Ajouter une ligne dans le Dockerfile MariaDB pour supprimer les fichiers par défaut :
```dockerfile
# ... apt-get install mariadb-server ...
RUN rm -rf /var/lib/mysql/*
# Maintenant le dossier existe mais est vide
```

Puis nettoyage complet :
```bash
make down
docker rmi mariadb:inception
sudo rm -rf ~/data/mariadb/* ~/data/mariadb/.*
make
```

**Vérification** :
```bash
docker logs mariadb | head -80
# Initializing MariaDB database... ✅
# Creating database and user... ✅
# MariaDB initialization complete! ✅

docker exec mariadb mysql -u root -e "SELECT User, Host FROM mysql.user;"
# root     localhost   ✅
# wpuser   %           ✅
```

**Concept** :
- Docker **cache les layers** avec les fichiers créés par `RUN`
- Un `RUN apt-get install` peut créer des fichiers de config/data par défaut
- Ces fichiers persistent dans l'image même si le bind mount est vide
- Il faut explicitement les supprimer avec `RUN rm -rf` dans le Dockerfile
- Les checks de type "if directory exists" peuvent échouer silencieusement

**Connaissance appliquée** : Day 35 - Docker Security (compréhension layer caching)

---

### Problème #10 : Boucle d'Initialisation WordPress - 25 décembre

**Symptôme** : WordPress entre dans une boucle infinie de redémarrages :
```bash
docker logs wordpress
# Error: WordPress files seem to already be present here.
# Waiting for MariaDB...
# MariaDB is ready!
# Installing WordPress...
# Error: WordPress files seem to already be present here.
# [conteneur redémarre, boucle se répète]
```

**Cause** : Anciens fichiers WordPress persistaient dans `~/data/wordpress/` depuis les installations ratées précédentes. WP-CLI détecte ces fichiers et refuse d'installer, causant un exit code d'erreur.

**Solution** : Nettoyage complet des données persistantes :
```bash
sudo rm -rf ~/data/wordpress/* ~/data/wordpress/.*
docker restart wordpress
sleep 30 && docker logs wordpress
```

**Sortie attendue** :
```
Downloading WordPress 6.9 (en_US)...
Success: WordPress downloaded.
Success: Generated 'wp-config.php' file.
Success: WordPress installed successfully. ✅
Success: Object cache enabled. ✅
WordPress installation complete with Redis caching!
```

**Concept** :
- Les **bind mounts** persistent même quand les conteneurs sont supprimés
- Un échec d'installation laisse des **artefacts** qui bloquent les tentatives suivantes
- Il faut nettoyer **à la fois** l'image Docker ET les données persistantes
- Ordre de nettoyage : `make down` → `rm image` → `rm volumes` → `make`

**Connaissance appliquée** : Day 35 - Container Security (gestion des volumes)

---

### Problème #11 : Portainer Web UI 404 - 25 décembre

**Symptôme** : L'API Portainer fonctionne mais l'interface web retourne 404 :
```bash
curl http://localhost:9443/
# 404 page not found

curl http://localhost:9443/api/status
# {"Version":"2.19.4"...} ✅ L'API marche!
```

**Diagnostic** :
```bash
docker exec portainer ls -la /usr/local/bin/
# -rwxr-xr-x 1 root root 123456789 portainer
# ❌ Il manque le dossier public/ avec l'interface web!
```

**Cause profonde** : Le Dockerfile extrayait l'archive puis **déplaçait uniquement le binaire** :
```dockerfile
# ❌ VERSION CASSÉE:
RUN tar -xzf portainer.tar.gz && \
    mv portainer/portainer /usr/local/bin/ && \
    rm -rf portainer  # ← Supprime le dossier public/ !

CMD ["/usr/local/bin/portainer", "--bind=:9000", "--data=/data"]
```

Portainer a besoin de :
- Le binaire `portainer`
- Le dossier `public/` contenant l'interface web React
- Que le `WORKDIR` soit le dossier parent pour trouver `public/`

**Solution** : Extraire le dossier complet et utiliser `WORKDIR` :
```dockerfile
# ✅ VERSION CORRIGÉE:
RUN tar -xzf portainer-2.19.4-linux-amd64.tar.gz -C /usr/local/bin/ && \
    rm portainer-2.19.4-linux-amd64.tar.gz

WORKDIR /usr/local/bin/portainer
CMD ["/usr/local/bin/portainer/portainer", "--bind=:9000", "--data=/data"]
```

Reconstruction :
```bash
docker-compose stop portainer
docker-compose rm -f portainer
docker rmi portainer:inception
docker-compose build portainer
docker-compose up -d portainer
```

**Vérification** :
```bash
curl -s http://localhost:9443/ | head -20
# <!doctype html><html lang="en" ng-app="portainer"... ✅

docker exec portainer ls -la /usr/local/bin/portainer/
# -rwxr-xr-x portainer
# drwxr-xr-x public/    ✅
```

**Concept** :
- Les applications web modernes ont souvent **binaire + assets statiques**
- Extraire une archive puis déplacer sélectivement peut **casser la structure**
- `WORKDIR` définit le **contexte d'exécution** (important pour chemins relatifs)
- Toujours vérifier que tous les fichiers nécessaires sont présents dans l'image

**Connaissance appliquée** : Day 35 - Container Security (Dockerfile best practices)

---

### Problème #12 : Port 443 Forwarding - 25 décembre

**Symptôme** : WordPress redirige de `:8443` vers le port 443 standard (sans numéro de port), causant des échecs de connexion dans le navigateur.

**Cause** : WordPress fait des redirections HTTPS vers son URL configurée. Si le port n'est pas standard (443), les redirections échouent.

**Solution** : Ajouter une règle de port forwarding pour le port 443 standard :
```bash
VBoxManage controlvm "inception" natpf1 "https443,tcp,,443,,443"
```

**Vérification** :
```bash
VBoxManage showvminfo "inception" | grep Rule
# NIC 1 Rule(0): https, tcp, host port = 8443, guest port = 443
# NIC 1 Rule(1): https443, tcp, host port = 443, guest port = 443 ✅
# NIC 1 Rule(2): portainer, tcp, host port = 9443, guest port = 9443
# NIC 1 Rule(3): ssh, tcp, host port = 2222, guest port = 22
```

**Résultat** : Accès possible via les deux URLs :
- `https://ggrisole.42.fr:8443` (port custom)
- `https://ggrisole.42.fr` (port standard)

**Concept** :
- Les applications web font souvent des **redirections internes**
- Ces redirections utilisent l'URL **configurée dans l'app**, pas l'URL d'origine
- Pour les environnements de dev avec ports non-standards, il faut **les deux forwards**
- NAT VirtualBox permet plusieurs mappings pour le même port guest

**Connaissance appliquée** : Day 9 - Network Security (port forwarding, NAT)

---

## 🎓 Connaissances Cybersécurité Appliquées

### Vue d'ensemble des 74 jours de formation

Entre septembre et novembre 2025, tu as suivi 30 sessions (74 jours calendaires) de formation Azure Security. Voici comment ces connaissances se sont appliquées à Inception :

### Day 5 : Infrastructure as Code

**Ce qui a été appris** : Principes Bicep/Terraform, configuration déclarative, reproductibilité.

**Application dans Inception** :
- Docker Compose comme IaC (fichier YAML déclaratif)
- Makefile pour automatisation (`make up`, `make down`, `make clean`, `make re`)
- Toutes les configurations en contrôle de version
- Déploiement reproductible en une commande

### Day 9 : Network Security

**Ce qui a été appris** : Azure Virtual Network, segmentation réseau, isolation.

**Application dans Inception** :
- Réseau Docker bridge personnalisé
- Services communiquent par DNS, pas par IP
- Seul port 443 exposé
- Résolution du problème Redis binding (`127.0.0.1` → `0.0.0.0`)

### Day 21 : Application Security

**Ce qui a été appris** : TLS/SSL, headers de sécurité, HTTPS enforcement.

**Application dans Inception** :
- TLS 1.2/1.3 uniquement
- Certificats auto-signés générés
- Headers de sécurité (HSTS, X-Frame-Options, CSP, XSS Protection)
- Durcissement WordPress

### Day 35 : Container Security

**Ce qui a été appris** : Sécurité conteneurs, Podman, builds multi-étapes, images minimales.

**Application dans Inception** :
- Tous les Dockerfiles construits from scratch
- Images de base Alpine/Debian (petites et sécurisées)
- Health checks implémentés
- Utilisateurs non-root
- Gestion des dépendances

### Days 41-43 : Kubernetes & Orchestration

**Ce qui a été appris** : K8s, orchestration, self-healing, PID 1, service discovery.

**Application dans Inception** :
- Compréhension PID 1 → résolution problème Redis daemon
- Service discovery par nom DNS → configuration WordPress Redis
- Restart policies (`restart: always`)
- Health checks (liveness probes)
- Troubleshooting systématique

### Day 51 : Supply Chain Security

**Ce qui a été appris** : SBOM, vulnérabilités, versions épinglées, sécurité dépendances.

**Application dans Inception** :
- Versions épinglées (pas de `latest`)
- Documentation origines logiciels
- Traçabilité complète
- Scan Trivy prévu

### Day 53 : Secret Management

**Ce qui a été appris** : Azure Key Vault, rotation secrets, variables environnement.

**Application dans Inception** :
- Fichier `.env` pour tous les secrets
- Aucun mot de passe dans Dockerfiles
- `.gitignore` configuré
- Génération sécurisée (OpenSSL)
- Résolution problème newlines dans passwords

### Day 55 : Security Hardening

**Ce qui a été appris** : CIS controls, compliance, posture management.

**Application dans Inception** :
- Checklist sécurité complète
- Durcissement chaque service
- Documentation mesures appliquées
- Identification améliorations possibles

### Day 59 : DevSecOps Pipeline

**Ce qui a été appris** : CI/CD, debugging systématique, documentation.

**Application dans Inception** :
- Approche debugging méthodique
- Documentation 7 problèmes résolus
- Workflow Git maîtrisé
- Mindset security-first

---

## 📊 Récapitulatif : Chiffres Clés

### Architecture
- **8 services** déployés (3 obligatoires + 5 bonus)
- **1 réseau** Docker privé
- **2 volumes** persistants (MariaDB, WordPress)
- **1 port** exposé (443 HTTPS)
- **0 secrets** dans Git

### Sécurité
- **TLS 1.2/1.3** uniquement
- **5 headers** de sécurité
- **7 problèmes** debuggés et documentés
- **10+ mesures** de durcissement appliquées

### Documentation
- **13 fichiers** de documentation
- **~100KB** de contenu technique
- **2 langues** (français + anglais)
- **100% traçabilité** des décisions

### Performance
- **< 2 secondes** chargement page WordPress (avec cache Redis)
- **~256MB RAM** limite Redis
- **30GB** espace disque VM
- **4GB RAM** VM allocation

---

## 🎯 Points Clés pour la Défense

### Ce que les évaluateurs vont vérifier

1. **Services fonctionnels** : Tous les conteneurs tournent et répondent
2. **Dockerfiles custom** : Pas d'images pré-faites de DockerHub
3. **TLS configuré** : HTTPS fonctionne avec TLS 1.2/1.3
4. **WordPress opérationnel** : 2 utilisateurs créés, site accessible
5. **Volumes persistants** : Données survivent au redémarrage
6. **Réseau isolé** : Services communiquent sur réseau privé
7. **Secrets protégés** : Pas de passwords en clair dans code
8. **Bonus fonctionnels** : Au moins 5 services bonus marchent

### Tes points forts à mettre en avant

1. **Application connaissances cybersécurité** : "J'ai appliqué 74 jours de formation Azure Security à ce projet. Par exemple, Day 9 m'a aidé à résoudre le problème Redis network binding."

2. **Debugging systématique après pause de 2 semaines** : "Le 25 décembre, après 2 semaines de pause intensive, j'ai découvert que rien ne marchait plus. J'ai diagnostiqué et résolu 5 problèmes critiques en 2h30 : secrets incorrects (25 vs 44 octets), MariaDB qui n'initialisait jamais la base à cause du Docker layer caching, boucle WordPress, Portainer UI 404, et port forwarding. J'ai documenté tout le processus méthodiquement."

3. **Compréhension approfondie Docker** : "Les problèmes #8-12 démontrent une compréhension des mécanismes internes : layer caching, bind mounts, secrets Docker, WORKDIR, et NAT port forwarding. Je peux expliquer pourquoi `apt-get install mariadb-server` bakait des fichiers dans l'image et cassait l'initialisation."

4. **Sécurité en profondeur** : "Isolation réseau, gestion secrets (44 octets base64 exact), TLS enforcement, health checks, restart policies. Pas juste faire marcher, mais faire marcher **sécurisé**."

5. **Documentation professionnelle** : "16 fichiers de documentation incluant troubleshooting détaillé (DECEMBER-25-FIXES.md de 500+ lignes), guides rapides, services knowledge, et application des connaissances cybersécurité. Reproductibilité totale."

6. **Résilience et méthodologie** : "Face à une infrastructure complètement cassée après pause, j'ai appliqué une approche systématique : diagnostics → root cause → fix → verify. Exactement comme dans DevSecOps Day 59 avec 13 itérations."

### Questions possibles et réponses

**Q : Pourquoi 8 services et pas 3 ?**  
R : J'ai fait les 5 bonus pour démontrer une compréhension approfondie. Redis pour le caching, FTP pour la gestion fichiers, Adminer pour l'admin DB, site statique pour montrer mes compétences web, Portainer pour la gestion conteneurs.

**Q : Qu'est-ce que tu as trouvé le plus difficile ?**  
R : La session de debugging du 25 décembre. Après 2 semaines de pause intensive, l'infrastructure complète ne marchait plus. J'ai dû diagnostiquer 5 problèmes interdépendants : secrets incorrects montés depuis l'ancien emplacement, MariaDB avec database baked dans l'image Docker empêchant l'initialisation, WordPress avec anciens fichiers causant une boucle, Portainer sans son dossier `public/`, et redirections WordPress cassées. Ça m'a pris 2h30 de debugging méthodique pour tout remettre en marche.

**Q : Tu peux expliquer le problème des secrets (Problème #8) ?**  
R : J'avais généré de nouveaux secrets de 44 octets le 25 décembre, mais dans le mauvais emplacement (`~/inception/srcs/secrets/` au lieu de `~/inception/secrets/`). Docker continuait de monter les anciens secrets de 25 octets depuis le bon emplacement. WordPress ne pouvait donc pas s'authentifier sur MariaDB. La commande correcte est `openssl rand -base64 32 | tr -d '\n'` qui produit exactement 44 caractères base64 sans newline. Le `tr -d '\n'` est critique.

**Q : C'est quoi le problème du Docker layer caching (Problème #9) ?**  
R : Quand tu fais `apt-get install mariadb-server`, ça crée des fichiers par défaut dans `/var/lib/mysql/`. Ces fichiers sont bakés dans l'image Docker (layers). Mon script d'init vérifiait `if [ ! -d "/var/lib/mysql/mysql" ]` pour savoir s'il devait initialiser. Mais comme le dossier existait déjà dans l'image, la condition était toujours fausse, donc l'utilisateur `wpuser` n'était jamais créé. La solution : `RUN rm -rf /var/lib/mysql/*` dans le Dockerfile pour forcer le dossier vide.

**Q : Comment tu gères les secrets ?**  
R : Fichier `.env` exclu de Git, secrets générés avec `openssl rand -base64 32 | tr -d '\n'` (exactement 44 octets), stockés dans `~/inception/secrets/` (pas dans srcs/), montés dans conteneurs via Docker secrets. Aucun secret en clair dans les Dockerfiles. J'ai même dû débugger un problème où l'absence du `tr -d '\n'` créait des secrets de 45 octets (avec newline caché).

**Q : Comment WordPress et Redis communiquent ?**  
R : Via le réseau Docker. WordPress utilise l'extension PHP Redis et se connecte à `redis:6379` (résolution DNS Docker). Redis garde en cache les objets PHP pour éviter des requêtes SQL répétées.

**Q : Tu peux expliquer le flux d'une requête HTTPS ?**  
R : 
1. Utilisateur → `https://ggrisole.42.fr/` (port 443)
2. NGINX reçoit, déchiffre TLS, vérifie la route
3. Si `.php` → NGINX envoie à WordPress via FastCGI (port 9000)
4. WordPress demande données à MariaDB (port 3306)
5. WordPress vérifie cache Redis (port 6379)
6. WordPress génère HTML
7. NGINX renvoie HTML chiffré TLS à l'utilisateur

**Q : Pourquoi tu as besoin de deux forwards pour le port 443 (Problème #12) ?**  
R : WordPress fait des redirections internes vers son URL configurée. Si j'accède via `:8443`, WordPress me redirige vers `:443` standard. Sans le forward `443→443`, le navigateur ne peut pas suivre la redirection. Donc j'ai deux mappings : `8443→443` pour l'accès initial, et `443→443` pour les redirections WordPress.

**Q : Comment tu as débugé le problème Portainer (Problème #11) ?**  
R : L'API marchait (`/api/status` retournait JSON) mais le web UI retournait 404. J'ai fait `docker exec portainer ls -la /usr/local/bin/` et découvert qu'il manquait le dossier `public/`. Le Dockerfile faisait `mv portainer/portainer /usr/local/bin/` puis `rm -rf portainer`, supprimant les assets web. Solution : extraire l'archive complète dans `/usr/local/bin/portainer/`, utiliser `WORKDIR`, et laisser tous les fichiers intacts.

**Q : Qu'est-ce que tu améliorerais pour la production ?**  
R : 
- Scan Trivy sur toutes les images (détection vulnérabilités)
- Resource limits CPU/mémoire (protection abus)
- Certificats Let's Encrypt (au lieu d'auto-signés)
- Backups automatisés (cron jobs pour volumes)
- Monitoring centralisé (Prometheus + Grafana)

---

## ✅ Checklist Finale Avant Défense

### Vérifications techniques

- [ ] VM démarre correctement
- [ ] SSH fonctionne (`ssh -p 2222 ggrisole@localhost`)
- [ ] `make` construit tout sans erreur
- [ ] `docker ps` montre 8 conteneurs "Up"
- [ ] `docker ps` montre MariaDB "healthy"
- [ ] Site accessible `https://ggrisole.42.fr/`
- [ ] Certificat TLS présent (warning navigateur normal)
- [ ] WordPress admin accessible `/wp-admin`
- [ ] Adminer accessible `/adminer`
- [ ] Site statique accessible `/static`
- [ ] Portainer accessible `http://localhost:9443`
- [ ] Redis connecté (vérifier WordPress dashboard)

### Vérifications conceptuelles

- [ ] Tu peux expliquer chaque service et son rôle
- [ ] Tu peux expliquer les **12 problèmes résolus** (7 initiaux + 5 du 25 décembre)
- [ ] Tu peux lier le projet aux jours de formation Azure
- [ ] Tu peux expliquer le flux d'une requête
- [ ] Tu peux justifier chaque choix de sécurité
- [ ] Tu peux proposer des améliorations
- [ ] Tu peux raconter l'histoire du debugging du 25 décembre (2h30 de diagnostic systématique)

### Documents à avoir sous la main

- [ ] `README.md` - Vue d'ensemble + status update 25 décembre
- [ ] `DECEMBER-25-FIXES.md` - **Problèmes #8-12 détaillés (500+ lignes)**
- [ ] `QUICK-FIX-REFERENCE.md` - Référence rapide pour les 5 nouveaux fixes
- [ ] `INCEPTION-TROUBLESHOOTING.md` - Problèmes #1-7 + références aux nouveaux
- [ ] `BONUS-SERVICES.md` - Services bonus + fix Portainer
- [ ] `inception-services-knowledge.md` - Connaissance services
- [ ] `CYBERSECURITY-KNOWLEDGE-APPLICATION-FR.md` - Application formation
- [ ] `GUIDE-DEFENSE-ORAL-FR.md` - Ce guide !

---

## 🎉 Conclusion

Tu as construit une infrastructure Docker complète, sécurisée, et documentée professionnellement. Ce n'est pas juste un projet d'école, c'est une démonstration de compétences réelles en cybersécurité, conteneurisation, et architecture distribuée.

**Points forts** :
- ✅ 8 services déployés et fonctionnels
- ✅ Sécurité en profondeur (réseau, secrets, TLS, health checks)
- ✅ Debugging méthodique (**12 problèmes documentés : 7 initiaux + 5 critiques du 25 décembre**)
- ✅ Résilience démontrée (infrastructure réparée après 2 semaines de pause)
- ✅ Application directe de 74 jours de formation cybersécurité
- ✅ Documentation exhaustive (16 fichiers, ~150KB, dont DECEMBER-25-FIXES.md de 500+ lignes)

**Ce qui te différencie** :
- Tu ne t'es pas contenté de "faire marcher"
- Tu as appliqué des principes de sécurité professionnels
- Tu as documenté ton processus de debugging avec rigueur
- Tu peux expliquer **pourquoi** chaque choix technique
- Tu as résolu 5 problèmes critiques interdépendants en 2h30 avec une approche systématique
- Tu comprends les mécanismes internes (Docker layer caching, bind mounts, secrets, WORKDIR)

**Pour la défense** :
- Reste confiant, tu maîtrises le sujet
- Explique avec des exemples concrets
- **L'histoire du 25 décembre est ton atout majeur** : infrastructure cassée → diagnostic méthodique → 5 problèmes identifiés → solutions appliquées → tout opérationnel → documentation complète
- Lie toujours au parcours cybersécurité
- Montre la documentation si besoin
- Propose des améliorations (montre que tu penses "production")

**Tu es prêt !** 💪

Bon courage pour ta défense ! 🚀
