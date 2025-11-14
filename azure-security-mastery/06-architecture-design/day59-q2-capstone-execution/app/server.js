const express = require('express');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');

const app = express();
const PORT = process.env.PORT || 3000;

// Security middleware - Helmet.js for security headers
app.use(helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", "data:"],
    },
  },
  hsts: {
    maxAge: 31536000,
    includeSubDomains: true,
    preload: true
  }
}));

// Rate limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100, // limit each IP to 100 requests per windowMs
  message: 'Too many requests from this IP, please try again later.'
});
app.use(limiter);

// JSON parsing
app.use(express.json());

// Health check endpoint
app.get('/health', (req, res) => {
  res.status(200).json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    uptime: process.uptime(),
    environment: process.env.NODE_ENV || 'production'
  });
});

// Readiness probe
app.get('/ready', (req, res) => {
  // Check if app is ready to serve traffic
  res.status(200).json({
    ready: true,
    timestamp: new Date().toISOString()
  });
});

// Metrics endpoint
app.get('/metrics', (req, res) => {
  res.status(200).json({
    uptime: process.uptime(),
    memory: process.memoryUsage(),
    cpu: process.cpuUsage(),
    pid: process.pid,
    version: process.version,
    user: process.getuid ? process.getuid() : 'N/A'
  });
});

