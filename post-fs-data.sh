#!/system/bin/sh
MODDIR=${0%/*}
ENV_FILE="/dev/root_assistant_env"
FLAG_FILE="$MODDIR/crash_count"
WARNING_FILE="$MODDIR/CRASH_WARNING"

# Function: Disable modul mandiri & kirim peringatan
disable_self_with_warning() {
    REASON="$1"
    touch "$MODDIR/disable"
    echo "$REASON" > "$WARNING_FILE"
    echo "ENGINE=DISABLED" > "$ENV_FILE"
    echo "REASON=$REASON" >> "$ENV_FILE"
    exit 0
}

# LOGIKA TOLERANSI & BOOTLOOP PROTECTION
if [ -f "$FLAG_FILE" ]; then
    COUNT=$(cat "$FLAG_FILE")
    COUNT=$((COUNT + 1))
else
    COUNT=1
fi
echo "$COUNT" > "$FLAG_FILE"

# Jika file 'disable' dihapus manual tetapi modul masih bermasalah di Boot ke-2
# -> Lepas proteksi toleransi dan trigger BOOTLOOP TOTAL
if [ "$COUNT" -ge 2 ] && [ ! -f "$MODDIR/disable" ]; then
    # User memaksa aktifkan modul yang rusak -> Forced Crash / Bootloop
    rm -f "$FLAG_FILE" "$WARNING_FILE"
    # Memicu kernel panic / exit 1 untuk simulasi crash total sesuai permintaan
    exit 1
fi

# Jika Boot 1 mendeteksi kesalahan -> Aktifkan Toleransi & Self-Disable
if [ "$COUNT" -eq 1 ]; then
    # STRICT LOCK: ReSukiSU / KernelSU Only
    if [ ! -c /dev/ksu ] && [ ! -d /sys/module/kernelsu ]; then
        disable_self_with_warning "NOT_RESUKISU_OR_KSU"
    fi

    # Cek Integritas File Modul (Contoh Pengecekan Crash/Corrupt)
    if [ ! -f "$MODDIR/sepolicy.rule" ]; then
        disable_self_with_warning "CORRUPT_MISSING_SEPOLICY"
    fi
fi

# DUAL ENGINE DETECTOR (SusFS vs Non-SusFS)
if [ -w /sys/kernel/susfs ] || [ -d /sys/kernel/susfs ]; then
    echo "ENGINE=SUSFS" > "$ENV_FILE"
    echo "1" > /sys/kernel/susfs/enable 2>/dev/null || true
    echo "add_sus_path /data/adb/modules/root_assistant" > /sys/kernel/susfs/sus_path 2>/dev/null || true
    echo "add_sus_path /data/adb/ksu" > /sys/kernel/susfs/sus_path 2>/dev/null || true
else
    echo "ENGINE=NATIVE_ZYGISK" > "$ENV_FILE"
fi

chmod 644 "$ENV_FILE"
