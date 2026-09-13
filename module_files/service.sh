#!/system/bin/sh
MODDIR=${0%/*}
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done

# Hardening system properties secara universal untuk semua banking & e-wallet
resetprop -n ro.boot.flash.locked 1
resetprop -n ro.boot.verifiedbootstate green
resetprop -n ro.secure 1
resetprop -n ro.debuggable 0
resetprop -n ro.build.type user
resetprop -n ro.build.tags release-keys
resetprop -n ro.adb.secure 1
resetprop -n service.adb.root 0

# Matikan total global accessibility flags dan bersihkan daftar layanan
settings put secure accessibility_enabled 0
settings put secure enabled_accessibility_services ""

# Lock down developer options & mock location
settings put global development_settings_enabled 0
settings put global adb_enabled 0
settings put global mock_location 0
