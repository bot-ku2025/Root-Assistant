#!/system/bin/sh
MODDIR=${0%/*}

# Tunggu sistem selesai boot
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 2
done

# Sembunyikan folder data MT Manager dari pemindaian OVO menggunakan tmpfs mount
if [ -d "/data/data/bin.mt.plus" ]; then
    mount -t tmpfs tmpfs /data/data/bin.mt.plus 2>/dev/null
fi

# Matikan aksesibilitas khusus untuk paket BCA Mobile di level AppOps OS
appops set com.bca BIND_ACCESSIBILITY_SERVICE deny 2>/dev/null
appops set com.bca SYSTEM_ALERT_WINDOW deny 2>/dev/null
