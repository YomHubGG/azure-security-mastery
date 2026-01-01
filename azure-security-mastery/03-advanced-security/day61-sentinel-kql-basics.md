# Day 61: Microsoft Sentinel & KQL Basics

**Date**: January 1, 2026  
**Focus**: SIEM concepts, KQL query language, threat detection fundamentals  
**Cost**: Free tier (500 MB/day Log Analytics ingestion)

---

## 📚 Learning Objectives

1. Understand SIEM (Security Information and Event Management) concepts
2. Master KQL (Kusto Query Language) fundamentals
3. Analyze Azure activity logs for security insights
4. Create basic threat detection queries
5. Understand log retention and cost optimization

---

## 🔍 What is Microsoft Sentinel?

**Microsoft Sentinel** is a cloud-native SIEM + SOAR solution:
- **SIEM**: Security Information and Event Management (collect, analyze, detect)
- **SOAR**: Security Orchestration, Automation, and Response (automate responses)

### Key Components
```
┌─────────────────────────────────────────────────────────┐
│                   Microsoft Sentinel                     │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  📥 Data Connectors                                     │
│  ├── Azure Activity Logs                                │
│  ├── Azure AD Sign-in Logs                              │
│  ├── Microsoft Defender alerts                          │
│  ├── Third-party sources (firewalls, IDS, etc.)        │
│  └── Custom logs via API                                │
│                                                          │
│  🔎 Log Analytics Workspace (storage + queries)         │
│  ├── KQL query engine                                   │
│  ├── Time-series data storage                           │
│  └── 31-90 day retention (configurable)                 │
│                                                          │
│  🎯 Analytics Rules (detection logic)                   │
│  ├── Scheduled queries (every X minutes/hours)          │
│  ├── Microsoft templates (800+ built-in)                │
│  ├── Machine learning anomaly detection                 │
│  └── Fusion AI (multi-stage attack detection)           │
│                                                          │
│  🚨 Incidents (correlated alerts)                       │
│  ├── Multiple alerts → single incident                  │
│  ├── Investigation graph (entity relationships)         │
│  └── Case management workflow                           │
│                                                          │
│  🤖 Playbooks (automation via Logic Apps)               │
│  ├── Auto-response to incidents                         │
│  ├── Ticket creation (ServiceNow, Jira)                 │
│  └── Enrichment (threat intelligence, IP reputation)    │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### Free Tier Benefits
- **500 MB/day** data ingestion (enough for learning)
- **31 days** default retention
- Access to all analytics features
- Basic automation capabilities
- No credit card trial needed

### When to Upgrade (Not Now!)
- **Standard tier**: $2.76/GB ingestion (avoid until production)
- Needed for: >500 MB/day, longer retention, advanced ML features

---

## 📖 KQL (Kusto Query Language) Fundamentals

### What is KQL?

**KQL** is a read-only query language optimized for:
- Fast time-series data analysis
- Log pattern recognition
- Security event correlation
- Real-time monitoring

### KQL vs SQL Comparison

| Feature | KQL | SQL |
|---------|-----|-----|
| **Syntax** | Pipe-based (`\|`) | Nested (`SELECT FROM WHERE`) |
| **Data** | Time-series logs | Relational tables |
| **Use Case** | Analytics, monitoring | Transactional, CRUD |
| **Performance** | Optimized for big data | Optimized for ACID |
| **Learning Curve** | Medium (unique syntax) | Easy (universal) |

**Example Comparison**:

```sql
-- SQL Style
SELECT UserPrincipalName, COUNT(*) as FailedAttempts
FROM SigninLogs
WHERE ResultType != '0'
  AND TimeGenerated > DATEADD(hour, -24, GETDATE())
