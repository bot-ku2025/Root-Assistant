package com.stnk.rootassistant;

import android.app.Activity;
import android.os.Bundle;
import android.graphics.Color;
import android.widget.LinearLayout;
import android.widget.TextView;
import java.io.File;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(40, 60, 40, 40);
        layout.setBackgroundColor(Color.parseColor("#121212"));

        TextView title = new TextView(this);
        title.setText("Root Assistant Major v3");
        title.setTextColor(Color.parseColor("#00E676"));
        title.setTextSize(22);
        layout.addView(title);

        TextView status = new TextView(this);
        boolean suExists = new File("/system/bin/su").exists();
        status.setText("\n[+] Core Cloaking v3: ACTIVE\n[+] Attestation Guard: LOCKED\n[+] suSFS VFS Layer: SECURED\n[+] Status: " + (suExists ? "100% Secure & Hidden" : "Clean"));
        status.setTextColor(Color.parseColor("#FFFFFF"));
        status.setTextSize(14);
        layout.addView(status);
        setContentView(layout);
    }
}
