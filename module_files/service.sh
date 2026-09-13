#!/system/bin/sh
MODDIR=${0%/*}
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done

# 1. System Property Hardening Universal (Anti-Root, Anti-Debug, Anti-Emulator)
resetprop -n ro.boot.flash.locked 1
resetprop -n ro.boot.verifiedbootstate green
resetprop -n ro.secure 1
resetprop -n ro.debuggable 0
resetprop -n ro.build.type user
resetprop -n ro.build.tags release-keys
resetprop -n ro.adb.secure 1
resetprop -n service.adb.root 0
resetprop -n sys.usb.state mass_storage
resetprop -n sys.usb.config none
resetprop -n ro.kernel.qemu 0

# 2. Neutralisasi Total Developer Options & USB Debugging
settings put global development_settings_enabled 0
settings put global adb_enabled 0
settings put global usb_debugging_enabled 0
settings put global mock_location 0

# 3. Pembersihan Universal Aksesibilitas untuk Seluruh Perbankan & E-Wallet
settings put secure accessibility_enabled 0
settings put secure enabled_accessibility_services ""
