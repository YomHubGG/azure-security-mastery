# Cross-Premises Connectivity: VPN vs ExpressRoute

## 🌐 The Connectivity Challenge

When building hybrid cloud architectures, you need secure, reliable connectivity between on-premises networks and Azure. Two main options exist, each with distinct trade-offs.

## 🔒 Option 1: VPN Gateway (Site-to-Site)

### Architecture Overview

```
[On-Premises Data Center]
├── Corporate Network (10.0.0.0/8)
├── Local VPN Device (Cisco, Fortinet, etc.)
│   └── Public IP: 203.0.113.10
│
│     [Internet - Encrypted IPsec Tunnel]
│          ↓
[Azure VPN Gateway]
├── Gateway Subnet (10.1.255.0/27)
├── Public IP: 20.50.100.200
└── Connected to [Azure Virtual Network]
          ├── Web Tier Subnet (10.1.1.0/24)
          ├── App Tier Subnet (10.1.2.0/24)
          └── Data Tier Subnet (10.1.3.0/24)
```

### How It Works

**Step-by-Step Connection**:
1. Azure VPN Gateway listens on public IP
2. On-prem VPN device initiates IPsec tunnel
3. IKE (Internet Key Exchange) negotiation
4. IPsec tunnel established with encryption
5. Traffic flows through encrypted tunnel
6. Both sides route traffic to each other

**Encryption**:
- **Phase 1 (IKE)**: AES-256, SHA-256
- **Phase 2 (IPsec)**: AES-256-GCM, SHA-256
- **Perfect Forward Secrecy (PFS)**: Enabled

### VPN Gateway SKUs

| SKU | Speed | S2S Tunnels | P2S Connections | Cost/Month | Use Case |
|-----|-------|-------------|-----------------|------------|----------|
| **Basic** | 100 Mbps | 10 | 128 | ~€25 | Dev/Test only |
| **VpnGw1** | 650 Mbps | 30 | 250 | ~€100 | Small prod |
| **VpnGw2** | 1 Gbps | 30 | 500 | ~€250 | Medium prod |
| **VpnGw3** | 1.25 Gbps | 30 | 1000 | ~€600 | Large prod |
| **VpnGw1AZ** | 650 Mbps | 30 | 250 | ~€120 | Zone-redundant |
| **VpnGw2AZ** | 1 Gbps | 30 | 500 | ~€300 | Zone-redundant |

**Note**: Basic SKU doesn't support BGP (dynamic routing) - avoid for production

### Supported VPN Devices

**On-Premises VPN Hardware** (100+ validated):
- ✅ Cisco ASA, ISR, Meraki
- ✅ Fortinet FortiGate
- ✅ Palo Alto Networks
- ✅ Juniper SRX, SSG
- ✅ pfSense, OPNsense (open-source)
- ✅ Azure Stack Edge

**Software-Based**:
- ✅ Windows Server RRAS
- ✅ Linux strongSwan
- ✅ Ubuntu Cloud Gateway

