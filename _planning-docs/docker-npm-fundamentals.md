# 🐳 Docker & npm Fundamentals - Quick Reference

**Created**: November 14, 2025  
**Purpose**: Preparation for 42 Inception project + Docker mastery  
**Context**: Day 59 Q2 Capstone work revealed need for deeper Docker understanding

---

## 📦 npm: Node Package Manager

### **`npm install` vs `npm ci`**

| Aspect | `npm install` | `npm ci` |
|--------|---------------|----------|
| **Purpose** | Install packages for development | Install exact versions for CI/CD |
| **Speed** | Slower (resolves dependencies) | Faster (uses lockfile directly) |
| **Lockfile** | Updates `package-lock.json` | Requires existing lockfile, doesn't modify it |
| **node_modules** | May reuse existing folder | Deletes and recreates from scratch |
| **Use Case** | Local development | CI/CD pipelines, production builds |
| **Behavior** | Installs compatible versions (^, ~) | Installs EXACT versions from lockfile |

### **When to Use Each**

```bash
# Development (local machine)
npm install                    # Install all dependencies
npm install express           # Add new package
npm install --save-dev jest   # Add dev dependency

# CI/CD / Production (automated builds)
npm ci                        # Clean, reproducible install
npm ci --only=production      # Skip devDependencies
```

### **Why `npm ci` in Dockerfiles?**

```dockerfile
# ❌ BAD - Non-reproducible, slower
RUN npm install

# ✅ GOOD - Reproducible, faster, safer
COPY package*.json ./
RUN npm ci --only=production
```

**Benefits in Docker**:
1. ✅ **Reproducibility**: Same packages every build
2. ✅ **Speed**: 2-3x faster than `npm install`
3. ✅ **Safety**: Fails if lockfile is missing or out of sync
4. ✅ **Clean**: Always fresh install, no cache issues

---

## 🐳 Dockerfile Fundamentals

### **Basic Structure**

```dockerfile
# 1. BASE IMAGE - Starting point
FROM node:18-alpine

# 2. METADATA - Who maintains this
LABEL maintainer="you@example.com"

# 3. ENVIRONMENT - Set variables
ENV NODE_ENV=production
ENV PORT=3000

# 4. WORKING DIRECTORY - Where commands run
WORKDIR /app

# 5. COPY FILES - Add code to image
COPY package*.json ./
COPY . .

# 6. RUN COMMANDS - Execute during build
RUN npm ci --only=production

# 7. EXPOSE PORTS - Document which ports are used
EXPOSE 3000

# 8. USER - Switch to non-root user
USER node

# 9. CMD - Default command when container starts
CMD ["node", "server.js"]
```

---

## 🔑 Key Dockerfile Concepts

### **1. Layers & Caching**

Each instruction creates a layer. Docker caches layers to speed up builds.

```dockerfile
# ❌ BAD - Changes to code invalidate all layers below
COPY . .
RUN npm ci

# ✅ GOOD - Package files cached separately from code
COPY package*.json ./
RUN npm ci
COPY . .
```

**Why?** If only code changes, npm dependencies don't need reinstalling!

---

### **2. Multi-Stage Builds**

Build in one stage, copy artifacts to smaller final image.

```dockerfile
# Stage 1: BUILD
FROM node:18 AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npm run build

# Stage 2: PRODUCTION
FROM node:18-alpine
WORKDIR /app
COPY --from=builder /app/dist ./dist
COPY --from=builder /app/node_modules ./node_modules
COPY package*.json ./
USER node
CMD ["node", "dist/server.js"]
```

**Benefits**:
- ✅ Smaller final image (no build tools)
- ✅ Faster deploys
- ✅ More secure (fewer packages)

---

### **3. Security Hardening**

```dockerfile
# ❌ INSECURE
FROM node:latest                    # Unpinned version
RUN apt-get update && apt-get install -y curl
COPY . .
RUN npm install
EXPOSE 3000
CMD ["node", "server.js"]          # Running as root!

# ✅ SECURE
FROM node:18.20.4-alpine3.20       # Pinned version
RUN apk update && apk upgrade --no-cache  # Security updates
WORKDIR /app
COPY --chown=node:node package*.json ./
RUN npm ci --only=production && npm cache clean --force
COPY --chown=node:node . .
USER node                           # Non-root user
HEALTHCHECK --interval=30s --timeout=3s \
  CMD node -e "require('http').get('http://localhost:3000/health')"
EXPOSE 3000
CMD ["node", "server.js"]
```

