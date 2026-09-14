#!/system/bin/sh
MODDIR=${0%/*}

if command -v ksu_susfs >/dev/null 2>&1; then
    ksu_susfs add_sus_path /sys/devices/platform/kcal_ctrl
    ksu_susfs add_sus_path /sys/module/kcal_ctrl
    ksu_susfs add_sus_path /sys/module/simple_lmk
    
    for f in /system/etc/permissions/*lineage*.xml; do
        [ -f "$f" ] && ksu_susfs add_sus_path "$f"
    done
    for f in /vendor/etc/permissions/*lineage*.xml; do
        [ -f "$f" ] && ksu_susfs add_sus_path "$f"
    done
fi