GROUP BY UserPrincipalName
ORDER BY FailedAttempts DESC;
```

```kql
// KQL Style (more readable for logs!)
SigninLogs
| where TimeGenerated > ago(24h)
| where ResultType != "0"
| summarize FailedAttempts = count() by UserPrincipalName
| order by FailedAttempts desc
```

---

## 🎓 KQL Core Concepts

### 1. Table-Based Queries

Every query starts with a **table name** (data source):

```kql
// Most common security tables
AzureActivity          // Resource creation, deletion, modifications
SigninLogs             // Azure AD authentication events
AuditLogs              // Azure AD configuration changes
SecurityAlert          // Defender for Cloud alerts
SecurityEvent          // Windows security events (VMs)
Syslog                 // Linux system logs
CommonSecurityLog      // Firewall, IDS/IPS logs
ThreatIntelligenceIndicator  // Known malicious IPs/domains
```

### 2. Pipe Operator (`|`)

KQL uses **pipes** to chain operations (like Linux bash):

```kql
TableName
| operation1
| operation2
| operation3
```

### 3. Time Filtering (`where`, `ago()`)

**Critical for cost optimization** - always filter by time first!

```kql
// ✅ GOOD: Time filter first (scans less data)
AzureActivity
| where TimeGenerated > ago(1h)
| where OperationName == "Delete Virtual Machine"

// ❌ BAD: Time filter last (scans entire table)
AzureActivity
| where OperationName == "Delete Virtual Machine"
| where TimeGenerated > ago(1h)
```

**Time Functions**:
```kql
ago(1h)                    // 1 hour ago
ago(24h)                   // 24 hours ago (yesterday)
ago(7d)                    // 7 days ago
startofday(now())          // Today at 00:00
startofweek(now())         // Start of current week
between(datetime(2026-01-01) .. datetime(2026-01-31))  // Date range
```

### 4. Filtering (`where`)

```kql
// Equality
| where Level == "Error"
| where StatusCode != 200

// Numeric comparisons
| where ResponseTime > 1000
| where FailedAttempts >= 5

// String matching
| where Message contains "authentication failed"
| where Message startswith "ERROR"
| where Message endswith "timeout"
| where Message matches regex @"\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}"  // IP address

// Multiple conditions
| where Level == "Error" and TimeGenerated > ago(1h)
| where StatusCode == 401 or StatusCode == 403
```

### 5. Aggregation (`summarize`)

Group data and calculate metrics:

```kql
// Count events
| summarize count() by OperationName

// Multiple aggregations
| summarize 
    EventCount = count(),
    UniqueUsers = dcount(UserPrincipalName),
    AvgDuration = avg(DurationMs),
    MaxSize = max(ContentLength)
  by ResourceGroup

// Time-based aggregation (bin = bucket)
| summarize count() by bin(TimeGenerated, 1h)  // Events per hour
| summarize count() by bin(TimeGenerated, 5m)  // Events per 5 minutes
```

### 6. Sorting (`sort`, `order`, `top`)

```kql
// Sort ascending
| sort by TimeGenerated asc

// Sort descending (default)
| order by FailedAttempts desc

// Top N results
| top 10 by FailedAttempts desc
```

### 7. Projection (`project`, `extend`)

Select and create columns:

```kql
// Select specific columns
| project TimeGenerated, UserPrincipalName, IPAddress, Location

// Create new columns
| extend Hour = hourofday(TimeGenerated)
| extend IsWeekend = dayofweek(TimeGenerated) in (0d, 6d)  // Sunday, Saturday
| extend RiskScore = FailedAttempts * 10
```

### 8. Joining Data (`join`)

Correlate data across tables:

```kql
SigninLogs
| where ResultType != "0"  // Failed sign-ins
| summarize FailedAttempts = count() by UserPrincipalName
| join kind=inner (
    AuditLogs
    | where OperationName == "Add user"
    | project UserPrincipalName, AccountCreatedTime = TimeGenerated
  ) on UserPrincipalName