---

## 🎯 Common Dockerfile Instructions Explained

### **FROM** - Base Image
```dockerfile
FROM node:18-alpine
# Format: image:tag
# alpine = minimal Linux (5MB vs 900MB for full Ubuntu)
```

### **WORKDIR** - Set Working Directory
```dockerfile
WORKDIR /app
# All subsequent commands run in /app
# Creates directory if it doesn't exist
```

### **COPY vs ADD**
```dockerfile
# COPY - Simple file copy (PREFERRED)
COPY package.json .
COPY src/ ./src/

# ADD - Copy + extract archives + download URLs
ADD https://example.com/file.tar.gz .  # Auto-extracts!
```

**Rule**: Always use `COPY` unless you need ADD's special features.

### **RUN** - Execute Commands During Build
```dockerfile
RUN npm ci
RUN apk add --no-cache curl
RUN useradd -m appuser

# Combine to reduce layers:
RUN apk update && \
    apk add --no-cache curl && \
    rm -rf /var/cache/apk/*
```

### **CMD vs ENTRYPOINT**
```dockerfile
# CMD - Default command (can be overridden)
CMD ["node", "server.js"]
# Run: docker run myapp              → node server.js
# Run: docker run myapp npm test     → npm test

# ENTRYPOINT - Fixed command (args append)
ENTRYPOINT ["node"]
CMD ["server.js"]
# Run: docker run myapp              → node server.js
# Run: docker run myapp debug.js     → node debug.js
```

### **ENV** - Environment Variables
```dockerfile
ENV NODE_ENV=production
ENV PORT=3000

# Can override at runtime:
# docker run -e PORT=8080 myapp
```

### **EXPOSE** - Document Ports
```dockerfile
EXPOSE 3000
# Doesn't actually publish port (just documentation)
# Use: docker run -p 3000:3000 myapp
```

### **USER** - Switch User
```dockerfile
USER node
# All subsequent RUN, CMD, ENTRYPOINT run as 'node' user
```

### **HEALTHCHECK** - Container Health
```dockerfile
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:3000/health || exit 1
```

---

## 🛡️ Security Best Practices (10 Rules)

1. ✅ **Pin versions**: `FROM node:18.20.4-alpine3.20` not `FROM node:latest`
2. ✅ **Use minimal base**: Alpine (5MB) vs Ubuntu (900MB)
3. ✅ **Multi-stage builds**: Separate build and runtime stages
4. ✅ **Non-root user**: `USER node` or create custom user
5. ✅ **Update packages**: `RUN apk update && apk upgrade --no-cache`
6. ✅ **Remove cache**: `RUN npm cache clean --force`
7. ✅ **Copy ownership**: `COPY --chown=node:node . .`
8. ✅ **Health checks**: `HEALTHCHECK CMD curl -f http://localhost/health`
9. ✅ **Scan images**: `trivy image myapp:latest`
10. ✅ **Sign images**: `cosign sign myapp:latest`

---

## 🚀 Common Docker Commands

### **Build**
```bash
# Basic build
docker build -t myapp:v1.0 .

# With build args
docker build --build-arg NODE_ENV=production -t myapp:v1.0 .

# Multi-platform (ARM + x86)
docker buildx build --platform linux/amd64,linux/arm64 -t myapp:v1.0 .
```

### **Run**
```bash
# Basic run
docker run -p 3000:3000 myapp:v1.0

# With environment variables
docker run -e PORT=8080 -e NODE_ENV=dev myapp:v1.0

# Interactive with bash
docker run -it myapp:v1.0 /bin/sh

# Background (detached)
docker run -d --name myapp-container myapp:v1.0

# With volume mount
docker run -v $(pwd)/data:/app/data myapp:v1.0
```

### **Manage**
```bash
# List running containers
docker ps

# List all containers
docker ps -a

# Stop container
docker stop myapp-container

# Remove container
docker rm myapp-container

# View logs
docker logs myapp-container
docker logs -f myapp-container  # Follow mode

# Execute command in running container
docker exec -it myapp-container /bin/sh
```

