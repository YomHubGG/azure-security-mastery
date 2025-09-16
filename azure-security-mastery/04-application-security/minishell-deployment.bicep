@description('Create a cloud-hosted Minishell with web terminal interface')
param containerName string = 'minishell-web-${uniqueString(resourceGroup().id)}'
param location string = resourceGroup().location
param containerRegistryName string = 'devacr4uybw3c2lbkwm'
param imageName string = 'minishell-web'
param imageTag string = 'v1'

@description('Container resource configuration')
param cpuCores string = '0.5'
param memoryInGb string = '1.0'
param port int = 3000

// Reference existing container registry
resource containerRegistry 'Microsoft.ContainerRegistry/registries@2023-01-01-preview' existing = {
  name: containerRegistryName
}

// Container Instance for web terminal
resource containerInstance 'Microsoft.ContainerInstance/containerGroups@2023-05-01' = {
  name: containerName
  location: location
  properties: {
    containers: [
      {
        name: 'minishell-web'
        properties: {
          image: '${containerRegistry.properties.loginServer}/${imageName}:${imageTag}'
          ports: [
            {
              port: port
              protocol: 'TCP'
            }
          ]
          resources: {
            requests: {
              cpu: json(cpuCores)
              memoryInGB: json(memoryInGb)
            }
          }
          environmentVariables: [
            {
              name: 'NODE_ENV'
              value: 'production'
            }
            {
              name: 'PORT'
              value: string(port)
            }
          ]
          livenessProbe: {
            httpGet: {
              path: '/health'
              port: port
              scheme: 'HTTP'
            }
            initialDelaySeconds: 30
            periodSeconds: 30
            timeoutSeconds: 5
            successThreshold: 1
            failureThreshold: 3
          }
          readinessProbe: {
            httpGet: {
              path: '/health'
              port: port
              scheme: 'HTTP'
            }
            initialDelaySeconds: 5
            periodSeconds: 10
            timeoutSeconds: 3
            successThreshold: 1
            failureThreshold: 3
          }
        }
      }
    ]
    imageRegistryCredentials: [
      {
        server: containerRegistry.properties.loginServer
        username: containerRegistry.listCredentials().username
        password: containerRegistry.listCredentials().passwords[0].value
      }
    ]
    ipAddress: {
      type: 'Public'
      ports: [
        {
          port: port
          protocol: 'TCP'
        }
      ]
      dnsNameLabel: 'minishell-${uniqueString(resourceGroup().id)}'
    }
    osType: 'Linux'
    restartPolicy: 'OnFailure'
  }
  tags: {
    project: 'cloud-minishell'
    environment: 'demo'
    purpose: '42-portfolio'
  }
}

// Output the connection information
output containerFQDN string = containerInstance.properties.ipAddress.fqdn
output containerIP string = containerInstance.properties.ipAddress.ip
output webTerminalUrl string = 'http://${containerInstance.properties.ipAddress.fqdn}:${port}'
output connectionInstructions string = 'Open ${containerInstance.properties.ipAddress.fqdn}:${port} in your browser to access the cloud minishell!'
