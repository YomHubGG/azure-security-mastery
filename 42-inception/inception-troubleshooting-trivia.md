# Inception - Trivia des Difficultés Rencontrées

**Date**: 30 novembre 2025  
**Projet**: 42 Inception - Déploiement de 8 services Docker  
**Résultat**: ✅ Tous les services opérationnels

---

## 📋 Vue d'Ensemble

Ce document recense tous les problèmes techniques rencontrés lors du déploiement du projet Inception, leurs causes profondes et leurs solutions. Chaque difficulté illustre un concept important en containerisation et networking.

---

## 🔴 Problème #1: Redis Container Exits Immediately

### **Contexte**
Lors du premier déploiement du service bonus Redis, le conteneur se créait avec succès mais s'arrêtait immédiatement avec un exit code 0.

### **Symptômes**
```bash
$ docker ps
# Redis absent de la liste

$ docker ps -a
NAMES    STATUS                     
redis    Exited (0) 2 seconds ago
```

### **Cause Racine**
Redis était configuré en mode daemon (`daemonize yes` dans redis.conf). En mode daemon, le processus principal se détache et retourne immédiatement, laissant le conteneur sans processus foreground actif.

**Principe Docker**: Un conteneur Docker ne reste actif que tant que son processus principal (PID 1) est en exécution. Si le processus se termine ou se détache, le conteneur s'arrête.

### **Solution Appliquée**
Modification du Dockerfile Redis pour forcer le mode foreground:
```dockerfile
CMD ["redis-server", "/etc/redis/redis.conf", "--daemonize", "no"]
```

Configuration redis.conf:
```conf
daemonize no
```

### **Concepts Clés**
- **PID 1**: Le processus principal d'un conteneur doit rester en foreground
- **Daemon vs Foreground**: Les services en container doivent tourner en mode foreground
- **Exit Code 0**: Sortie propre ≠ erreur, mais indique terminaison du processus

---

## 🔴 Problème #2: Docker Compose Version Incompatibility

### **Contexte**
Tentative d'utiliser des commandes docker-compose v2 sur une VM équipée de docker-compose v1.

### **Symptômes**
```bash
$ docker compose -f docker-compose.yml up -d
unknown shorthand flag: 'f' in -f
```

### **Cause Racine**
La VM Debian 12 utilisait `docker-compose` v1 (commande avec trait d'union), pas la version v2 intégrée à Docker CLI (commande `docker compose` sans trait d'union).

**Différences**:
- **v1**: `docker-compose` (binaire séparé Python)
- **v2**: `docker compose` (plugin Docker Go)

### **Solution Appliquée**
Utilisation systématique de la syntaxe v1:
```bash
docker-compose up -d --build
# Au lieu de: docker compose up -d --build
```

### **Concepts Clés**
- **Backward Compatibility**: Toujours vérifier la version disponible avant d'écrire des scripts
- **CLI Evolution**: Docker Compose a migré de Python vers Go et s'est intégré à Docker CLI
- **Environment Detection**: Adapter les commandes à l'environnement cible

---

## 🔴 Problème #3: Portainer Restart Loop

### **Contexte**
Le conteneur Portainer se construisait correctement mais entrait dans une boucle de redémarrage infinie avec exit code 1.

### **Symptômes**
```bash
$ docker ps
NAMES       STATUS
portainer   Restarting (1) 8 seconds ago

$ docker logs portainer
FTL failed initializing compose deployer | error="docker-compose plugin is missing from config path"
```

### **Cause Racine**
Portainer v2.19.4 cherchait automatiquement le binaire `docker-compose` pour initialiser son module "compose deployer". Sans ce binaire, Portainer refusait de démarrer.

**Architecture**: Portainer nécessite docker-compose pour pouvoir déployer/gérer des stacks Docker Compose via son interface web.

### **Solution Appliquée**
Installation de docker-compose v1.29.2 dans le conteneur Portainer:

```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    ca-certificates \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Installation de docker-compose (requis par Portainer)
RUN curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose && \
    chmod +x /usr/local/bin/docker-compose

# Installation du binaire Portainer
RUN wget https://github.com/portainer/portainer/releases/download/2.19.4/portainer-2.19.4-linux-amd64.tar.gz && \
    tar -xzf portainer-2.19.4-linux-amd64.tar.gz && \
    mv portainer/portainer /usr/local/bin/ && \
    rm -rf portainer portainer-2.19.4-linux-amd64.tar.gz

VOLUME /data
EXPOSE 9000

CMD ["/usr/local/bin/portainer", "--bind=:9000", "--data=/data"]
```

### **Concepts Clés**
- **Dependency Hell**: Un binaire peut avoir des dépendances runtime non documentées
- **Container Dependencies**: Même les outils de gestion de containers ont des dépendances
- **Fatal Errors**: Les erreurs FTL (Fatal) entraînent un crash immédiat
- **Restart Policies**: Docker restart automatiquement les conteneurs qui crashent

---

## 🔴 Problème #4: Redis Network Binding Issue

### **Contexte**
Le conteneur Redis démarrait correctement, mais WordPress ne pouvait pas s'y connecter malgré être sur le même réseau Docker.

### **Symptômes**
```bash
# Depuis le conteneur WordPress
$ redis-cli -h redis ping
Could not connect to Redis at redis:6379: Connection refused

# Depuis le conteneur Redis lui-même
$ redis-cli ping
PONG  # ✓ Fonctionne localement
```

### **Diagnostic Approfondi**
```bash
$ docker exec redis cat /etc/redis/redis.conf | grep "^bind"
bind 127.0.0.1 -::1  # ❌ Écoute uniquement sur localhost
```

### **Cause Racine**
Redis était configuré pour écouter uniquement sur l'interface loopback (127.0.0.1), ce qui signifie:
- ✅ Accessible depuis l'intérieur du conteneur
- ❌ Inaccessible depuis d'autres conteneurs du réseau Docker

**Networking Docker**: Chaque conteneur a sa propre pile réseau. Pour communiquer entre conteneurs, les services doivent écouter sur `0.0.0.0` (toutes les interfaces), pas seulement sur `127.0.0.1`.

### **Tentatives Infructueuses**

#### Tentative #1: Utilisation de sed
```dockerfile
RUN sed -i 's/bind 127.0.0.1 ::1/bind 0.0.0.0/' /etc/redis/redis.conf
```
**Échec**: Le pattern ne correspondait pas exactement à la ligne dans le fichier (espaces, format IPv6).

#### Tentative #2: Multiples sed
```dockerfile
RUN sed -i 's/bind 127.0.0.1 -::1/bind 0.0.0.0/' /etc/redis/redis.conf && \
    sed -i 's/protected-mode yes/protected-mode no/' /etc/redis/redis.conf
```
**Échec**: Les transformations sed ne s'appliquaient pas correctement à toutes les variantes de syntaxe.

### **Solution Finale**
Création d'un fichier de configuration Redis complet et explicite:

**redis.conf**:
```conf
# Network
bind 0.0.0.0           # Écoute sur toutes les interfaces
protected-mode no      # Désactive le mode protégé (OK pour réseau Docker privé)
port 6379

# General
daemonize no           # Mode foreground pour Docker
pidfile /var/run/redis/redis-server.pid
loglevel notice

# Snapshotting - disabled for pure cache
save ""                # Pas de persistence sur disque
appendonly no          # Pas de log AOF

# Memory
maxmemory 256mb
maxmemory-policy allkeys-lru  # Éviction LRU pour cache
```

**Dockerfile**:
```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    redis-server \
    && rm -rf /var/lib/apt/lists/*

# Copie de notre configuration custom
COPY conf/redis.conf /etc/redis/redis.conf

EXPOSE 6379

CMD ["redis-server", "/etc/redis/redis.conf"]
```

