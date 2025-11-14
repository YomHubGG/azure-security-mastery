# =============================================================================
# Day 59 Q2 Capstone: Secure Multi-Stage Dockerfile
# Security Controls: 10+ hardening measures from Days 35-43
# =============================================================================

# Stage 1: Builder (larger image for building)
FROM docker.io/library/node:18-alpine3.19 AS builder

# Security: Non-root user for build (use node user from base image)
# Note: node:alpine base image already includes 'node' user with UID 1000

WORKDIR /build

# Copy dependency files explicitly
COPY package.json package-lock.json ./

# Install dependencies (as root in builder, then copy as node user)
RUN npm ci --only=production && \
    npm cache clean --force

# Set ownership for node user
RUN chown -R node:node /build

# Copy application code
COPY --chown=node:node server.js ./

# =============================================================================
# Stage 2: Runtime (minimal production image)
# =============================================================================
FROM docker.io/library/node:18-alpine3.19

# Metadata labels
LABEL maintainer="YomHubGG" \
      version="1.0.0" \
      description="SecureCloud DevSecOps Platform - Q2 Capstone" \
      security.scan="trivy" \
      security.sbom="cyclonedx" \
      security.signed="cosign"

# Security: Install only security updates
RUN apk update && \
    apk upgrade --no-cache && \
    apk add --no-cache dumb-init && \
    rm -rf /var/cache/apk/* /tmp/* /var/tmp/*

# Security: Create app directory with proper permissions
# Note: node:alpine base image already includes 'node' user with UID 1000
WORKDIR /app
RUN chown -R node:node /app

# Copy from builder stage
COPY --from=builder --chown=node:node /build/node_modules ./node_modules
COPY --from=builder --chown=node:node /build/server.js ./
COPY --from=builder --chown=node:node /build/package.json ./

# Security: Switch to non-root user (UID 1000)
USER node

# Security: Expose port (non-privileged)
EXPOSE 3000

# Security: Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3000/health', (res) => { process.exit(res.statusCode === 200 ? 0 : 1); })" || exit 1

# Security: Use dumb-init to handle signals properly
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

# Security: Run as node process (no shell)
CMD ["node", "server.js"]

# =============================================================================
# Security Measures Implemented:
# 1. ✅ Multi-stage build (minimal attack surface)
# 2. ✅ Non-root user (UID 1000)
# 3. ✅ Alpine Linux (small base image)
# 4. ✅ Security updates only
# 5. ✅ No secrets in image
# 6. ✅ Health check configured
# 7. ✅ Proper signal handling (dumb-init)
# 8. ✅ Cache cleaning
# 9. ✅ Proper file ownership
# 10. ✅ Metadata labels
# 11. ✅ Non-privileged port (3000)
# 12. ✅ No shell in CMD (direct node execution)
# =============================================================================
