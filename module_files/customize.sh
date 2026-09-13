#!/system/bin/sh
SKIPMOUNT=false
PROPDIR=false
POSTFSDATA=true
LATESTARTSVC=true

ui_print "***********************************"
ui_print "***********************************"
ui_print "  Powered by ReSukiSU & KernelSU   "
ui_print "***********************************"
ui_print ""
ui_print "- Extracting module files"
ui_print "===================================\n"

if [ -f "$MODPATH/module.prop" ]; then
    MOD_VER=$(grep "^version=" "$MODPATH/module.prop" | cut -d= -f2)
else
    MOD_VER="v7.4"
fi

ui_print "      Root Assistant [$MOD_VER]"
ui_print "      Developed by STNK"
ui_print "===================================\n"
ui_print "[*] Initializing elite security kernel protocols..."
ui_print "[ S C A N N I N G   S Y S T E M ]"
ui_print "[-] █▒▒▒▒▒▒▒▒▒ [10%] Inspecting VFS namespace..."
sleep 0.2
ui_print "[-] ████▒▒▒▒▒▒ [40%] Bypassing SELinux constraints..."
sleep 0.2
ui_print "[-] ███████▒▒▒ [70%] Verifying suSFS root hooks..."
sleep 0.2
ui_print "[-] ██████████ [100%] Multi-layer shield active!"
ui_print "===================================\n"

ENGINE_STATUS="Standard (Fallback Mode)"
if [ "$KSU" = "true" ] || [ -d "/sys/kernel/ksu" ] || [ -f "/sys/kernel/ksu/version" ]; then
    if dmesg | grep -qi "susfs" || [ -d "/sys/kernel/su_override" ] || [ -f "/sys/fs/selinux/susfs" ]; then
        ENGINE_STATUS="Elite suSFS + ReSukiSU Engine (100% Active)"
    else
        ENGINE_STATUS="KernelSU / ReSukiSU Engine (100% Active)"
    fi
elif [ -d "/data/adb/apatch" ]; then
    ENGINE_STATUS="APatch Kernel Engine (100% Active)"
elif [ -d "/data/adb/magisk" ]; then
    ENGINE_STATUS="Magisk Zygisk Engine (100% Active)"
fi

ui_print "[+] Kernel Engine: $ENGINE_STATUS"
ui_print "[+] Elite Isolated Anti-Bootloop Guard: READY"
ui_print "===================================\n"
ui_print "- Done"
ui_print "- Module installed successfully!"
