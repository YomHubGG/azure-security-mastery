# 🐚 Cloud Minishell Project

Welcome to the **Cloud Minishell** project! This is where we're taking your 42 school Minishell and making it absolutely EPIC by deploying it to the cloud with a slick web interface.

## 🎯 Project Overview

**What we're building:**
- Containerized version of your 42 Minishell
- Web-based terminal interface accessible from any browser
- Cloud deployment on Azure Container Instances
- Real-time terminal interaction via WebSockets

**Why this is awesome:**
- Impress your 42 teammates with cloud-deployed shell
- Portfolio piece showing containerization skills
- Practical application of Azure security concepts
- Browser-based access from anywhere

## 📁 Project Structure

```
04-application-security/
├── Dockerfile.minishell              # Containerize your minishell binary
├── Dockerfile.minishell-web          # Web terminal server container  
├── web-terminal-server.js            # Socket.io server for terminal
├── public/
│   └── terminal.html                 # Browser-based terminal UI
├── package.json                      # Node.js dependencies
└── minishell-deployment.bicep        # Azure deployment template
```

## 🚀 Quick Start

### 1. Prepare Your Minishell
First, you'll need your compiled minishell binary from your 42 project:

```bash
# Copy your minishell binary to this directory
cp /path/to/your/minishell ./minishell
chmod +x ./minishell
```

### 2. Build the Containers

```bash
# Build the minishell container
podman build -f Dockerfile.minishell -t minishell:latest .

# Build the web terminal server
podman build -f Dockerfile.minishell-web -t minishell-web:latest .
```

### 3. Test Locally

```bash
# Run the web terminal server
podman run -p 3000:3000 minishell-web:latest

# Open browser to http://localhost:3000
```

### 4. Deploy to Azure

```bash
# Push to Azure Container Registry
az acr login --name devacr4uybw3c2lbkwm
podman tag minishell-web:latest devacr4uybw3c2lbkwm.azurecr.io/minishell-web:v1
podman push devacr4uybw3c2lbkwm.azurecr.io/minishell-web:v1

# Deploy with Bicep
az deployment group create \
  --resource-group learn-security-rg \
  --template-file minishell-deployment.bicep
```

## 🎨 Features

### Web Terminal Interface
- **Real-time interaction**: Socket.io for instant command execution
- **Full keyboard support**: Arrow keys, Ctrl+C, Tab completion
- **Authentic terminal feel**: Cursor blinking, proper scrolling
- **Mobile-friendly**: Responsive design for any device

### Container Security
- **Multi-stage builds**: Minimal runtime image
- **Non-root user**: Security best practices
- **Alpine Linux**: Lightweight and secure base
- **Health checks**: Ensure service reliability

### Azure Integration
- **Container Instances**: Serverless container hosting
- **Container Registry**: Private image storage
- **Resource Groups**: Organized resource management
- **Cost optimization**: Pay only for running time

## 🛠️ Development Workflow

### Local Development
1. Edit `web-terminal-server.js` for server changes
2. Modify `public/terminal.html` for UI improvements
3. Test with `node web-terminal-server.js`
4. Rebuild containers when ready

### Cloud Deployment
1. Build and tag new container versions
2. Push to Azure Container Registry
3. Update Bicep template if needed
4. Deploy with Azure CLI

## 🏆 Impressing Your Teammates

**Demo Script:**
1. "Hey everyone, remember our minishell from 42?"
2. "Well, I deployed it to the cloud!" *opens browser*
3. "Now anyone can access it from anywhere!" *types commands*
4. "It's running in a secure container on Azure!" *shows real-time execution*
5. "And it cost me basically nothing!" *mentions serverless billing*

**Technical Highlights:**
- Containerization with Docker/Podman
- WebSocket real-time communication
- Cloud deployment with Azure
- Infrastructure as Code with Bicep
- Security best practices throughout

## 📚 What You're Learning

### Cloud Security Concepts
- Container security and hardening
- Network isolation in cloud environments
- Secrets management (when we add authentication)
- Infrastructure as Code principles

### DevOps Skills
- Containerization strategies
- CI/CD pipeline thinking
- Cloud resource management
- Monitoring and logging

### Full-Stack Development
- WebSocket programming
- Node.js server development
- Frontend JavaScript
- System integration

## 🔧 Troubleshooting

### Common Issues
- **Port conflicts**: Use different port with `-p 3001:3000`
- **Binary not found**: Ensure minishell is executable and in correct path
- **Azure login**: Check `az account show` for correct subscription
- **Container fails**: Check logs with `podman logs <container-id>`

### Debugging Commands
```bash
# Check container status
podman ps -a

# View container logs
podman logs <container-name>

# Access container shell
podman exec -it <container-name> /bin/sh

# Test network connectivity
curl http://localhost:3000/health
```

## 🎯 Next Steps

1. **Add authentication**: Integrate with Azure AD
2. **File persistence**: Mount Azure File Share
3. **Multi-user support**: Session management
4. **Monitoring**: Add Application Insights
5. **SSL/TLS**: Custom domain with certificates

## 🌟 Success Metrics

- [x] Minishell running in container
- [x] Web interface accessible
- [x] Real-time command execution
- [ ] Deployed to Azure
- [ ] Teammates amazed
- [ ] Portfolio enhanced

---

**Remember**: This project showcases your ability to take a school assignment and turn it into a professional cloud application. That's the kind of thinking that gets you hired! 🚀
