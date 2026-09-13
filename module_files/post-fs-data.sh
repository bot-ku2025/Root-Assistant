#!/system/bin/sh
MODDIR="${0%/*}"

if [ -f "$MODDIR/boot_fail" ]; then
    touch "$MODDIR/disable"
    echo "post-fs-data.sh crashed during early boot execution at line 18." > "$MODDIR/error_report.txt"
    rm -f "$MODDIR/boot_fail"
    exit 0
fi

touch "$MODDIR/boot_fail"

[ -f "/data/local/tmp/root_x_safemode" ] && exit 0

resetprop -n "ro.boot.verifiedbootstate" "green"
resetprop -n "ro.boot.flash.locked" "1"
resetprop -n "ro.boot.vbmeta.device_state" "locked"
resetprop -n "ro.secure" "1"
resetprop -n "ro.debuggable" "0"
resetprop -n "ro.build.type" "user"
resetprop -n "ro.build.tags" "release-keys"
resetprop -n "ro.boot.selinux" "enforcing"
resetprop -n "ro.adb.secure" "1"

chmod 0700 /data/local/tmp 2>/dev/null
exit 0
