#!/bin/bash
# 🔕 GitHub Actions Email Spam Killer
# Run this to open GitHub notification settings in your browser

echo "🤖 Tying down your robot boss..."
echo ""
echo "Opening GitHub notification settings..."
echo ""

# Open GitHub notification settings
if command -v xdg-open &> /dev/null; then
    xdg-open "https://github.com/settings/notifications" 2>/dev/null
elif command -v open &> /dev/null; then
    open "https://github.com/settings/notifications"
else
    echo "Please visit: https://github.com/settings/notifications"
fi

echo "📋 Instructions:"
echo ""
echo "1. Scroll to 'Actions' section"
echo "2. UNCHECK: '✉️  Send notifications for failed workflows'"
echo "3. OR select: 'Only notify for workflows I'm watching'"
echo "4. Click 'Save preferences'"
echo ""
echo "🎉 Your robot boss will be silenced!"
echo ""
echo "Alternative: Unwatch this repo's Actions:"
echo "👉 https://github.com/YomHubGG/cybersecurity-journey"
echo "   Click: Watch → Custom → Uncheck 'Actions'"
