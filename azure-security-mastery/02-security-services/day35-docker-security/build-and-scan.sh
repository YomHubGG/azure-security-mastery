#!/bin/bash
# ============================================
# Build and Scan Container Image
# Automates secure container build + security scanning
# ============================================

set -e  # Exit on error
set -u  # Exit on undefined variable

# ============================================
# Configuration
# ============================================
IMAGE_NAME="secure-app"
IMAGE_TAG="1.0.0"
FULL_IMAGE="${IMAGE_NAME}:${IMAGE_TAG}"
DOCKERFILE="Dockerfile.secure"
SCAN_SEVERITY="HIGH,CRITICAL"  # Only fail on HIGH/CRITICAL

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ============================================
# Functions
# ============================================

print_header() {
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}============================================${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

check_prerequisites() {
    print_header "Checking Prerequisites"
    
    # Check for Podman (or Docker)
    if command -v podman &> /dev/null; then
        CONTAINER_TOOL="podman"
        print_success "Found Podman $(podman --version)"
    elif command -v docker &> /dev/null; then
        CONTAINER_TOOL="docker"
        print_success "Found Docker $(docker --version)"
    else
        print_error "Neither Podman nor Docker found!"
        exit 1
    fi
    
    # Check for Trivy scanner (optional but recommended)
    if command -v trivy &> /dev/null; then
        TRIVY_AVAILABLE=true
        print_success "Found Trivy $(trivy --version | head -n1)"
    else
        TRIVY_AVAILABLE=false
        print_warning "Trivy not found. Install with: brew install aquasecurity/trivy/trivy"
        print_warning "Continuing without vulnerability scanning..."
    fi
    
    echo ""
}

build_image() {
    print_header "Building Container Image"
    
    echo "Image: ${FULL_IMAGE}"
    echo "Dockerfile: ${DOCKERFILE}"
    echo ""
    
    # Build with Podman/Docker
    if ${CONTAINER_TOOL} build \
        -f "${DOCKERFILE}" \
        -t "${FULL_IMAGE}" \
        --progress=plain \
        ../../06-architecture-design/day33-q1-capstone-project/secure-app/; then
        print_success "Image built successfully!"
    else
        print_error "Build failed!"
        exit 1
    fi
    
    echo ""
}

inspect_image() {
    print_header "Image Details"
    
    # Get image size
    IMAGE_SIZE=$(${CONTAINER_TOOL} images ${FULL_IMAGE} --format "{{.Size}}")
    print_success "Image size: ${IMAGE_SIZE}"
    
    # Show layers (first 10)
    echo ""
    echo "Layers (first 10):"
    ${CONTAINER_TOOL} history ${FULL_IMAGE} | head -n 11
    
    echo ""
}

scan_with_trivy() {
    print_header "Scanning with Trivy"
    
    if [ "$TRIVY_AVAILABLE" = false ]; then
        print_warning "Trivy not available, skipping scan"
        return 0
    fi
    
    # Create scan report directory
    REPORT_DIR="scan-reports"
    mkdir -p "${REPORT_DIR}"
    REPORT_FILE="${REPORT_DIR}/trivy-$(date +%Y%m%d-%H%M%S).json"
    
    echo "Scanning for vulnerabilities..."
    echo "Report will be saved to: ${REPORT_FILE}"
    echo ""
    
    # Run Trivy scan
    if trivy image \
        --severity "${SCAN_SEVERITY}" \
        --format json \
        --output "${REPORT_FILE}" \
        "${FULL_IMAGE}"; then
        
        # Parse results
        CRITICAL=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity=="CRITICAL")] | length' "${REPORT_FILE}")
        HIGH=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity=="HIGH")] | length' "${REPORT_FILE}")
        MEDIUM=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity=="MEDIUM")] | length' "${REPORT_FILE}")
        LOW=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity=="LOW")] | length' "${REPORT_FILE}")
        
        echo ""
        echo "Vulnerability Summary:"
        echo -e "  ${RED}CRITICAL: ${CRITICAL}${NC}"
        echo -e "  ${YELLOW}HIGH: ${HIGH}${NC}"
        echo -e "  ${BLUE}MEDIUM: ${MEDIUM}${NC}"
        echo -e "  ${GREEN}LOW: ${LOW}${NC}"
        echo ""
        
        # Fail if critical/high vulnerabilities found
        if [ "$CRITICAL" -gt 0 ] || [ "$HIGH" -gt 0 ]; then
            print_error "Found ${CRITICAL} CRITICAL and ${HIGH} HIGH vulnerabilities!"
            print_warning "Review scan report: ${REPORT_FILE}"
            
            # Show top 5 issues
            echo ""
            echo "Top 5 Vulnerabilities:"
            trivy image --severity "${SCAN_SEVERITY}" --format table "${FULL_IMAGE}" | head -n 20
            
            return 1
        else
            print_success "No HIGH or CRITICAL vulnerabilities found!"
            return 0
        fi
    else
        print_error "Trivy scan failed!"
        return 1
    fi
}

