#!/system/bin/sh
MODDIR=${0%/*}
SUSFS_DIR="/sys/fs/susfs"

if [ -d "$SUSFS_DIR" ]; then
    [ -d "/sys/devices/platform/kcal_ctrl" ] && echo "/sys/devices/platform/kcal_ctrl" > ${SUSFS_DIR}/sus_path
    [ -d "/sys/module/kcal_ctrl" ] && echo "/sys/module/kcal_ctrl" > ${SUSFS_DIR}/sus_path
    [ -d "/sys/module/simple_lmk" ] && echo "/sys/module/simple_lmk" > ${SUSFS_DIR}/sus_path

    for f in /system/etc/permissions/*lineage*.xml; do
        [ -f "$f" ] && echo "$f" > ${SUSFS_DIR}/sus_path
    done
    for f in /vendor/etc/permissions/*lineage*.xml; do
        [ -f "$f" ] && echo "$f" > ${SUSFS_DIR}/sus_path
    done
fi
