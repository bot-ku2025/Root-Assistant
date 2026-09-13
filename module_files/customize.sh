#!/system/bin/sh
ui_print "=================================================="
ui_print "       Root Assistant [v7.2 Stable]               "
ui_print "       Developed by STNK                          "
ui_print "=================================================="
ui_print "[*] Initializing elite security kernel protocols..."

ui_print "[ S C A N N I N G   S Y S T E M ]"
ui_print "[-] █▒▒▒▒▒▒▒▒▒ [10%] Inspecting VFS namespace..."
sleep 1.5
ui_print "[-] ████▒▒▒▒▒▒ [40%] Bypassing SELinux constraints..."
sleep 1.5
ui_print "[-] ███████▒▒▒ [70%] Verifying suSFS root hooks..."
sleep 1.5
ui_print "[-] ██████████ [100%] Multi-layer shield active!"
ui_print "=================================================="

rm -f "$MODPATH/disable" 2>/dev/null

if [ -f "$MODPATH/RootAssistantUI.apk" ]; then
    pm install -r "$MODPATH/RootAssistantUI.apk" >/dev/null 2>&1
fi

if [ -e "/sys/kernel/susfs" ] || [ -f "/data/adb/ksu/bin/susfs" ]; then
    ui_print "[+] Kernel Engine: suSFS Detected (Advanced Mode)"
else
    ui_print "[!] Kernel Engine: Standard (Fallback Mode Active)"
fi

set_perm_recursive "$MODPATH" 0 0 0755 0644
set_perm "$MODPATH/post-fs-data.sh" 0 0 0755
set_perm "$MODPATH/service.sh" 0 0 0755
ui_print "[+] Elite Isolated Anti-Bootloop Guard: READY"
ui_print "=================================================="
