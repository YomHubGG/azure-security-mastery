// 🔐 Secure Node.js App with Azure Key Vault Integration
// This demonstrates universal patterns for secrets management

const express = require('express');
const { SecretClient } = require('@azure/keyvault-secrets');
const { DefaultAzureCredential } = require('@azure/identity');

const app = express();

// ✅ UNIVERSAL PATTERN: Get configuration from environment
const keyVaultUrl = process.env.KEYVAULT_URL || 'https://kvlearning4uybw3c2lbkwm.vault.azure.net/';
const port = process.env.PORT || 3000;

// ✅ UNIVERSAL PATTERN: Initialize secrets client
const credential = new DefaultAzureCredential();
const secretClient = new SecretClient(keyVaultUrl, credential);

// ✅ UNIVERSAL PATTERN: Cache secrets for performance
let secretsCache = {};

// ✅ UNIVERSAL PATTERN: Async function to load secrets
async function loadSecrets() {
    try {
        console.log('🔑 Loading secrets from Key Vault...');
        
        // Load all required secrets
        const [dbPassword, jwtSecret, apiKey] = await Promise.all([
            secretClient.getSecret('database-password'),
            secretClient.getSecret('jwt-secret'),
            secretClient.getSecret('external-api-key')
        ]);
        
        // Cache secrets for app use
        secretsCache = {
            databasePassword: dbPassword.value,
            jwtSecret: jwtSecret.value,
            externalApiKey: apiKey.value
        };
        
        console.log('✅ All secrets loaded successfully');
        return secretsCache;
        
    } catch (error) {
        console.error('❌ Failed to load secrets:', error.message);
        throw error;
    }
}

// ✅ UNIVERSAL PATTERN: Health check endpoint
app.get('/health', (req, res) => {
    res.json({
        status: 'healthy',
        timestamp: new Date().toISOString(),
        secrets: Object.keys(secretsCache).length > 0 ? 'loaded' : 'not loaded'
    });
});

// ✅ UNIVERSAL PATTERN: Configuration endpoint (safe)
app.get('/config', (req, res) => {
    res.json({
        keyVaultUrl: keyVaultUrl,
        environment: process.env.NODE_ENV || 'development',
        secretsAvailable: Object.keys(secretsCache),
        // ❌ NEVER expose actual secret values!
        secretsStatus: Object.keys(secretsCache).reduce((acc, key) => {
            acc[key] = secretsCache[key] ? 'loaded' : 'missing';
            return acc;
        }, {})
    });
});

// ✅ UNIVERSAL PATTERN: Demonstrate secure database connection
app.get('/database-test', async (req, res) => {
    try {
        // In real app, you'd connect to actual database
        const dbPassword = secretsCache.databasePassword;
        
        // Simulate database connection
        const dbConnection = {
            host: 'your-database-host',
            user: 'your-database-user',
            password: dbPassword ? '[SECURED]' : '[MISSING]',
            status: dbPassword ? 'connected' : 'failed'
        };
        
        res.json({
            message: 'Database connection test',
            connection: dbConnection
        });
        
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// ✅ UNIVERSAL PATTERN: Demonstrate API key usage
app.get('/external-api-test', async (req, res) => {
    try {
        const apiKey = secretsCache.externalApiKey;
        
        if (!apiKey) {
            return res.status(500).json({ error: 'API key not available' });
        }
        
        // In real app, you'd call external service
        res.json({
            message: 'External API test',
            apiKeyStatus: 'available',
            // ❌ NEVER expose actual API key!
            apiKeyPreview: apiKey.substring(0, 6) + '...'
        });
        
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// ✅ UNIVERSAL PATTERN: Graceful startup with error handling
async function startServer() {
    try {
        // Load secrets before starting server
        await loadSecrets();
        
        app.listen(port, () => {
            console.log(`🚀 Server running on port ${port}`);
            console.log(`📊 Health check: http://localhost:${port}/health`);
            console.log(`⚙️ Configuration: http://localhost:${port}/config`);
            console.log(`🗄️ Database test: http://localhost:${port}/database-test`);
            console.log(`🔌 API test: http://localhost:${port}/external-api-test`);
        });
        
    } catch (error) {
        console.error('💥 Failed to start server:', error.message);
        process.exit(1);
    }
}

// ✅ UNIVERSAL PATTERN: Graceful shutdown
process.on('SIGINT', () => {
    console.log('\n🛑 Shutting down gracefully...');
    process.exit(0);
});

// Start the application
startServer();

// 🎓 LEARNING NOTES:
// 1. This pattern works with ANY secrets manager (AWS, Google, HashiCorp)
// 2. Environment variables for configuration (12-factor app)
// 3. Async/await for all external calls
// 4. Error handling at every level
// 5. Health checks for monitoring
// 6. Never expose secrets in responses
// 7. Cache secrets for performance
// 8. Graceful startup and shutdown
