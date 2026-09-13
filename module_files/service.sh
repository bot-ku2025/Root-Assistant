#!/system/bin/sh
MODDIR=${0%/*}
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done
resetprop -n ro.boot.flash.locked 1
resetprop -n ro.boot.verifiedbootstate green
resetprop -n ro.secure 1
resetprop -n ro.debuggable 0
resetprop -n ro.build.type user
resetprop -n ro.build.tags release-keys
settings put secure enabled_accessibility_services "" 2>/dev/null
