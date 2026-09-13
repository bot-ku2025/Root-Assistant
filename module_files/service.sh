#!/system/bin/sh
MODDIR=${0%/*}

while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done

# Reset status Aksesibilitas, ADB, dan Developer Options
settings put secure enabled_accessibility_services "" 2>/dev/null
settings put global adb_enabled 0 2>/dev/null
settings put global development_settings_enabled 0 2>/dev/null
