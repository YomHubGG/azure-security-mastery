// Web Terminal Server for Minishell
// Provides browser-based access to your cloud minishell

const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const pty = require('node-pty');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});

const PORT = process.env.PORT || 3000;

// Serve static files (web terminal UI)
app.use(express.static(path.join(__dirname, 'public')));

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({ 
    status: 'healthy', 
    service: 'minishell-web-terminal',
    timestamp: new Date().toISOString()
  });
});

// Main terminal page
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'terminal.html'));
});

// Socket.io connection handling
io.on('connection', (socket) => {
  console.log('🔌 New terminal connection:', socket.id);
  
  // Detect which shell to use - prioritize our 42 minishell
  const fs = require('fs');
  let shellPath = '/bin/bash'; // fallback
  let shellArgs = [];
  let shellName = 'bash';
  
  // Check for minishell in different locations
  const minishellPaths = [
    './minishell',           // Local development
    '/usr/local/bin/minishell', // Container location
    '/app/minishell'         // Alternative container location
  ];
  
  for (const path of minishellPaths) {
    if (fs.existsSync(path)) {
      shellPath = path;
      shellName = '42 Minishell';
      console.log(`🦥 Found and using 42 minishell at: ${path}`);
      break;
    }
  }
  
  if (shellName === 'bash') {
    console.log('🐚 Using bash fallback - minishell not found');
  }
  
  // Create a new shell process for each connection
  const shell = pty.spawn(shellPath, shellArgs, {
    name: 'xterm-color',
    cols: 80,
    rows: 24,
    cwd: process.cwd(),
    env: {
      ...process.env,
      TERM: 'xterm',  // Use xterm but not 256color to reduce escape sequences
      SHELL: shellPath,
      HOME: process.cwd()
    }
  });

  // Send welcome message
  socket.emit('output', `🐚 Welcome to Cloud ${shellName}!\n`);
  socket.emit('output', '======================================\n');
  socket.emit('output', 'Your 42 shell is now running in the cloud!\n');
  socket.emit('output', '🦥🦥\n\n');

  // Handle data from minishell
  shell.on('data', (data) => {
    // Filter out problematic ANSI escape sequences
    const cleanData = data
      .replace(/\x1b\[\?2004[hl]/g, '') // Remove bracketed paste mode
      .replace(/\x1b\[K/g, '');         // Remove clear line sequences
    
    if (cleanData) {
      socket.emit('output', cleanData);
    }
  });

  // Handle input from browser
  socket.on('input', (data) => {
    shell.write(data);
  });

  // Handle terminal resize
  socket.on('resize', (data) => {
    shell.resize(data.cols, data.rows);
  });

  // Handle disconnection
  socket.on('disconnect', () => {
    console.log('💔 Terminal disconnected:', socket.id);
    shell.kill();
  });

  // Handle shell exit
  shell.on('exit', (code) => {
    console.log('🐚 Minishell exited with code:', code);
    socket.emit('output', '\n🐚 Minishell session ended. Refresh to start new session.\n');
    socket.disconnect();
  });
});

// Start server
server.listen(PORT, () => {
  console.log(`🚀 Minishell Web Terminal running on port ${PORT}`);
  console.log(`🌐 Access your cloud shell at: http://localhost:${PORT}`);
  console.log(`🐚 Each connection gets its own minishell instance!`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('🛑 Shutting down web terminal server...');
  server.close(() => {
    process.exit(0);
  });
});
