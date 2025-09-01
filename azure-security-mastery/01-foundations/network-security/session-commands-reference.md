# Azure Daily Commands Reference

## Essential Commands
```bash
# Authentication
az login --use-device-code
az account show

# Resources
az resource list --resource-group rg-learning-day1 --output table

# Bicep Operations  
az deployment group validate --resource-group rg-learning-day1 --template-file template.bicep --parameters @params.json
az deployment group what-if --resource-group rg-learning-day1 --template-file template.bicep --parameters @params.json  
az deployment group create --resource-group rg-learning-day1 --template-file template.bicep --parameters @params.json
```

## REST API (When CLI Breaks)
```bash
# VNet Info
az rest --method GET --url "https://management.azure.com/.../virtualNetworks/vnet-name?api-version=2023-04-01" --query "properties.addressSpace.addressPrefixes[0]" --output tsv

# NSG Rules
az rest --method GET --url "https://management.azure.com/.../networkSecurityGroups/nsg-name?api-version=2023-04-01" --query "properties.securityRules[].name" --output tsv
```

*Updated: September 1, 2025*
