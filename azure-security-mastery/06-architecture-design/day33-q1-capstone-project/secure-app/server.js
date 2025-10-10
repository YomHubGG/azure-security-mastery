const express = require('express');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const cors = require('cors');
const compression = require('compression');
const appInsights = require('applicationinsights');
require('dotenv').config();

// ============================================================================
// 🔒 SECURITY CONFIGURATION - Day 33 Capstone Project
// ============================================================================

// Initialize Application Insights (Azure Monitoring)
if (process.env.APPLICATIONINSIGHTS_CONNECTION_STRING) {
  appInsights.setup(process.env.APPLICATIONINSIGHTS_CONNECTION_STRING)
    .setAutoDependencyCorrelation(true)
    .setAutoCollectRequests(true)
    .setAutoCollectPerformance(true, true)
    .setAutoCollectExceptions(true)
    .setAutoCollectDependencies(true)
    .setAutoCollectConsole(true)
    .setUseDiskRetryCaching(true)
    .setSendLiveMetrics(true)
    .start();
  
  console.log('✅ Application Insights enabled');
} else {
  console.log('⚠️  Application Insights not configured (set APPLICATIONINSIGHTS_CONNECTION_STRING)');
}

const app = express();
const PORT = process.env.PORT || 3000;

// ============================================================================
// 🛡️ SECURITY MIDDLEWARE - Implementing Security Controls from Months 1-3
// ============================================================================

// 1. Helmet.js - Sets security headers (Day 21: Application Security)
app.use(helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", 'data:', 'https:'],
    },
  },
  hsts: {
    maxAge: 31536000, // 1 year
    includeSubDomains: true,
    preload: true
  },
  frameguard: {
    action: 'deny' // Prevent clickjacking
  },
  referrerPolicy: {
    policy: 'strict-origin-when-cross-origin'
  }
}));

// 2. Rate Limiting - Prevent brute force attacks (Day 21: Application Security)
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100, // Limit each IP to 100 requests per windowMs
  message: 'Too many requests from this IP, please try again later.',
  standardHeaders: true,
  legacyHeaders: false,
});
app.use('/api/', limiter);

// 3. CORS - Configure allowed origins (Day 21: Application Security)
const corsOptions = {
  origin: process.env.ALLOWED_ORIGINS ? process.env.ALLOWED_ORIGINS.split(',') : '*',
  methods: ['GET', 'POST'],
  allowedHeaders: ['Content-Type', 'Authorization'],
  credentials: true,
  maxAge: 600 // 10 minutes
};
app.use(cors(corsOptions));

// 4. Compression - Improve performance
app.use(compression());

// 5. Body parsing with size limits - Prevent payload attacks
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// ============================================================================
// 📊 MONITORING & LOGGING
// ============================================================================

// Request logging middleware
app.use((req, res, next) => {
  const start = Date.now();
  res.on('finish', () => {
    const duration = Date.now() - start;
    console.log(`${req.method} ${req.path} - ${res.statusCode} - ${duration}ms`);
    
    // Track custom metric in Application Insights
    if (appInsights.defaultClient) {
      appInsights.defaultClient.trackMetric({
        name: 'Request Duration',
        value: duration
      });
    }
  });
  next();
});

// ============================================================================
// 🚀 APPLICATION ROUTES
// ============================================================================

// Health check endpoint (required for Azure App Service)
app.get('/health', (req, res) => {
  res.status(200).json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    uptime: process.uptime(),
    environment: process.env.NODE_ENV || 'development'
  });
});

