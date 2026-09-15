MODPATH="${0%/*}"

until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 1
done
sleep 3


