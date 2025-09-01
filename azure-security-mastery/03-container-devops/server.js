// Simple Express app for testing
const express = require('express');
const app = express();
const PORT = process.env.PORT || 3000;

// Health check endpoint
app.get('/health', (req, res) => {
  res.status(200).json({ status: 'healthy', timestamp: new Date().toISOString() });
});

// Main endpoint
app.get('/', (req, res) => {
  res.json({ 
    message: '🐳 Secure Container Running!',
    environment: process.env.NODE_ENV || 'development',
    user: process.getuid() // Should not be 0 (root)
  });
});

app.listen(PORT, () => {
  console.log(`🚀 Secure app running on port ${PORT}`);
  console.log(`👤 Running as user ID: ${process.getuid()}`);
});
