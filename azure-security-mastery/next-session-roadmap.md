# 🗺️ Next Learning Session Roadmap

**Based on**: iac-deep-dive.md learning path  
**Status**: Day 1 foundation complete ✅  
**Next**: Advance IaC skills and network security

## 🎯 **Next Session Goals**

### **IaC Skills Progression** (from iac-deep-dive.md):

#### **Foundation** ✅ (Completed):
- [x] Understand IaC concepts 
- [x] Create first Bicep template
- [x] Deploy real infrastructure
- [x] Version control templates

#### **Next Steps** (Priority for next session):
- [ ] **Multi-resource templates** - Deploy VNet + Storage + VM together
- [ ] **Parameter files** - Separate configs for Dev/Test/Prod environments
- [ ] **Template validation** - `bicep build` and testing practices
- [ ] **Dependency management** - How resources depend on each other

## 🚀 **Suggested Session Plan**

### **Phase 1: Multi-Resource Template (45 min)**
Create a template that deploys:
```bicep
// Goal: Single template, multiple resources
├── Virtual Network
├── Subnet  
├── Storage Account
├── Network Security Group
└── Proper dependencies between them
```

### **Phase 2: Environment Parameters (30 min)**
Learn parameter files:
```bash
# Different configs per environment
bicep deploy --parameters dev.parameters.json
bicep deploy --parameters prod.parameters.json
```

### **Phase 3: Validation & Testing (30 min)**
```bash
# Template validation
bicep build template.bicep
az deployment group validate

# What-if analysis  
az deployment group what-if --template-file template.bicep
```

### **Phase 4: Network Security Focus (45 min)**
Since next is "Network Security Architecture":
- Network Security Groups (NSGs)
- Virtual Network peering
- Azure Bastion for secure access
- Security best practices in networking

## 📂 **Files to Create Next Session**

```
01-foundations/
├── azure-account-setup/ (current)
└── network-security/  (next)
    ├── multi-resource.bicep
    ├── dev.parameters.json  
    ├── prod.parameters.json
    ├── network-security-template.bicep
    └── day3-checklist.md
```

## 🎓 **Learning Objectives for Next Time**

By end of next session, I should be able to:
- [ ] Deploy complex infrastructure with multiple resources
- [ ] Use parameter files for environment-specific configs
- [ ] Validate templates before deployment
- [ ] Design secure network architectures
- [ ] Implement NSG rules as code
- [ ] Understand VNet design patterns

## 🔗 **Reference for Next Session**

**Key files to review before starting**:
1. `iac-deep-dive.md` - My learning path
2. `storage-account.bicep` - Working template to build upon
3. `day1-checklist.md` - See what's been accomplished

**Commands to remember**:
```bash
# Template validation
bicep build template.bicep

# What-if deployment  
az deployment group what-if --template-file template.bicep

# Deploy with parameters
az deployment group create --parameters @parameters.json
```

## 💡 **Key Mindset for Next Session**

Continue thinking **"code first, not clicks first"** for everything:
- Network design → Bicep template
- Security rules → NSG in code  
- Multiple environments → Parameter files
- Changes → Git commits, not manual edits

## 🎯 **Success Metrics for Next Session**

- [ ] Deploy infrastructure with 3+ resource types in single template
- [ ] Successfully use parameter files for different environments  
- [ ] Validate templates without deploying
- [ ] Create secure network architecture following best practices
- [ ] More green commits on GitHub 🟢

---

**💪 Ready to level up the IaC skills and dive into network security!**

*Remember: I'm building towards €70K-120K Azure Security Engineer skills, one session at a time.*
