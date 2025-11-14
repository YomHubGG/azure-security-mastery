# 🐛 Debugging GitHub Actions + Docker Build Context Issues

## Executive Summary
**Problem:** Docker build in GitHub Actions CI/CD pipeline consistently failed with `package.json: not found` despite file existing locally and working in local Podman builds.

**Root Cause:** Repository root `.gitignore` contained `*.json` pattern that blocked `package.json` from being transferred to Docker build context in GitHub Actions environment.

**Solution:** Remove `*.json` from `.gitignore`, add specific exclusions instead (`package-lock.json` should be committed, `*.bicep.json` can be ignored).

**Learning:** `.gitignore` affects Docker build context transfer in GitHub Actions, not just Git operations!

---

## 📋 Timeline of Investigation

### Initial Symptom (Attempt #1)
**Error:**
```
npm ci: exit code 243
Permission denied when running npm install as non-root user
```

**Suspicion:** Dockerfile permissions issue - running `npm install` as `node` user before files are owned by that user.

**Action Taken:** Changed Dockerfile to run `npm ci` as root, then `chown` to node user.

**Result:** ❌ Failed - Different error appeared.

---

### Second Error (Attempts #2-3)
**Error:**
```
npm error code EUSAGE
The `npm ci` command can only install with an existing package-lock.json
```

**Suspicion:** Missing `package-lock.json` file (never committed to Git).

**Action Taken:** 
1. Generated `package-lock.json` with `npm install` locally
2. Found it was being ignored by `.dockerignore` (line: `package-lock.json`)
3. Removed `package-lock.json` from `.dockerignore`
4. Force-added with `git add -f`

**Result:** ❌ Failed - Yet another error appeared.

---

### Third Error (Attempts #4-7)
**Error:**
```
npm error code ENOENT
npm error path /build/package.json
npm error errno -2
npm error enoent Could not read package.json: Error: ENOENT: no such file or directory
```

**Suspicion:** `.dockerignore` blocking files from build context.

**Actions Taken (Multiple Attempts):**
1. **Attempt #4:** Minimized `.dockerignore` to only `node_modules/`
2. **Attempt #5:** Deleted `.dockerignore` entirely
3. **Attempt #6:** Added `./` prefix to context paths in workflow
4. **Attempt #7:** Disabled Docker buildx cache (`no-cache: true`)

**Result:** ❌ All failed with same error - `package.json` still not found!

---

### Critical Debugging Step (Attempt #8)
**Action:** Added `RUN ls -la /build/` in Dockerfile to see what files Docker actually received.

**Output:**
```dockerfile
#10 0.051 total 40
#10 0.051 drwxr-xr-x    1 root     root          4096 Nov 14 10:24 .
#10 0.051 drwxr-xr-x    1 root     root          4096 Nov 14 10:24 ..
#10 0.051 -rw-r--r--    1 root     root         28833 Nov 14 10:23 package-lock.json
```

**💡 REVELATION:** Only `package-lock.json` present! `package.json` was completely missing from build context!

**Suspicion Updated:** Something in Git configuration is preventing `package.json` from being transferred to Docker build context, despite file existing in repository.

---

### Fourth Error (Attempt #9)
**Action:** Changed Dockerfile from wildcard `COPY package*.json ./` to explicit `COPY package.json package-lock.json ./`

**Error (NEW!):**
```
ERROR: failed to compute cache key: 
failed to calculate checksum of ref p15xo1ac1397fq1qgjvzgk47c::dl6a1jgpfb1mexcujxkb65uic: 
"/package.json": not found
```

**Analysis:** Docker buildx couldn't even compute cache key because `package.json` didn't exist in the build context at all!

**Suspicion:** File is being filtered BEFORE Docker build process starts - likely by Git mechanisms.

---

### Root Cause Discovery (Attempt #10)
**Action:** Checked repository root `.gitignore`

**Finding:**
```gitignore
# Build outputs
dist/
build/

# Bicep build output
*.json    # ← THE CULPRIT!
```

**💥 ROOT CAUSE IDENTIFIED:** 

The `.gitignore` pattern `*.json` was originally added to ignore Bicep-generated JSON files (from `*.bicep` transpilation), but it had the unintended consequence of excluding `package.json` from Docker build context!

---

## 🎓 Key Learnings

### 1. `.gitignore` Affects Docker Build Context in GitHub Actions

**What Happened:**
- GitHub Actions uses Git to checkout repository
- When Docker build-push-action creates build context, it respects `.gitignore` patterns
- Files matching `.gitignore` patterns are **NOT** included in the context sent to Docker daemon
- This differs from local Docker/Podman builds where the file system is used directly

**Why It's Confusing:**
- Local `docker build` or `podman build` uses filesystem directly - `.gitignore` has NO effect
- GitHub Actions `docker/build-push-action` uses Git-aware context transfer
- No clear error message indicating files are filtered by `.gitignore`

### 2. `.dockerignore` vs `.gitignore` Interaction

**Execution Order:**
```
1. Git checkout (respects .gitignore) → Files cloned to workspace
2. Docker context preparation (respects .dockerignore) → Files sent to Docker daemon
3. Docker build (uses received context) → COPY commands execute
```

**Critical Rule:**
- If `.gitignore` blocks a file, it NEVER reaches Docker, even if `.dockerignore` allows it!
- Both filters are cumulative, not mutually exclusive

### 3. Docker Buildx Cache Behavior

**Observation:**
- Docker buildx computes cache keys by checksumming source files
- If file doesn't exist in context, buildx fails at cache key computation stage
- Error message is cryptic: `failed to calculate checksum of ref <hash>: "/file": not found`

### 4. Wildcard COPY Behavior