### **Concepts Clés**
- **Network Binding**: `127.0.0.1` = localhost uniquement, `0.0.0.0` = toutes les interfaces
- **Docker Networking**: Les conteneurs communiquent via le réseau bridge, pas via localhost
- **Protected Mode**: Redis refuse les connexions externes par défaut en mode protégé
- **Configuration Management**: Remplacer un fichier complet > modifications avec sed/awk
- **DNS Docker**: Les conteneurs se résolvent par leur nom de service (ex: `redis`)

---

## 🔴 Problème #5: WordPress Redis Configuration

### **Contexte**
Après avoir corrigé le binding Redis, WordPress ne parvenait toujours pas à activer le cache Redis.

### **Symptômes**
```bash
$ wp redis enable --allow-root
Error: Object cache could not be enabled. Redis server is unreachable: Connection refused [tcp://127.0.0.1:6379]
```

**Observation**: WordPress tentait de se connecter à `127.0.0.1:6379` au lieu de `redis:6379`.

### **Cause Racine**
La configuration WordPress (wp-config.php) ne contenait pas les constantes PHP définissant l'hôte Redis. Le plugin Redis Object Cache utilisait donc ses valeurs par défaut:
- Host: `127.0.0.1` (défaut)
- Port: `6379` (défaut)

### **Solution Appliquée**
Ajout des constantes Redis dans wp-config.php via WP-CLI:

```bash
wp config set WP_REDIS_HOST redis --allow-root
wp config set WP_REDIS_PORT 6379 --raw --allow-root
wp config set WP_REDIS_TIMEOUT 1 --raw --allow-root
wp config set WP_REDIS_READ_TIMEOUT 1 --raw --allow-root
wp config set WP_CACHE true --raw --allow-root
```

**Résultat dans wp-config.php**:
```php
define( 'WP_REDIS_HOST', 'redis' );
define( 'WP_REDIS_PORT', 6379 );
define( 'WP_REDIS_TIMEOUT', 1 );
define( 'WP_REDIS_READ_TIMEOUT', 1 );
define( 'WP_CACHE', true );
```

Puis activation:
```bash
$ wp redis enable --allow-root
Success: Object cache enabled.

$ wp redis status --allow-root
Status: Connected
Client: PhpRedis (v5.3.7)
Redis Version: 7.0.15
```

### **Concepts Clés**
- **Service Discovery**: Dans Docker, les services se trouvent par leur nom (DNS interne)
- **Default Values**: Toujours vérifier les valeurs par défaut des applications
- **PHP Constants**: WordPress utilise des constantes pour la configuration runtime
- **Plugin Configuration**: Les plugins WordPress peuvent nécessiter une config externe

---

## 🔴 Problème #6: WordPress Dependencies (Redis Tools)

### **Contexte**
Le script de setup WordPress tentait de tester la connexion Redis avec `redis-cli`, mais la commande n'existait pas.

### **Symptômes**
```bash
$ docker logs wordpress
bash: line 1: redis-cli: command not found
```

### **Cause Racine**
Le Dockerfile WordPress installait PHP et ses extensions, mais pas les outils client Redis (`redis-tools` package).

### **Solution Appliquée**
Ajout de `redis-tools` et de l'extension PHP Redis:

```dockerfile
FROM debian:bookworm

RUN apt-get update && apt-get install -y \
    php8.2-fpm \
    php8.2-mysql \
    php8.2-curl \
    php8.2-gd \
    php8.2-mbstring \
    php8.2-xml \
    php8.2-zip \
    php8.2-redis \      # Extension PHP pour Redis
    curl \
    mariadb-client \
    redis-tools \       # Client Redis (redis-cli)
    && rm -rf /var/lib/apt/lists/*
```

### **Concepts Clés**
- **Client vs Server**: redis-server ≠ redis-cli (packages séparés)
- **PHP Extensions**: php8.2-redis fournit l'API PHP, pas les outils CLI
- **Debugging Tools**: Inclure les outils de débogage facilite le troubleshooting
- **Package Dependencies**: Anticiper les dépendances nécessaires pour les scripts

