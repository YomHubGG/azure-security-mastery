#!/bin/bash
# 🚀 Start Minishell Container
# Usage: ./start-minishell.sh

echo "🚀 Starting your Minishell container..."

CONTAINER_NAME="minishell-daily-$(date +%m%d)"
DNS_LABEL="minishell-$(openssl rand -hex 3)"

az container create \
  --resource-group rg-learning-day1 \
  --name "$CONTAINER_NAME" \
  --image devacr4uybw3c2lbkwm.azurecr.io/minishell-web:v8 \
  --registry-login-server devacr4uybw3c2lbkwm.azurecr.io \
  --registry-username devacr4uybw3c2lbkwm \
  --registry-password "$(az acr credential show --name devacr4uybw3c2lbkwm --query passwords[0].value --output tsv)" \
  --dns-name-label "$DNS_LABEL" \
  --ports 3000 \
  --environment-variables NODE_ENV=production PORT=3000 \
  --cpu 0.5 \
  --memory 1 \
  --query 'ipAddress.fqdn' \
  --output tsv

echo ""
echo "✅ Container started!"
echo "🌐 URL: http://$DNS_LABEL.westeurope.azurecontainer.io:3000"
echo "📊 Stats: http://$DNS_LABEL.westeurope.azurecontainer.io:3000/stats"
echo ""
echo "💡 To stop: ./stop-minishell.sh"