// Main landing page
app.get('/', (req, res) => {
  res.send(`
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Azure Security Capstone - Day 33</title>
      <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
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
          padding: 40px;
          max-width: 800px;
          box-shadow: 0 20px 60px rgba(0,0,0,0.3);
        }
        h1 {
          color: #667eea;
          margin-bottom: 10px;
          font-size: 2.5em;
        }
        .subtitle {
          color: #666;
          margin-bottom: 30px;
          font-size: 1.2em;
        }
        .security-badge {
          display: inline-block;
          background: #10b981;
          color: white;
          padding: 8px 16px;
          border-radius: 20px;
          font-weight: bold;
          margin: 10px 5px;
          font-size: 0.9em;
        }
        .section {
          margin: 30px 0;
          padding: 20px;
          background: #f9fafb;
          border-radius: 10px;
          border-left: 4px solid #667eea;
        }
        .section h2 {
          color: #333;
          margin-bottom: 15px;
          font-size: 1.5em;
        }
        .feature-list {
          list-style: none;
          padding: 0;
        }
        .feature-list li {
          padding: 10px 0;
          border-bottom: 1px solid #e5e7eb;
        }
        .feature-list li:last-child {
          border-bottom: none;
        }
        .feature-list li:before {
          content: "✅ ";
          margin-right: 10px;
        }
        .api-section {
          background: #1f2937;
          color: white;
          padding: 20px;
          border-radius: 10px;
          margin-top: 20px;
        }
        .api-section h3 {
          color: #10b981;
          margin-bottom: 10px;
        }
        .endpoint {
          background: #374151;
          padding: 10px;
          border-radius: 5px;
          margin: 10px 0;
          font-family: 'Courier New', monospace;
        }
        .footer {
          text-align: center;
          margin-top: 30px;
          color: #666;
          font-size: 0.9em;
        }
      </style>
    </head>
    <body>
      <div class="container">
        <h1>🔒 Azure Security Capstone</h1>
        <p class="subtitle">Day 33 - Q1 Integration Project</p>
        
        <div style="margin: 20px 0;">
          <span class="security-badge">HTTPS Only</span>
          <span class="security-badge">Helmet.js</span>
          <span class="security-badge">Rate Limited</span>
          <span class="security-badge">CORS Protected</span>
          <span class="security-badge">App Insights</span>
        </div>

        <div class="section">
          <h2>🎯 Project Overview</h2>
          <p>This application demonstrates comprehensive Azure security controls learned across Months 1-3 of the 1-year security journey.</p>
        </div>

        <div class="section">
          <h2>🛡️ Security Features Implemented</h2>
          <ul class="feature-list">
            <li><strong>Security Headers</strong> - Helmet.js with CSP, HSTS, X-Frame-Options</li>
            <li><strong>Rate Limiting</strong> - 100 requests per 15 minutes per IP</li>
            <li><strong>CORS Protection</strong> - Configured allowed origins</li>
            <li><strong>Input Validation</strong> - 10MB payload limit</li>
            <li><strong>Monitoring</strong> - Azure Application Insights integration</li>
            <li><strong>Health Checks</strong> - Production-ready health endpoint</li>
            <li><strong>Compression</strong> - Response optimization</li>
          </ul>
        </div>

        <div class="section">
          <h2>🏗️ Infrastructure Security</h2>
          <ul class="feature-list">
            <li><strong>Key Vault</strong> - Soft delete + purge protection (90-day retention)</li>
            <li><strong>Storage Accounts</strong> - HTTPS-only, TLS 1.2, no public access</li>
            <li><strong>Virtual Networks</strong> - Segmented dev/prod environments</li>
            <li><strong>Network Security Groups</strong> - Inbound/outbound traffic control</li>
            <li><strong>App Service</strong> - F1 Free tier with security best practices</li>
          </ul>
        </div>

        <div class="api-section">
          <h3>📡 Available Endpoints</h3>
          <div class="endpoint">
            <strong>GET</strong> /health - Health check endpoint
          </div>
          <div class="endpoint">
            <strong>GET</strong> /api/security-info - Security configuration details
          </div>
          <div class="endpoint">
            <strong>GET</strong> /api/metrics - Application metrics
          </div>
        </div>

        <div class="footer">
          <p>🚀 Built with Node.js + Express | 🔐 Secured with Azure Best Practices</p>
          <p>💼 Portfolio Project - YomHub Cybersecurity Journey</p>
          <p>📅 October 2025 - Day 33/365</p>
        </div>
      </div>
    </body>
    </html>
  `);
});

// Security information API
app.get('/api/security-info', (req, res) => {
  res.json({
    securityHeaders: {
      helmet: 'enabled',
      contentSecurityPolicy: 'enforced',
      hsts: 'enabled (1 year)',
      frameguard: 'deny',
      referrerPolicy: 'strict-origin-when-cross-origin'
    },
    rateLimiting: {
      enabled: true,
      windowMs: '15 minutes',
      maxRequests: 100
    },
    cors: {
      enabled: true,
      allowedOrigins: corsOptions.origin
    },
    monitoring: {
      applicationInsights: appInsights.defaultClient ? 'connected' : 'not configured'
    },
    infrastructure: {
      keyVaults: 3,
      storageAccounts: 7,
      virtualNetworks: 3,
      securityScore: '88/100'
    }
  });
});

// Application metrics API
app.get('/api/metrics', (req, res) => {
  res.json({
    uptime: process.uptime(),
    timestamp: new Date().toISOString(),
    memory: process.memoryUsage(),
    nodeVersion: process.version,
    platform: process.platform,
    environment: process.env.NODE_ENV || 'development'
  });
});

// ============================================================================
// 🚨 ERROR HANDLING
// ============================================================================

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    error: 'Not Found',
    message: 'The requested resource does not exist',
    path: req.path
  });
});

// Global error handler
app.use((err, req, res, next) => {
  console.error('Error:', err);
  
  // Track exception in Application Insights
  if (appInsights.defaultClient) {
    appInsights.defaultClient.trackException({ exception: err });
  }
  
  res.status(err.status || 500).json({
    error: 'Internal Server Error',
    message: process.env.NODE_ENV === 'production' ? 'An error occurred' : err.message
  });
});

// ============================================================================
// 🎬 START SERVER
// ============================================================================

app.listen(PORT, () => {
  console.log('');
  console.log('🔒 ====================================');
  console.log('   Azure Security Capstone - Day 33');
  console.log('====================================== 🔒');
  console.log('');
  console.log(`✅ Server running on port ${PORT}`);
  console.log(`🌍 Environment: ${process.env.NODE_ENV || 'development'}`);
  console.log(`🏥 Health check: http://localhost:${PORT}/health`);
  console.log(`📊 Metrics: http://localhost:${PORT}/api/metrics`);
  console.log(`🔐 Security info: http://localhost:${PORT}/api/security-info`);
  console.log('');
  console.log('🛡️  Security Features Active:');
  console.log('   ✓ Helmet.js security headers');
  console.log('   ✓ Rate limiting (100 req/15min)');
  console.log('   ✓ CORS protection');
  console.log('   ✓ Input validation & size limits');
  console.log('   ✓ Request logging & monitoring');
  console.log('');
  console.log('Press Ctrl+C to stop');
  console.log('');
});

module.exports = app;