---

## 🔴 Problème #7: NGINX Reverse Proxy Configuration

### **Contexte**
Les services bonus (Adminer, Static Site) fonctionnaient en interne mais n'étaient pas accessibles via NGINX.

### **Solution Appliquée**
Ajout de locations proxy dans la configuration NGINX:

```nginx
server {
    listen 443 ssl;
    server_name ggrisole.42.fr;
    
    # ... config SSL ...
    
    # Adminer - Database Management UI (BONUS)
    location /adminer {
        proxy_pass http://adminer:9001;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
    
    # Static Site Showcase (BONUS)
    location /static {
        proxy_pass http://static-site:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
    
    # WordPress (default)
    location / {
        try_files $uri $uri/ /index.php?$args;
    }
}
```

### **Concepts Clés**
- **Reverse Proxy**: NGINX route les requêtes vers les services internes
- **Location Blocks**: Pattern matching pour router différents paths
- **Proxy Headers**: Préserver l'IP client et le protocole original
- **Service Names**: Docker DNS permet d'utiliser les noms de services

---

## 📊 Récapitulatif des Compétences Démontrées

### 🐳 Docker & Containerisation
- Compréhension des processus foreground vs daemon en containers
- Gestion du cycle de vie des conteneurs (build, run, restart policies)
- Troubleshooting des conteneurs qui crashent
- Optimisation des Dockerfiles (layers, cache)
- Gestion des dépendances runtime

### 🌐 Networking
- Maîtrise du networking Docker (bridge networks)
- Binding d'interfaces réseau (0.0.0.0 vs 127.0.0.1)
- DNS interne Docker (service discovery)
- Configuration de reverse proxy NGINX
- Exposition sélective des ports

### 🔧 Configuration Management
- Gestion de fichiers de configuration complexes
- Préférence fichiers complets vs modifications inline (sed/awk)
- Configuration d'applications multi-tiers
- Variables d'environnement et secrets

### 🐛 Debugging & Troubleshooting
- Analyse de logs Docker (`docker logs`)
- Inspection de conteneurs (`docker exec`, `docker inspect`)
- Tests de connectivité réseau
- Diagnostic de problèmes de binding/listening
- Méthodologie: isoler le problème, tester l'hypothèse, vérifier la solution

### 🗄️ Services & Applications
- Configuration Redis (networking, memory management, persistence)
- Intégration WordPress + Redis Object Cache
- Configuration Portainer avec dépendances
- Setup NGINX comme reverse proxy
- Gestion de services FTP, Adminer, static sites

---

## 💡 Leçons Apprises

1. **Toujours vérifier les bindings réseau**: Un service qui fonctionne localement peut être inaccessible en réseau
2. **Les processus Docker doivent être en foreground**: Jamais de daemonization dans les containers
3. **Lire les logs en entier**: Les vraies erreurs sont souvent noyées dans l'output
4. **Tester la connectivité à chaque niveau**: Host → Container → Container-to-Container
5. **Les valeurs par défaut sont rarement adaptées**: Toujours configurer explicitement
6. **Inclure les outils de débogage**: redis-cli, ping, netstat facilitent le troubleshooting
7. **Documenter les problèmes**: Chaque erreur enseigne quelque chose d'important

---

## 🎯 Résultat Final

**8 services opérationnels** avec toutes les difficultés résolues:
- ✅ NGINX (reverse proxy HTTPS)
- ✅ WordPress (PHP-FPM + Redis cache)
- ✅ MariaDB (base de données)
- ✅ Redis (object cache - binding corrigé)
- ✅ Adminer (interface DB)
- ✅ Static Site (vitrine HTML/CSS)
- ✅ FTP (accès fichiers)
- ✅ Portainer (gestion containers - docker-compose installé)

**Tous les prérequis 42 respectés** + **5 services bonus fonctionnels**.

---

*Document créé le 30 novembre 2025*  
*Projet: 42 Inception*  
*Auteur: @YomHubGG*