**Discovery:**
- `COPY package*.json ./` silently succeeds if it matches 1+ files (even if some expected files are missing)
- In our case, it matched `package-lock.json` but not `package.json` (which was filtered by `.gitignore`)
- Explicit `COPY package.json package-lock.json ./` failed faster with clearer error

**Best Practice:**
- Use explicit filenames in COPY commands for critical files
- Wildcards can hide missing file issues

---

## 🔧 Solution Implementation

### Fix #1: Update Root `.gitignore`
```diff
 # Build outputs
 dist/
 build/
 
-# Bicep build output
-*.json
+# Bicep build output (be specific!)
+*.bicep.json
+*parameters.json
+
+# Keep package.json files for Node.js projects
+!package.json
+!package-lock.json
```

**Rationale:**
- Be specific with ignore patterns - avoid broad wildcards like `*.json`
- Use negation patterns (`!`) to explicitly include important files
- Document WHY patterns exist to prevent future issues

### Fix #2: Explicit COPY in Dockerfile
```dockerfile
# ❌ BAD: Silent failure if package.json missing
COPY package*.json ./

# ✅ GOOD: Explicit file list - fails fast with clear error
COPY package.json package-lock.json ./
```

---

## 🎯 How to Debug Similar Issues

### Step 1: Verify Local Build Works
```bash
cd path/to/app
podman build -t test-local .
```
**Purpose:** Confirms Dockerfile syntax and logic are correct.

### Step 2: Add Debug Statements
```dockerfile
RUN ls -la /workdir/
RUN find /workdir -name "*.json"
```
**Purpose:** See what files Docker actually received.

### Step 3: Check `.gitignore` Patterns
```bash
# Test if file is ignored by Git
git check-ignore -v path/to/file

# Example output if blocked:
# .gitignore:42:*.json    path/to/package.json
```

### Step 4: Examine GitHub Actions Checkout
```yaml
- name: Debug Workspace
  run: |
    echo "=== Workspace Contents ==="
    find ${{ github.workspace }} -name "package*.json"
    echo "=== Git Status ==="
    git status
    echo "=== Git Check-Ignore ==="
    git check-ignore -v */package.json || true
```

### Step 5: Test Build Context Transfer
```yaml
- name: Build with Verbose Output
  uses: docker/build-push-action@v5
  with:
    context: ./app
    file: ./app/Dockerfile
    push: false
    load: true
    # Add verbose logging
    build-args: |
      BUILDKIT_PROGRESS=plain
```

---

## 🚨 Common Pitfalls

### Pitfall #1: Overly Broad `.gitignore` Patterns
```gitignore
# ❌ TOO BROAD - blocks critical files
*.json
*.log
*.config

# ✅ SPECIFIC - clear intent
build/*.json
logs/*.log
.vscode/*.config
```

### Pitfall #2: Assuming Local = CI/CD
- **Local Docker:** Uses filesystem directly, ignores `.gitignore`
- **GitHub Actions Docker:** Uses Git checkout, respects `.gitignore`
- **Lesson:** Always test in CI/CD environment, not just locally

### Pitfall #3: Ignoring Silent COPY Wildcards
```dockerfile
# ❌ RISKY: Might copy 0, 1, or many files silently
COPY *.config ./

# ✅ SAFE: Explicit list, fails if missing
COPY app.config database.config ./
```

### Pitfall #4: Not Using `git check-ignore`
```bash
# Before assuming file transfer issues, always check:
git check-ignore -v path/to/missing/file

# Output reveals which .gitignore rule is blocking it:
# .gitignore:15:*.json    path/to/package.json
```

---

## 📚 References & Further Reading

### Official Documentation
1. **GitHub Actions - Docker build-push-action**
   - https://github.com/docker/build-push-action
   - Note: Doesn't explicitly document `.gitignore` interaction!

2. **Docker - .dockerignore**
   - https://docs.docker.com/engine/reference/builder/#dockerignore-file
   - Only covers `.dockerignore`, not Git integration

3. **Git - gitignore Patterns**
   - https://git-scm.com/docs/gitignore
   - Pattern matching rules and negation

### Key Insights Not in Docs
1. **Build context in GitHub Actions is Git-aware** (undocumented behavior)
2. **`.gitignore` takes precedence over everything** in CI/CD Docker builds
3. **Buildx cache key computation fails early** if files missing from context

---

## ✅ Verification Checklist

After implementing fixes, verify:

- [ ] `git check-ignore path/to/package.json` returns nothing (file not ignored)
- [ ] Local build works: `podman build -t test .`
- [ ] GitHub Actions build succeeds (check workflow logs)
- [ ] `ls -la` debug output in Dockerfile shows all expected files
- [ ] `package.json` and `package-lock.json` both present in Git history
- [ ] `.gitignore` patterns are specific, not overly broad
- [ ] Critical files use explicit COPY, not wildcards

---

## 🎬 Conclusion

This debugging journey revealed a critical but poorly documented interaction between:
1. Git ignore patterns (`.gitignore`)
2. Docker build context transfer (GitHub Actions)
3. Docker buildx caching mechanisms

**Key Takeaway:** In GitHub Actions CI/CD pipelines, `.gitignore` patterns affect Docker build context. Always use specific ignore patterns and verify with `git check-ignore` before assuming file transfer issues.

**Time Invested:** ~10 attempts, ~45 minutes of debugging
**Root Cause:** Single line in `.gitignore`: `*.json`
**Lesson:** Overly broad ignore patterns have far-reaching consequences!

---

**Author:** YomHubGG  
**Date:** November 14, 2025  
**Context:** Day 59 Q2 Capstone - SecureCloud DevSecOps Platform  
**Status:** ✅ Resolved