| where FailedAttempts > 5
| project UserPrincipalName, FailedAttempts, AccountCreatedTime
```

---

## 🔥 Practical Security Queries

### Query 1: Failed Sign-In Attempts (Brute Force Detection)

```kql
// Detect accounts with multiple failed authentication attempts
SigninLogs
| where TimeGenerated > ago(24h)
| where ResultType != "0"  // 0 = success, anything else = failure
| summarize 
    FailedAttempts = count(),
    UniqueIPs = dcount(IPAddress),
    DistinctLocations = make_set(Location, 5)
  by UserPrincipalName
| where FailedAttempts > 5
| order by FailedAttempts desc
| project UserPrincipalName, FailedAttempts, UniqueIPs, DistinctLocations
```

**What to look for**:
- `FailedAttempts > 10`: Possible brute force
- `UniqueIPs > 3`: Distributed attack
- `DistinctLocations`: Geographic anomaly

### Query 2: Successful Sign-Ins After Failed Attempts

```kql
// Detect successful breach after brute force attempts
let FailedLogins = SigninLogs
  | where TimeGenerated > ago(24h)
  | where ResultType != "0"
  | summarize FailedCount = count() by UserPrincipalName;
SigninLogs
| where TimeGenerated > ago(24h)
| where ResultType == "0"  // Successful login
| join kind=inner (FailedLogins) on UserPrincipalName
| where FailedCount > 5
| project TimeGenerated, UserPrincipalName, IPAddress, Location, FailedCount
| order by FailedCount desc
```

**Threat Indicator**: Account with 5+ failures then successful login = possible compromise

### Query 3: Resource Deletion Events

```kql
// Monitor for unauthorized resource deletions
AzureActivity
| where TimeGenerated > ago(7d)
| where OperationName contains "Delete"
| where ActivityStatus == "Succeeded"
| summarize 
    DeleteCount = count(),
    DeletedResources = make_list(ResourceId, 10)
  by Caller, OperationName
| order by DeleteCount desc
| project Caller, OperationName, DeleteCount, DeletedResources
```

**Use Case**: Detect insider threats, account compromise, or accidental deletions

### Query 4: Privilege Escalation Detection

```kql
// Monitor role assignments (admin rights granted)
AuditLogs
| where TimeGenerated > ago(24h)
| where OperationName in ("Add member to role", "Add eligible member to role")
| where Result == "success"
| extend RoleAssigned = tostring(TargetResources[0].modifiedProperties[0].newValue)
| where RoleAssigned contains "Admin" or RoleAssigned contains "Owner"
| project TimeGenerated, Identity, TargetUser = TargetResources[0].userPrincipalName, RoleAssigned
| order by TimeGenerated desc
```

**High-Risk Roles**: Global Administrator, Security Administrator, Privileged Role Administrator

### Query 5: Suspicious IP Detection

```kql
// Detect logins from known suspicious IPs (simplified example)
let SuspiciousIPs = datatable(IPAddress:string) [
    "192.0.2.1",
    "198.51.100.1",
    "203.0.113.1"
];
SigninLogs
| where TimeGenerated > ago(24h)
| join kind=inner (SuspiciousIPs) on IPAddress
| project TimeGenerated, UserPrincipalName, IPAddress, Location, AppDisplayName
```

**Production Enhancement**: Replace with ThreatIntelligenceIndicator table (real threat feeds)

### Query 6: Anomalous Sign-In Locations

```kql
// Detect logins from unusual countries
SigninLogs
| where TimeGenerated > ago(30d)
| summarize Countries = make_set(Location) by UserPrincipalName
| extend CountryCount = array_length(Countries)
| where CountryCount > 3  // User logged in from 4+ countries
| join kind=inner (
    SigninLogs
    | where TimeGenerated > ago(24h)
    | project TimeGenerated, UserPrincipalName, Location, IPAddress
  ) on UserPrincipalName
| project TimeGenerated, UserPrincipalName, CurrentLocation = Location, 
          AllCountries = Countries, TotalCountries = CountryCount