### **Images**
```bash
# List images
docker images

# Remove image
docker rmi myapp:v1.0

# Tag image
docker tag myapp:v1.0 registry.com/myapp:v1.0

# Push to registry
docker push registry.com/myapp:v1.0

# Pull from registry
docker pull registry.com/myapp:v1.0
```

---

## 🎓 42 Inception Project Notes

### **What You'll Need to Know**

1. **Docker Compose** - Multi-container orchestration
   ```yaml
   version: '3.8'
   services:
     nginx:
       build: ./nginx
       ports:
         - "443:443"
     wordpress:
       build: ./wordpress
       depends_on:
         - mariadb
     mariadb:
       build: ./mariadb
       volumes:
         - db_data:/var/lib/mysql
   ```

2. **Networking** - Container communication
   ```bash
   # Containers can talk to each other by service name
   # wordpress can reach: mysql://mariadb:3306
   ```

3. **Volumes** - Persistent data
   ```dockerfile
   VOLUME /var/lib/mysql  # Data survives container restart
   ```

4. **Custom Images** - No DockerHub allowed!
   ```dockerfile
   # Must build from scratch (Alpine/Debian base only)
   FROM alpine:3.18
   RUN apk add nginx
   # ... configure nginx from scratch
   ```

### **Key Inception Requirements**
- ✅ NGINX with TLSv1.2 or TLSv1.3
- ✅ WordPress + php-fpm
- ✅ MariaDB (no official image)
- ✅ Volumes for WordPress files + database
- ✅ Docker network
- ✅ Containers restart on crash
- ✅ No passwords in Dockerfiles (use .env)

---

## 📚 Learning Resources

### **Recommended Courses**:
1. **Docker Mastery** (Udemy - Bret Fisher) - 19 hours, comprehensive
2. **Docker for Developers** (Pluralsight) - 8 hours, practical
3. **Official Docker Docs** - https://docs.docker.com/get-started/

### **Practice Projects**:
1. ✅ Simple Node.js API (DONE - Day 59)
2. 🔄 42 Inception (UPCOMING - NGINX + WordPress + MariaDB)
3. 🔄 Multi-service app (React + Node + PostgreSQL)

### **Security Deep Dives**:
- Dockerfile Security Best Practices: https://docs.docker.com/develop/security-best-practices/
- OWASP Docker Security Cheat Sheet
- CIS Docker Benchmark

---

## 🎯 Your Current Progress

✅ **Docker Skills Acquired**:
- Multi-stage builds (Day 35)
- Security hardening (10+ measures)
- Podman proficiency
- Container registries (GitHub CR)
- Image scanning (Trivy)
- Image signing (Cosign)

🔄 **Next Steps for Inception**:
1. Learn Docker Compose
2. Practice custom Nginx configuration
3. WordPress + php-fpm setup
4. MariaDB from scratch (no official image)
5. Volume management
6. TLS certificate generation

---

## 💡 Quick Tips

### **Dockerfile Anti-Patterns to Avoid**
```dockerfile
# ❌ Don't do this:
FROM node:latest                    # Unpinned!
RUN apt-get update                 # Without upgrade
COPY . .                           # Before npm install
RUN npm install                    # Use npm ci instead
# No USER instruction              # Running as root!
```

### **Debugging Tips**
```bash
# Build and stop at specific stage
docker build --target builder -t debug .

# Run and get shell access
docker run -it --entrypoint /bin/sh myapp:v1.0

# Check what's inside image
docker run --rm -it myapp:v1.0 ls -la /app

# View image layers
docker history myapp:v1.0
```

---

## 📊 npm ci Performance Comparison

| Project Size | npm install | npm ci | Speed Improvement |
|--------------|-------------|--------|-------------------|
| Small (10 deps) | 5s | 2s | 2.5x faster |
| Medium (50 deps) | 25s | 8s | 3.1x faster |
| Large (200 deps) | 90s | 30s | 3x faster |

**In CI/CD**: `npm ci` is always the right choice!

---

*Last Updated: November 14, 2025*  
*Status: Reference guide for Day 59 and future Docker projects*  
*Next: Apply these concepts to 42 Inception project*
