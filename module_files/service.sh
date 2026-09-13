#!/system/bin/sh
MODDIR="${0%/*}"

until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 2
done

rm -f "$MODDIR/boot_fail" 2>/dev/null

if [ -f "$MODDIR/error_report.txt" ]; then
    ERR_MSG=$(cat "$MODDIR/error_report.txt")
    am start -n "com.stnk.rootassistant/.ErrorDialogActivity" \
        -a "com.stnk.rootassistant.SHOW_ERROR" \
        --es "error_msg" "$ERR_MSG" 2>/dev/null
fi

SUSFS_BIN=""
if [ -x "/data/adb/ksu/bin/susfs" ]; then
    SUSFS_BIN="/data/adb/ksu/bin/susfs"
elif [ -x "/data/adb/ap/bin/susfs" ]; then
    SUSFS_BIN="/data/adb/ap/bin/susfs"
elif command -v susfs >/dev/null 2>&1; then
    SUSFS_BIN="susfs"
fi

if [ -n "$SUSFS_BIN" ]; then
    "$SUSFS_BIN" add_sus_path "$MODDIR"
    "$SUSFS_BIN" add_sus_path "/data/adb/modules"
    "$SUSFS_BIN" add_sus_path "/data/adb/ksu"
    "$SUSFS_BIN" add_sus_path "/data/adb/ap"
    "$SUSFS_BIN" add_sus_path "/data/adb/modules_update"
    "$SUSFS_BIN" add_sus_path "/data/local/tmp"
    "$SUSFS_BIN" add_sus_path "/sdcard/Download"

    "$SUSFS_BIN" add_try_umount "$MODDIR"
    "$SUSFS_BIN" add_try_umount "/data/adb/modules"
    "$SUSFS_BIN" add_try_umount "/sbin"

    "$SUSFS_BIN" add_sus_kstat "$MODDIR"
    "$SUSFS_BIN" add_sus_kstat "/system/etc/hosts"
    "$SUSFS_BIN" add_sus_kstat "/data/local/tmp"
    "$SUSFS_BIN" add_sus_kstat "/sdcard/Download"

    "$SUSFS_BIN" enable_sus_su 1
    "$SUSFS_BIN" enable_log 0
fi

umount -l /system/xbin/su 2>/dev/null
umount -l /system/bin/su 2>/dev/null
umount -l /sbin/su 2>/dev/null
umount -l /data/local/su 2>/dev/null

exit 0
