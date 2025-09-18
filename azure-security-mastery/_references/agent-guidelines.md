# 🤖 Agent Guidelines & Guardrails

## ⛔ **NEVER DO WITHOUT EXPLICIT PERMISSION**

### **Billing/Trial Related**
- ❌ **NEVER start any Azure trials** (Defender, Sentinel, etc.)
- ❌ **NEVER enable paid tiers** (Standard storage, Premium anything)
- ❌ **NEVER deploy expensive resources** (VMs, databases, etc.)
- ✅ **ALWAYS ask first**: "This will start a trial/cost money. Proceed?"

### **Resource Creation**
- ❌ **NEVER deploy without user confirmation** 
- ❌ **NEVER assume "go ahead and deploy"**
- ✅ **ALWAYS show what will be created first**
- ✅ **ALWAYS get explicit "yes, deploy this" confirmation**

## 🔍 **ALWAYS CHECK FIRST**

### **Before Taking Action**
1. **Read current context**: What files exist? What's been done?
2. **Check session state**: What was the last action? Where are we?
3. **Verify scope**: Is this a learning exercise or production deployment?
4. **Ask for clarification**: "Should I create/deploy/enable X?"

### **Memory/Context Issues**
1. **Use tools to gather context** before assuming anything
2. **Read relevant files** to understand current state
3. **Check terminal history** to see what was done recently
4. **Ask user to confirm** current situation if unclear

## 📋 **Prompt Templates for User**

### **Start Each Session With**
```
"Before we continue, let me check current state and confirm what you want to do next. 

Current focus: [what you want to work on]
Billing concerns: [yes/no - if yes, what limits]
Resource deployment: [read-only/create new/modify existing]

Please confirm this is correct and I'll proceed accordingly."
```

### **Before Any Deployment**
```
"I'm about to create/deploy:
- [specific resources]
- [cost implications if any]
- [trial implications if any]

Type 'DEPLOY' to confirm or 'STOP' to reconsider."
```

### **When Something Looks Expensive**
```
"⚠️ WARNING: This involves:
- [specific cost/trial concern]
- [monthly implications]
- [what happens after trial]

Please confirm you understand the costs and want to proceed."
```

## 🎯 **Safe Learning Patterns**

### **Default Mode: Read-Only**
- Explore existing resources
- Analyze configurations
- Create documentation
- Plan deployments (but don't execute)

### **Creation Mode: Explicit Permission**
- Get specific "create X resource" instruction
- Show template/plan first
- Confirm costs/implications
- Execute only after explicit approval

### **Trial Mode: Maximum Caution**
- Assume user wants to AVOID trials unless explicitly stated
- Always mention trial implications
- Provide free alternatives first
- Get written confirmation for any trial activation

## 📖 **Learning vs Production Context**

### **Learning Context Indicators**
- Working in `/cybersecurity-journey/` directory
- References to "Day X" structure
- Educational objectives mentioned
- Budget consciousness expressed

### **Learning Mode Defaults**
- Prefer free tier resources
- Use existing resources when possible
- Focus on configuration over deployment
- Simulate expensive features instead of enabling them

## 🚨 **Recovery Patterns**

### **If I Make a Mistake**
1. **Stop immediately** when user expresses concern
2. **Assess the damage** (what was created/enabled)
3. **Provide immediate remediation** (revert scripts, cost analysis)
4. **Document lessons learned** in this guideline file

### **Cost Mitigation**
- Always provide revert scripts for anything with ongoing costs
- Monitor and report current spending
- Set up alerts and safeguards
- Prefer temporary resources with auto-cleanup

## 🎯 **Session Start Checklist**

Before each session, agent should:
1. ✅ Read ACTUAL-PROGRESS.md to understand current state
2. ✅ Check last terminal commands to see recent activity
3. ✅ Ask user to confirm current objectives
4. ✅ Verify any cost/trial constraints
5. ✅ Confirm deployment permissions (read-only vs create)

## 📝 **User Prompt Suggestions**

### **Ideal Session Starter**
```
"Resume Azure learning from Day X. 
Mode: [read-only/create-safe/create-with-costs]
Constraints: [no trials/free tier only/budget limit]
Focus: [specific topic/hands-on lab/planning]

Refer to agent-guidelines.md for guardrails."
```

### **When User Wants Deployment**
```
"Deploy [specific resource] for Day X learning.
Costs: [acceptable/concern/must be free]
Trials: [ok to start/avoid/revert after]

Check guidelines before proceeding."
```

## 🔄 **Iterative Approach**

### **Small Steps Pattern**
1. **Show plan first** → Get approval → Execute
2. **Verify results** → Document → Ask for next step  
3. **Check for issues** → Remediate if needed → Continue

### **Avoid "Fire and Forget"**
- Don't chain multiple deployments
- Don't assume next steps
- Don't continue without user feedback
- Always pause for confirmation

---

**Remember**: Better to ask too many questions than create unexpected costs or complexity!

*Last updated: September 18, 2025*