| order by TimeGenerated desc
```

**Threat**: Credential sharing, account compromise, or impossible travel

### Query 7: Off-Hours Access

```kql
// Detect logins outside business hours (18:00 - 06:00 UTC)
SigninLogs
| where TimeGenerated > ago(7d)
| extend Hour = hourofday(TimeGenerated)
| where Hour >= 18 or Hour <= 6  // After 6 PM or before 6 AM
| where ResultType == "0"  // Successful only
| summarize LoginCount = count() by UserPrincipalName, Hour
| where LoginCount > 5
| order by LoginCount desc
```

**Context Required**: Adjust timezone for your organization

### Query 8: Service Principal (Non-Human) Activity

```kql
// Monitor automated service account activity
SigninLogs
| where TimeGenerated > ago(24h)
| where AppDisplayName contains "Service" or UserPrincipalName contains "@"
| summarize 
    Actions = count(),
    UniqueApps = dcount(AppDisplayName)
  by UserPrincipalName
| where Actions > 1000  // High-volume automated activity
| order by Actions desc
```

**Use Case**: Detect compromised service principals, API abuse

---

## 🎯 KQL Best Practices

### Performance Optimization

1. **Time filter first** (reduces data scanned):
   ```kql
   ✅ | where TimeGenerated > ago(1h)  // FIRST
   ✅ | where Level == "Error"
   ```

2. **Use `project` early** (reduces memory):
   ```kql
   ✅ | project TimeGenerated, User, Action  // Keep only needed columns
   ```

3. **Aggregate before join** (smaller datasets):
   ```kql
   ✅ | summarize count() by User
   ✅ | join (OtherTable) on User
   ```

4. **Avoid `contains` when possible** (slower than `==`):
   ```kql
   ✅ | where OperationName == "Delete"  // Fast exact match
   ⚠️  | where OperationName contains "Delete"  // Slower pattern match
   ```

### Cost Optimization

1. **Limit time ranges** (avoid scanning entire history):
   ```kql
   ✅ ago(1h)    // €0.00001 scan cost
   ⚠️  ago(30d)   // €0.001 scan cost
   ```

2. **Use `take` or `top` for testing**:
   ```kql
   ✅ | take 10   // Quick preview
   ```

3. **Monitor data ingestion** (check free tier limit):
   ```bash
   # Check daily ingestion volume
   az monitor log-analytics workspace get-schema \
     --resource-group rg-security-learning-001 \
     --workspace-name law-sentinel-001
   ```

### Security Best Practices

1. **Never log sensitive data** (PII, secrets, passwords)
2. **Use dynamic allow-lists** (not hardcoded IPs)
3. **Set alert thresholds** (avoid alert fatigue)
4. **Test queries before scheduling** (validate logic)
5. **Document query purpose** (maintainability)

---

## 📊 Common Log Tables Reference

### Azure Activity (AzureActivity)
**Purpose**: Track Azure resource operations  
**Key Fields**:
- `OperationName`: Action performed (Create, Delete, Update)
- `Caller`: Who performed the action
- `ResourceGroup`, `ResourceId`: What was affected
- `ActivityStatus`: Success, Failed

### Sign-In Logs (SigninLogs)
**Purpose**: Azure AD authentication events  
**Key Fields**:
- `UserPrincipalName`: User email
- `IPAddress`, `Location`: Where from
- `AppDisplayName`: Which application
- `ResultType`: 0 = success, else = error code
- `ConditionalAccessStatus`: Applied, Not Applied

### Audit Logs (AuditLogs)
**Purpose**: Azure AD configuration changes  
**Key Fields**:
- `OperationName`: What changed (Add user, Delete group)
- `InitiatedBy`: Who made the change
- `TargetResources`: What was affected
- `Result`: success, failure

### Security Alerts (SecurityAlert)
**Purpose**: Defender for Cloud alerts  
**Key Fields**:
- `AlertName`, `AlertSeverity`: Threat classification
- `CompromisedEntity`: Affected resource
- `RemediationSteps`: Recommended actions
- `ExtendedProperties`: Additional context

---

## 🧪 Hands-On Practice

### Exercise 1: Your First Query

Let's create a Log Analytics workspace and run your first query:

```bash
# Create resource group (if needed)
az group create \
  --name rg-sentinel-learning \
  --location westeurope

