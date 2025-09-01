// Azure Container Instance - Deploy secure container to cloud
param location string = resourceGroup().location
param environmentPrefix string = 'dev'
param containerInstanceName string = '${environmentPrefix}-secure-container'
param acrName string = 'devacr4uybw3c2lbkwm' // Your existing ACR
param imageName string = 'secure-app:v1'

// Container Instance with VNet integration
resource containerInstance 'Microsoft.ContainerInstance/containerGroups@2023-05-01' = {
  name: containerInstanceName
  location: location
  properties: {
    containers: [
      {
        name: 'secure-app'
        properties: {
          image: '${acrName}.azurecr.io/${imageName}'
          resources: {
            requests: {
              cpu: 1
              memoryInGB: 1
            }
          }
          ports: [
            {
              port: 3000
              protocol: 'TCP'
            }
          ]
          environmentVariables: [
            {
              name: 'NODE_ENV'
              value: environmentPrefix
            }
            {
              name: 'PORT'
              value: '3000'
            }
          ]
        }
      }
    ]
    osType: 'Linux'
    restartPolicy: 'Always'
    ipAddress: {
      type: 'Public'
      ports: [
        {
          port: 3000
          protocol: 'TCP'
        }
      ]
    }
    imageRegistryCredentials: [
      {
        server: '${acrName}.azurecr.io'
        username: acrName
        password: listCredentials(resourceId('Microsoft.ContainerRegistry/registries', acrName), '2023-01-01-preview').passwords[0].value
      }
    ]
  }
}

// Outputs
output containerIP string = containerInstance.properties.ipAddress.ip
output containerUrl string = 'http://${containerInstance.properties.ipAddress.ip}:3000'
