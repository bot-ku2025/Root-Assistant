#!/system/bin/sh
MODDIR=${0%/*}

while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done

# Paksa kosongkan database layanan aksesibilitas yang memicu deteksi BCA
settings put secure enabled_accessibility_services "" 2>/dev/null
settings put secure accessibility_enabled 0 2>/dev/null

# Kunci properti sistem
resetprop -n ro.boot.flash.locked 1
resetprop -n ro.boot.verifiedbootstate green
resetprop -n ro.secure 1
resetprop -n ro.debuggable 0