# Create Log Analytics workspace (FREE tier)
az monitor log-analytics workspace create \
  --resource-group rg-sentinel-learning \
  --workspace-name law-sentinel-learning \
  --location westeurope \
  --retention-time 31 \
  --sku PerGB2018  # Free tier with 500 MB/day
```

**Cost**: $0.00 (within free tier limit)

### Exercise 2: Query Azure Activity Logs

1. Enable Activity Log diagnostic settings:
```bash
# Get subscription ID
SUB_ID=$(az account show --query id -o tsv)

# Get workspace ID
WORKSPACE_ID=$(az monitor log-analytics workspace show \
  --resource-group rg-sentinel-learning \
  --workspace-name law-sentinel-learning \
  --query id -o tsv)

# Create diagnostic setting (sends Activity Logs to workspace)
az monitor diagnostic-settings create \
  --name send-to-sentinel \
  --resource "/subscriptions/$SUB_ID" \
  --logs '[{"category":"Administrative","enabled":true}]' \
  --workspace $WORKSPACE_ID
```

2. Wait 5-10 minutes for data ingestion
3. Run your first query in Azure Portal:
   - Navigate to Log Analytics workspace → Logs
   - Paste this query:

```kql
AzureActivity
| where TimeGenerated > ago(24h)
| summarize EventCount = count() by OperationName
| order by EventCount desc
| take 10
```

### Exercise 3: Create Your First Alert

**Scenario**: Alert on any VM deletion

```kql
// Detection query
AzureActivity
| where TimeGenerated > ago(5m)
| where OperationName == "Delete Virtual Machine"
| where ActivityStatus == "Succeeded"
| project TimeGenerated, Caller, ResourceId, ResourceGroup
```

**Alert Configuration**:
- **Frequency**: Every 5 minutes
- **Threshold**: 1 event
- **Action**: Email notification

---

## 📖 Additional Resources

### Microsoft Learn Paths
1. [KQL from scratch](https://docs.microsoft.com/learn/modules/write-first-query-kusto-query-language/)
2. [Microsoft Sentinel fundamentals](https://docs.microsoft.com/learn/paths/security-ops-sentinel/)
3. [Threat detection with Sentinel](https://docs.microsoft.com/learn/modules/detect-threats-sentinel/)

### Practice Platforms
- **Azure Data Explorer Demo**: https://dataexplorer.azure.com/clusters/help/databases/Samples
- **Microsoft Sentinel Training Lab**: https://github.com/Azure/Azure-Sentinel
- **KQL Cheat Sheet**: https://github.com/marcusbakker/KQL

### YouTube Channels
- John Savill's Technical Training (Azure security)
- Microsoft Security (official channel)
- Cloud Security Podcast

---

## ✅ Day 61 Success Criteria

By end of session, you should be able to:
- [ ] Explain SIEM vs SOAR concepts
- [ ] Write basic KQL queries (filter, aggregate, sort)
- [ ] Query Azure Activity and Sign-In logs
- [ ] Identify common attack patterns in logs
- [ ] Create a simple detection rule
- [ ] Understand free tier limitations (500 MB/day)
- [ ] Optimize queries for performance and cost

---

## 🎯 Next Steps (Day 63)

**Threat Intelligence Integration**:
- Connect external threat feeds (TAXII, STIX)
- Enrich alerts with IP reputation data
- Create watchlists for known bad actors
- Implement automated threat hunting

---

**Session**: #31  
**Cost**: €0.00 (free tier)  
**Commands Learned**: 15+ KQL queries  
**Documentation**: day61-sentinel-kql-basics.md