[Full compatibility list](https://docs.microsoft.com/azure/vpn-gateway/vpn-gateway-about-vpn-devices)

### Deployment Process (Conceptual)

**Step 1: Create Virtual Network**
```bash
az network vnet create \
  --resource-group rg-hybrid \
  --name vnet-onprem-connection \
  --address-prefix 10.1.0.0/16 \
  --location westeurope
```

**Step 2: Create Gateway Subnet**
```bash
# Special subnet name required: "GatewaySubnet"
az network vnet subnet create \
  --resource-group rg-hybrid \
  --vnet-name vnet-onprem-connection \
  --name GatewaySubnet \
  --address-prefix 10.1.255.0/27
```

**Step 3: Create Public IP**
```bash
az network public-ip create \
  --resource-group rg-hybrid \
  --name pip-vpn-gateway \
  --allocation-method Static \
  --sku Standard
```

**Step 4: Create VPN Gateway** (⚠️ Takes 30-45 minutes!)
```bash
az network vnet-gateway create \
  --resource-group rg-hybrid \
  --name vgw-onprem-connection \
  --public-ip-address pip-vpn-gateway \
  --vnet vnet-onprem-connection \
  --gateway-type Vpn \
  --vpn-type RouteBased \
  --sku VpnGw1 \
  --no-wait  # Don't wait for completion
```

**Step 5: Create Local Network Gateway** (represents on-prem)
```bash
az network local-gateway create \
  --resource-group rg-hybrid \
  --name lgw-onprem-datacenter \
  --gateway-ip-address 203.0.113.10 \  # On-prem public IP
  --local-address-prefixes 10.0.0.0/8   # On-prem network range
```

**Step 6: Create VPN Connection**
```bash
az network vpn-connection create \
  --resource-group rg-hybrid \
  --name cn-onprem-to-azure \
  --vnet-gateway1 vgw-onprem-connection \
  --local-gateway2 lgw-onprem-datacenter \
  --location westeurope \
  --shared-key "SuperSecurePreSharedKey123!"
```

**Step 7: Configure On-Prem VPN Device**
```
Download configuration from Azure Portal:
VPN Gateway → Connections → Download Configuration
→ Select your device (Cisco, FortiGate, etc.)
→ Apply configuration to on-prem device
```

### Security Considerations

**Encryption Best Practices**:
- ✅ Use strong pre-shared keys (32+ characters, random)
- ✅ Enable Perfect Forward Secrecy (PFS)
- ✅ Use AES-256-GCM for encryption
- ✅ Implement IKEv2 (not IKEv1)
- ✅ Rotate keys regularly (quarterly)

**Network Security**:
- ✅ Apply NSGs to subnets (not GatewaySubnet)
- ✅ Use Azure Firewall for egress filtering
- ✅ Enable DDoS Protection on public IPs
- ✅ Monitor with Network Watcher
- ✅ Log all connection attempts

**Monitoring**:
```bash
# Check VPN connection status
az network vpn-connection show \
  --resource-group rg-hybrid \
  --name cn-onprem-to-azure \
  --query connectionStatus

# View metrics
az monitor metrics list \
  --resource /subscriptions/.../vpn-connection \
  --metric "TunnelAverageBandwidth"
```

### Pros and Cons

**Advantages** ✅:
- **Cost-effective**: €25-€600/month vs €500-€5,000+ for ExpressRoute
- **Quick setup**: Hours, not weeks
- **Flexible**: Any internet connection works
- **No provider dependencies**: Direct Azure deployment
- **Good for**: Dev/test, small offices, backup connectivity

**Disadvantages** ❌:
- **Internet-based**: Subject to internet congestion
- **Higher latency**: Variable (10-50ms typical)
- **Lower throughput**: Max 1.25 Gbps (VpnGw3)
- **Shared medium**: Bandwidth shared with internet traffic
- **Security perception**: Some orgs distrust internet-based VPN

### Use Cases

**Perfect For**:
- ✅ Dev/test environments
- ✅ Small branch offices
- ✅ Backup for ExpressRoute
- ✅ Low-volume data transfers
- ✅ Budget-conscious deployments

**Not Recommended For**:
- ❌ High-throughput workloads (>1 Gbps)
- ❌ Latency-sensitive applications (<10ms required)
- ❌ 24/7 production with SLA requirements
- ❌ Transferring TB+ of data regularly

---

## 🚀 Option 2: Azure ExpressRoute

### Architecture Overview

```
[On-Premises Data Center]
├── Corporate Network (10.0.0.0/8)
├── Edge Router (BGP enabled)
│   └── Private connection to provider
│
│   [Connectivity Provider - Private Link]
│   (Equinix, Megaport, AT&T, etc.)
│          ↓
[ExpressRoute Circuit]
├── Private Peering → [Azure VNets]
├── Microsoft Peering → [Office 365, Dynamics 365]
└── Primary/Secondary links (redundancy)
       ↓
[Azure Virtual Networks]
├── Production Workloads
├── Databases
└── Enterprise Applications
```

### How It Works

**Physical Connection**:
1. Your datacenter connects to colocation facility (Equinix, etc.)
2. Provider provisions dedicated circuit to Microsoft
3. You configure BGP peering with Azure
4. Traffic flows over private fiber (never touches internet)

**Peering Types**:

1. **Private Peering**: Access Azure VNets
   - IaaS resources (VMs, Storage)
   - PaaS services with Private Link
   - Private IP addressing

2. **Microsoft Peering**: Access Microsoft services
   - Office 365 (Exchange, SharePoint, Teams)
   - Dynamics 365
   - Azure public services (Cosmos DB, etc.)
   - Public IP addressing

### ExpressRoute SKUs

| SKU | Bandwidth | Monthly Cost | Provider Fee | Total Cost/Month |
|-----|-----------|--------------|--------------|------------------|
| **50 Mbps** | 50 Mbps | ~€50 | €300-500 | ~€400-600 |
| **100 Mbps** | 100 Mbps | ~€500 | €400-600 | ~€900-1,100 |
| **200 Mbps** | 200 Mbps | ~€750 | €500-700 | ~€1,250-1,450 |
| **500 Mbps** | 500 Mbps | ~€1,000 | €700-1,000 | ~€1,700-2,000 |
| **1 Gbps** | 1 Gbps | ~€1,500 | €1,000-1,500 | ~€2,500-3,000 |
| **2 Gbps** | 2 Gbps | ~€2,000 | €1,500-2,000 | ~€3,500-4,000 |
| **5 Gbps** | 5 Gbps | ~€4,000 | €2,000-3,000 | ~€6,000-7,000 |
| **10 Gbps** | 10 Gbps | ~€5,000 | €3,000-5,000 | ~€8,000-10,000 |

**Additional Costs**:
- Outbound data transfer (€0.02-0.08/GB)
- ExpressRoute Gateway (€180-€1,400/month)
- Provider setup fees (one-time: €500-€5,000)

### Deployment Process (Conceptual)

**Step 1: Choose Provider & Location**
```bash
# List available providers
az network express-route list-service-providers -o table

# Example output:
# Provider: Equinix
# Peering Location: Amsterdam, London, Paris
# Bandwidths: 50Mbps, 100Mbps, 500Mbps, 1Gbps, 10Gbps
```

**Step 2: Create ExpressRoute Circuit**
```bash
az network express-route create \
  --resource-group rg-hybrid \
  --name er-onprem-circuit \
  --provider "Equinix" \
  --peering-location "Amsterdam" \
  --bandwidth 500 \  # Mbps
  --sku-family MeteredData \
  --sku-tier Standard
```

**Step 3: Get Service Key** (give to provider)
```bash
az network express-route show \
  --resource-group rg-hybrid \
  --name er-onprem-circuit \
  --query serviceKey
```

**Step 4: Provider Provisions Circuit** (1-4 weeks)
- Provider configures physical connection
- Validates circuit connectivity
- Notifies you when ready

**Step 5: Create ExpressRoute Gateway**
```bash
# In target VNet
az network vnet-gateway create \
  --resource-group rg-hybrid \
  --name ergw-production \
  --vnet vnet-production \
  --public-ip-address pip-er-gateway \
  --gateway-type ExpressRoute \
  --sku Standard  # or HighPerformance, UltraPerformance
```

**Step 6: Link Circuit to Gateway**
```bash
az network vpn-connection create \
  --resource-group rg-hybrid \
  --name cn-onprem-er \
  --vnet-gateway1 ergw-production \
  --express-route-circuit2 er-onprem-circuit \
  --location westeurope
```

### Security Considerations

**Physical Security**:
- Circuit is private (not on public internet)
- Dedicated fiber or wavelength
- Provider-managed physical security

**Encryption Options**:

1. **MACsec** (Layer 2 encryption):
   - Encrypts ExpressRoute connection itself
   - Available on ExpressRoute Direct only
   - Requires compatible hardware

2. **IPsec over ExpressRoute**:
   - Add VPN tunnel on top of ExpressRoute
   - Encrypted end-to-end
   - Lower throughput (VPN overhead)

3. **Application-level encryption** (recommended):
   - TLS for web traffic
   - Encrypted databases
   - VPN for specific apps

**Microsoft's Position**:
> "ExpressRoute is private and physically secure. Most customers don't encrypt the ExpressRoute connection itself but rely on application-level encryption (TLS, SSL)."

**Network Security**:
- ✅ Implement route filters (control which services accessible)
- ✅ Use NSGs on subnets
- ✅ Deploy Azure Firewall for inspection
- ✅ Enable DDoS Protection Standard
- ✅ Monitor with Network Watcher and NSG Flow Logs

### Pros and Cons

**Advantages** ✅:
- **Predictable performance**: Dedicated bandwidth, not shared
- **Low latency**: 1-5ms typical (vs 10-50ms for VPN)
- **High throughput**: Up to 100 Gbps (ExpressRoute Direct)
- **Private connection**: Never traverses public internet
- **99.95% SLA**: Same as VPN but more reliable in practice
- **Better for compliance**: Some regulations prefer private links

**Disadvantages** ❌:
- **Expensive**: 10-50x cost of VPN Gateway
- **Long setup time**: Weeks to months (provider coordination)
- **Provider dependency**: Tied to provider's infrastructure
- **Complex**: More moving parts (circuit, provider, BGP)
- **Geographic limitations**: Limited peering locations

### Use Cases

**Perfect For**:
- ✅ Production workloads with SLA requirements
- ✅ Latency-sensitive applications (<10ms)
- ✅ High-throughput requirements (>1 Gbps sustained)
- ✅ Large data transfers (TB+ regularly)
- ✅ Enterprise Office 365 (thousands of users)
- ✅ Mission-critical applications

**Not Recommended For**:
- ❌ Dev/test environments (overkill)
- ❌ Small offices (<100 users)
- ❌ Budget-constrained projects
- ❌ Temporary/short-term needs

---

## 📊 VPN vs ExpressRoute: Decision Matrix

| Factor | VPN Gateway | ExpressRoute | Winner |
|--------|-------------|--------------|--------|
| **Cost** | €25-€600/mo | €400-€10,000/mo | 🏆 VPN |
| **Setup Time** | Hours | Weeks/Months | 🏆 VPN |
| **Bandwidth** | 100Mbps-1.25Gbps | 50Mbps-100Gbps | 🏆 ExpressRoute |
| **Latency** | 10-50ms | 1-5ms | 🏆 ExpressRoute |
| **Reliability** | Good (99.95%) | Excellent (99.95%+) | 🏆 ExpressRoute |
| **Security** | IPsec encrypted | Private (no encryption) | 🤔 Depends |
| **Complexity** | Low | High | 🏆 VPN |
| **Flexibility** | High (any ISP) | Low (specific providers) | 🏆 VPN |
| **Provider Dependency** | None | High | 🏆 VPN |

### Hybrid Approach: VPN + ExpressRoute

**Best Practice for Enterprise**:
```
Primary: ExpressRoute (production traffic)
Backup: VPN Gateway (failover)
```

**Architecture**:
```
[On-Premises]
├── Primary Route: ExpressRoute (99.95% uptime)
│   ├── BGP metric: 100 (preferred)
│   └── Production traffic flows here
│
└── Backup Route: VPN Gateway (99.95% uptime)
    ├── BGP metric: 200 (backup)
    └── Traffic fails over if ExpressRoute down
```

**Combined SLA**: 99.99%+ (both must fail simultaneously)

**Cost**: ExpressRoute cost + ~€100/month for VPN backup

---

## 🏢 Real-World Scenarios

### Scenario 1: Startup → Scale-up

**Phase 1: Startup (10 employees)**
- Solution: VPN Gateway Basic (€25/month)
- Use case: Dev/test, occasional Azure access
- Decision: Cost matters more than performance

**Phase 2: Growing (50 employees)**
- Solution: VPN Gateway VpnGw1 (€100/month)
- Use case: Light production workloads
- Decision: Still cost-sensitive, good enough performance

**Phase 3: Enterprise (500 employees)**
- Solution: ExpressRoute 500Mbps + VPN backup (€2,000/month)
- Use case: Mission-critical production
- Decision: Performance and SLA justify cost

### Scenario 2: European Bank

**Requirements**:
- Latency <5ms (trading systems)
- Bandwidth 10+ Gbps (data replication)
- 99.99% SLA (regulatory)
- GDPR compliance (data sovereignty)

**Solution**:
- Primary: ExpressRoute 10Gbps (€8,000/month)
- Backup: ExpressRoute 1Gbps (€2,500/month)
- Dual circuits to different providers (resilience)
- Private Peering only (no internet breakout)

**Cost**: ~€10,500/month (justified for trading revenue)

### Scenario 3: Healthcare Provider

**Requirements**:
- Patient data stays on-premises
- Cloud for AI/analytics only
- Budget-conscious
- HIPAA compliance

**Solution**:
- VPN Gateway VpnGw2AZ (€300/month)
- Zone-redundant for high availability
- IPsec + application-level encryption
- Transfer only de-identified data to cloud

**Cost**: €300/month (fits healthcare budget)

---

## 🎓 Portfolio Talking Points

### What You've Learned

1. ✅ **VPN Gateway architecture and use cases**
   - Site-to-Site IPsec connectivity
   - SKU comparison and cost optimization
   - Security configuration and monitoring

2. ✅ **ExpressRoute architecture and use cases**
   - Private peering vs Microsoft peering
   - Provider ecosystem and deployment process
   - Cost analysis and ROI justification

3. ✅ **Decision framework for enterprises**
   - When to use VPN (dev/test, cost-sensitive)
   - When to use ExpressRoute (production, SLA-critical)
   - Hybrid approach (ExpressRoute + VPN backup)

4. ✅ **Real-world enterprise patterns**
   - Startup scaling journey
   - Financial services requirements
   - Healthcare compliance scenarios

### Interview Answers You Can Give

**Q**: "Should we use VPN or ExpressRoute for our hybrid cloud?"

**A**: "It depends on requirements. For dev/test or small offices under 100 users, VPN Gateway at €100-€300/month is cost-effective with decent performance. For production workloads needing <10ms latency or >1Gbps sustained throughput, ExpressRoute justifies its €2,000-€5,000/month cost. Best practice for enterprise is ExpressRoute primary with VPN backup for 99.99% combined SLA."

**Q**: "How would you secure a VPN connection to Azure?"

**A**: "I'd use strong pre-shared keys (32+ chars), enable Perfect Forward Secrecy, configure AES-256-GCM encryption with IKEv2. On Azure side, apply NSGs to subnets, deploy Azure Firewall for egress filtering, enable Network Watcher for monitoring. Key is defense-in-depth: encryption in transit, access controls, and continuous monitoring."

---

**Next**: Creating the comprehensive learning summary for Day 31
