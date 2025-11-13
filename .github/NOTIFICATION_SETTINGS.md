# GitHub Actions Notification Settings

## 🔕 How to Stop Email Spam from GitHub Actions

### Method 1: GitHub Web UI (Easiest)
1. Visit: https://github.com/settings/notifications
2. Under **Actions**, select:
   - ✅ "Only notify me for failed workflows I'm watching"
   - ❌ Uncheck "Send notifications for all workflows"
3. Click **Save preferences**

### Method 2: Repository-Specific Settings
1. Go to your repository: https://github.com/YomHubGG/cybersecurity-journey
2. Click **Watch** → **Custom** → Uncheck **Actions**
3. This stops emails for THIS repository only

### Method 3: Email Filters
Create a Gmail/Outlook filter:
- **From**: `notifications@github.com`
- **Subject**: `[YomHubGG/cybersecurity-journey]`
- **Action**: Skip inbox / Archive / Delete

### Method 4: GitHub Mobile App
Disable push notifications in the GitHub mobile app settings.

---

## 🛠️ Workflow Modifications Applied

The workflows have been updated with:
- `continue-on-error: true` on non-critical jobs
- Less frequent triggers (manual + PR only for some workflows)
- Consolidated notifications

## Current Workflow Behavior

| Workflow | Trigger | Emails? |
|----------|---------|---------|
| `day45-secure-pipeline.yml` | Push to main, PR | On failure only |
| `iac-security-scanning.yml` | Push to main (*.bicep), PR | On failure only |
| `secure-devsecops-pipeline.yml` | Push to main, PR | On failure only |

---

**Note**: If you want workflows to run but NOT send emails, use Method 1 or 2 above.
