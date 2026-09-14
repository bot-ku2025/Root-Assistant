#!/system/bin/sh
MODDIR=${0%/*}
LOG_FILE="/data/adb/root_assistant.log"

echo "[$(date)] Root-Assistant v1.7-Phase7 initializing..." > $LOG_FILE

if [ -d "/dev/susfs" ] || [ -f "/sys/kernel/susfs/version" ]; then
    echo "[$(date)] SuSFS detected: Active" >> $LOG_FILE
else
    echo "[$(date)] SuSFS detected: Standard Mode" >> $LOG_FILE
fi
