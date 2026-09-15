MODPATH="${0%/*}"

until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 1
done
sleep 3

# Gunakan tmpfs untuk menimpa folder Sentinel dengan folder kosong bersih di level kernel
TARGET_TRAP="/data/adb/ModuleSentinel"
if [ -d "$TARGET_TRAP" ]; then
    mount -t tmpfs -o size=4k tmpfs "$TARGET_TRAP"
fi
