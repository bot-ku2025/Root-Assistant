MODPATH="${0%/*}"

# Tunggu sampai sistem benar-benar nyala
until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 1
done
sleep 5 # Jeda ekstra agar KernelSU selesai mounting

# Eksekusi jebakan Sentinel setelah aman
TARGET_TRAP="/data/adb/ModuleSentinel"
if [ -d "$TARGET_TRAP" ]; then
    mount -o bind /dev/null "$TARGET_TRAP"
fi
