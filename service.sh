#!/system/bin/sh
MODDIR=${0%/*}

until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 2
done

# Cek apakah modul dalam mode Toleransi Crash
if [ -f "$MODDIR/CRASH_WARNING" ]; then
    REASON=$(cat "$MODDIR/CRASH_WARNING")
    
    # Tampilkan Notifikasi Peringatan Keras + Instruksi Penyelamatan (Toleransi)
    cmd notification post -S bigtext \
        -t "⚠️ PERINGATAN KERAS: ROOT ASSISTANT" \
        "Modul di-DISABLE otomatis ($REASON). Silakan HAPUS MODUL atau UPDATE via Root Manager untuk memperbaiki. JANGAN aktifkan paksa/reboot tanpa perbaikan atau akan BOOTLOOP TOTAL!" \
        "root_assistant_alert" 2>/dev/null || \
    am broadcast -a android.intent.action.MAIN \
        --es title "⚠️ CRASH WARNING" \
        --es msg "Root Assistant di-disable. Hapus modul atau Update di Root Manager untuk mencegah Bootloop Total!" 2>/dev/null

    exit 0
fi

# Jika boot normal & sukses -> Reset counter crash
rm -f "$MODDIR/crash_count"
rm -f "$MODDIR/CRASH_WARNING"