// Main endpoint
app.get('/', (req, res) => {
  res.status(200).send(`
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>SecureCloud DevSecOps Platform</title>
      <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          min-height: 100vh;
          display: flex;
          align-items: center;
          justify-content: center;
          padding: 20px;
        }
        .container {
          background: white;
          border-radius: 20px;
          box-shadow: 0 20px 60px rgba(0,0,0,0.3);
          padding: 40px;
          max-width: 800px;
          width: 100%;
        }
        h1 {
          color: #667eea;
          font-size: 2.5em;
          margin-bottom: 10px;
          text-align: center;
        }
        .subtitle {
          text-align: center;
          color: #666;
          margin-bottom: 30px;
          font-size: 1.1em;
        }
        .badge {
          display: inline-block;
          background: #10b981;
          color: white;
          padding: 8px 16px;
          border-radius: 20px;
          font-size: 0.9em;
          margin: 5px;
        }
        .badge.security { background: #3b82f6; }
        .badge.cost { background: #f59e0b; }
        .section {
          margin: 25px 0;
          padding: 20px;
          background: #f8f9fa;
          border-radius: 10px;
          border-left: 4px solid #667eea;
        }
        .section h2 {
          color: #333;
          font-size: 1.3em;
          margin-bottom: 15px;
        }
        .grid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
          gap: 15px;
          margin-top: 15px;
        }
        .stat {
          background: white;
          padding: 15px;
          border-radius: 8px;
          box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .stat-label {
          color: #666;
          font-size: 0.85em;
          margin-bottom: 5px;
        }
        .stat-value {
          color: #667eea;
          font-size: 1.5em;
          font-weight: bold;
        }
        .check { color: #10b981; font-size: 1.2em; }
        ul { list-style: none; }
        li { padding: 8px 0; border-bottom: 1px solid #e5e7eb; }
        li:last-child { border-bottom: none; }
        .footer {
          text-align: center;
          margin-top: 30px;
          color: #666;
          font-size: 0.9em;
        }
        a { color: #667eea; text-decoration: none; }
        a:hover { text-decoration: underline; }
      </style>
    </head>
    <body>
      <div class="container">
        <h1>🛡️ SecureCloud DevSecOps Platform</h1>
        <p class="subtitle">Q2 Capstone Project - Day 59</p>
        
        <div style="text-align: center; margin-bottom: 20px;">
          <span class="badge">✅ Deployed</span>
          <span class="badge security">🔒 40+ Security Controls</span>
          <span class="badge cost">💰 €0.00 Cost</span>
        </div>

        <div class="section">
          <h2>📊 Project Metrics</h2>
          <div class="grid">
            <div class="stat">
              <div class="stat-label">Sessions Completed</div>
              <div class="stat-value">29</div>
            </div>
            <div class="stat">
              <div class="stat-label">Security Controls</div>
              <div class="stat-value">40+</div>
            </div>
            <div class="stat">
              <div class="stat-label">Security Score</div>
              <div class="stat-value">83.3%</div>
            </div>
            <div class="stat">
              <div class="stat-label">Total Cost</div>
              <div class="stat-value">€0.00</div>
            </div>
          </div>
        </div>

        <div class="section">
          <h2>🔒 Security Layers</h2>
          <ul>
            <li><span class="check">✓</span> OIDC Zero-Trust Authentication</li>
            <li><span class="check">✓</span> Trivy Vulnerability Scanning (0 CRITICAL)</li>
            <li><span class="check">✓</span> SBOM Generation (CycloneDX)</li>
            <li><span class="check">✓</span> Cosign Image Signing</li>
            <li><span class="check">✓</span> TruffleHog Secret Scanning (0 secrets)</li>
            <li><span class="check">✓</span> 90-Day Secret Rotation (PCI-DSS)</li>
            <li><span class="check">✓</span> CIS Azure Compliance (14 controls)</li>
            <li><span class="check">✓</span> Read-Only Root Filesystem</li>
          </ul>
        </div>

        <div class="section">
          <h2>🏗️ Architecture</h2>
          <ul>
            <li><strong>Platform:</strong> Azure Container Instances (West Europe)</li>
            <li><strong>Container:</strong> Podman rootless, non-root user (UID 1000)</li>
            <li><strong>Registry:</strong> GitHub Container Registry (signed)</li>
            <li><strong>CI/CD:</strong> GitHub Actions with OIDC</li>
            <li><strong>IaC:</strong> Bicep templates (Checkov validated)</li>
            <li><strong>Secrets:</strong> Azure Key Vault + Managed Identity</li>
          </ul>
        </div>

        <div class="section">
          <h2>📚 Learning Journey</h2>
          <p><strong>Timeline:</strong> September 1 - November 14, 2025 (74 days)</p>
          <p><strong>Days Completed:</strong> 57/365 (15.6%)</p>
          <p><strong>Tools Mastered:</strong> 25+ (Podman, Trivy, Checkov, Semgrep, TruffleHog, Cosign, k3s, SARIF, Bicep, Azure CLI)</p>
          <p><strong>CVEs Analyzed:</strong> 32 vulnerabilities</p>
          <p><strong>Breach Studies:</strong> 15 cases ($4.5B+ in fines)</p>
        </div>

        <div class="footer">
          <p><strong>Day 59: Q2 Capstone Execution</strong></p>
          <p>Session #30 | November 14, 2025</p>
          <p><a href="/health">Health Check</a> | <a href="/metrics">Metrics</a> | <a href="/ready">Readiness</a></p>
        </div>
      </div>
    </body>
    </html>
  `);
});

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    error: 'Not Found',
    path: req.path,
    timestamp: new Date().toISOString()
  });
});

// Error handler
app.use((err, req, res, next) => {
  console.error('Error:', err.message);
  res.status(500).json({
    error: 'Internal Server Error',
    message: process.env.NODE_ENV === 'development' ? err.message : 'Something went wrong'
  });
});

// Start server
const server = app.listen(PORT, '0.0.0.0', () => {
  console.log(`✅ SecureCloud DevSecOps Platform running on port ${PORT}`);
  console.log(`🔒 Running as user: ${process.getuid ? process.getuid() : 'N/A'}`);
  console.log(`🌍 Environment: ${process.env.NODE_ENV || 'production'}`);
  console.log(`📊 Endpoints: /health, /ready, /metrics, /`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('SIGTERM received, closing server gracefully...');
  server.close(() => {
    console.log('Server closed');
    process.exit(0);
  });
});

process.on('SIGINT', () => {
  console.log('SIGINT received, closing server gracefully...');
  server.close(() => {
    console.log('Server closed');
    process.exit(0);
  });
});