run_container_test() {
    print_header "Testing Container"
    
    CONTAINER_NAME="test-${IMAGE_NAME}"
    
    # Remove existing test container if exists
    ${CONTAINER_TOOL} rm -f "${CONTAINER_NAME}" 2>/dev/null || true
    
    echo "Starting container..."
    ${CONTAINER_TOOL} run -d \
        --name "${CONTAINER_NAME}" \
        -p 3000:3000 \
        -e NODE_ENV=production \
        -e PORT=3000 \
        "${FULL_IMAGE}"
    
    print_success "Container started: ${CONTAINER_NAME}"
    
    # Wait for app to start
    echo "Waiting for app to start..."
    sleep 3
    
    # Test health endpoint
    echo "Testing health endpoint..."
    if curl -f -s http://localhost:3000/health > /dev/null 2>&1; then
        print_success "Health check passed!"
    else
        print_error "Health check failed!"
        ${CONTAINER_TOOL} logs "${CONTAINER_NAME}"
        ${CONTAINER_TOOL} stop "${CONTAINER_NAME}"
        ${CONTAINER_TOOL} rm "${CONTAINER_NAME}"
        exit 1
    fi
    
    # Show logs
    echo ""
    echo "Container logs (last 10 lines):"
    ${CONTAINER_TOOL} logs --tail 10 "${CONTAINER_NAME}"
    
    # Stop and remove test container
    echo ""
    echo "Stopping test container..."
    ${CONTAINER_TOOL} stop "${CONTAINER_NAME}"
    ${CONTAINER_TOOL} rm "${CONTAINER_NAME}"
    
    print_success "Test completed successfully!"
    echo ""
}

security_checklist() {
    print_header "Security Checklist"
    
    echo "Checking image against security best practices..."
    echo ""
    
    # Check 1: Non-root user
    USER_ID=$(${CONTAINER_TOOL} inspect ${FULL_IMAGE} --format '{{.Config.User}}')
    if [ -n "$USER_ID" ] && [ "$USER_ID" != "0" ] && [ "$USER_ID" != "root" ]; then
        print_success "Running as non-root user (${USER_ID})"
    else
        print_warning "Running as root user!"
    fi
    
    # Check 2: Health check
    HEALTHCHECK=$(${CONTAINER_TOOL} inspect ${FULL_IMAGE} --format '{{.Config.Healthcheck}}')
    if [ "$HEALTHCHECK" != "<nil>" ]; then
        print_success "Health check configured"
    else
        print_warning "No health check configured"
    fi
    
    # Check 3: Image size (should be < 200MB for Node.js Alpine)
    SIZE_MB=$(${CONTAINER_TOOL} images ${FULL_IMAGE} --format "{{.Size}}" | sed 's/MB//')
    if (( $(echo "$SIZE_MB < 200" | bc -l) )); then
        print_success "Image size optimized (${SIZE_MB}MB < 200MB)"
    else
        print_warning "Image size large (${SIZE_MB}MB)"
    fi
    
    # Check 4: Base image
    BASE_IMAGE=$(${CONTAINER_TOOL} inspect ${FULL_IMAGE} --format '{{index .Config.Labels "org.opencontainers.image.source"}}')
    if [ -n "$BASE_IMAGE" ]; then
        print_success "Source label present"
    fi
    
    echo ""
}

summary() {
    print_header "Build Summary"
    
    echo "Image: ${FULL_IMAGE}"
    echo "Size: $(${CONTAINER_TOOL} images ${FULL_IMAGE} --format '{{.Size}}')"
    echo "Created: $(${CONTAINER_TOOL} images ${FULL_IMAGE} --format '{{.CreatedAt}}')"
    echo ""
    
    print_success "Build and scan complete!"
    echo ""
    echo "Next steps:"
    echo "  1. Review scan report (if Trivy available)"
    echo "  2. Run manually: ${CONTAINER_TOOL} run -d -p 3000:3000 ${FULL_IMAGE}"
    echo "  3. Push to registry (optional): ${CONTAINER_TOOL} push ${FULL_IMAGE}"
    echo ""
}

# ============================================
# Main Execution
# ============================================

main() {
    echo ""
    print_header "Secure Container Build Pipeline"
    echo ""
    
    check_prerequisites
    build_image
    inspect_image
    
    # Only scan and test if requested (can be slow)
    if [ "${1:-}" == "--full" ]; then
        scan_with_trivy
        run_container_test
    else
        print_warning "Skipping scan and test. Run with --full for complete pipeline."
        echo ""
    fi
    
    security_checklist
    summary
}

# Run main function
main "$@"
