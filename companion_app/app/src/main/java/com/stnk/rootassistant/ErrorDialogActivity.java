package com.stnk.rootassistant;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;

public class ErrorDialogActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String errorMsg = getIntent().getStringExtra("error_msg");
        if (errorMsg == null || errorMsg.isEmpty()) {
            errorMsg = "Unknown critical boot error.";
        }

        final String finalLog = "⚠️ Root Assistant Safe-Mode Triggered!\n" +
                "Modul dinonaktifkan otomatis untuk mencegah bootloop.\n\n" +
                "Lokasi Error:\n" + errorMsg;

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Root Assistant Safe-Mode");
        builder.setMessage(finalLog);
        builder.setCancelable(false);

        // Tombol 1: Salin Log & Buka Telegram
        builder.setPositiveButton("Kirim Ke Author", (dialog, which) -> {
            ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
            ClipData clip = ClipData.newPlainText("RootAssistantError", finalLog);
            if (clipboard != null) {
                clipboard.setPrimaryClip(clip);
                Toast.makeText(this, "Log berhasil disalin ke clipboard!", Toast.LENGTH_SHORT).show();
            }

            Intent tgIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://t.me/MuhammadDimasRidho"));
            tgIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(tgIntent);
            finish();
        });

        // Tombol 2: OK / Tutup Dialog
        builder.setNegativeButton("OK", (dialog, which) -> {
            dialog.dismiss();
            finish();
        });

        AlertDialog dialog = builder.create();
        dialog.show();
    }
}
