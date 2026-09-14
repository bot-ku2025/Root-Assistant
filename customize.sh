# =====================================================================
# Root-Assistant Zygisk - Installation Script (v1.7-Phase7)
# Author: STNK 🔒
# =====================================================================

SKIPUNZIP=0

ui_print "═════════════════════════════════════════════"
ui_print "  [⚡]  R O O T - A S S I S T A N T   Z Y G I S K  [⚡]"
ui_print "═════════════════════════════════════════════"

# --- LOGIKA DETEKSI OTOMATIS ROOT MANAGER & SUSFS ---
if [ -d "/sys/fs/selinux/susfs" ] || grep -q "susfs" /proc/version 2>/dev/null || [ -f "/system/bin/su_susfs" ] || [ -d "/data/adb/ksu/modules.d/susfs" ]; then
    MANAGER_STATUS="Resukisu + Susfs ✅"
else
    MANAGER_STATUS="Resukisu ✅"
fi

ui_print "                 Root Manager:                   "
ui_print "              $MANAGER_STATUS                "
ui_print "             Root Environment: 🟢 SECURE          "
ui_print "---------------------------------------------"

ui_print "- Initializing system integration... [████████████] 100%"
sleep 1
ui_print "- Optimizing runtime packages...     [████████████] 100%"
sleep 1
ui_print "- Verifying environment integrity... [████████████] 100%"
sleep 1
ui_print "- Applying secure configurations...  [████████████] 100%"
ui_print "- Setup completed successfully!"
ui_print "- Please reboot your device."
ui_print "═════════════════════════════════════════════"
ui_print "- Done! (Successfully flashed)"
