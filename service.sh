MODPATH="${0%/*}"

until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 1
done
sleep 3


# Optimasi izin akses path dasar agar tidak mudah ditelusuri oleh direktori traversal biasa
if [ -d "/data/adb" ]; then
    chmod 700 /data/adb 2>/dev/null
fi
