package com.greenpower2669.spacefortressvs;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

/**
 * Debug APK entry point. One visible launcher reaches BOTH the untouched
 * original SpaceFortress menu and the separate SOLO prototype.
 * Release builds still open SpaceFortressActivity directly.
 *
 * The historical choices themselves remain in their existing game menu:
 * this hub never mislabels a mode as individually deep-linked.
 */
public final class SpaceFortressHubActivity extends Activity {
    private static final int WHITE = Color.rgb(246, 250, 255);
    private static final int MUTED = Color.rgb(184, 207, 234);

    private int dp(float value) {
        return Math.round(value * getResources().getDisplayMetrics().density);
    }

    private GradientDrawable background(int top, int bottom, int radiusDp) {
        GradientDrawable shape = new GradientDrawable(
                GradientDrawable.Orientation.TOP_BOTTOM, new int[]{top, bottom});
        shape.setCornerRadius(dp(radiusDp));
        shape.setStroke(dp(2), Color.rgb(133, 186, 229));
        return shape;
    }

    private TextView label(String value, int sizeSp, int color, boolean heavy) {
        TextView view = new TextView(this);
        view.setText(value);
        view.setTextColor(color);
        view.setTextSize(sizeSp);
        view.setGravity(Gravity.CENTER);
        view.setIncludeFontPadding(true);
        if (heavy) view.setTypeface(Typeface.DEFAULT, Typeface.BOLD);
        return view;
    }

    private void put(LinearLayout parent, View child, int minHeightDp, int marginDp) {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, minHeightDp > 0 ? dp(minHeightDp)
                        : LinearLayout.LayoutParams.WRAP_CONTENT);
        params.setMargins(0, dp(marginDp), 0, dp(marginDp));
        parent.addView(child, params);
    }

    private TextView action(LinearLayout parent, String title, String description,
                            int topColor, int bottomColor, Class<?> activity) {
        LinearLayout card = new LinearLayout(this);
        card.setOrientation(LinearLayout.VERTICAL);
        card.setGravity(Gravity.CENTER);
        card.setPadding(dp(12), dp(12), dp(12), dp(12));
        card.setBackground(background(topColor, bottomColor, 18));
        card.setClickable(true);
        card.setFocusable(true);
        card.setContentDescription(title + ". " + description);
        TextView button = label(title, 23, WHITE, true);
        TextView subtitle = label(description, 17, WHITE, false);
        card.addView(button);
        card.addView(subtitle);
        card.setOnClickListener(v -> startActivity(new Intent(this, activity)));
        put(parent, card, 110, 10);
        return button;
    }

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        // The historical SDL theme is fullscreen; the native hub is easier to
        // read when its top and bottom content aren't beneath system bars.
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().setStatusBarColor(Color.rgb(6, 12, 28));
        getWindow().setNavigationBarColor(Color.rgb(6, 12, 28));

        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(true);
        scroll.setBackgroundColor(Color.rgb(6, 12, 28));
        // Android 16's edge-to-edge system bars must not cover the title or
        // the big mode buttons (also safe for minSdk 23).
        scroll.setOnApplyWindowInsetsListener((v, insets) -> {
            v.setPadding(insets.getSystemWindowInsetLeft(),
                    insets.getSystemWindowInsetTop(),
                    insets.getSystemWindowInsetRight(),
                    insets.getSystemWindowInsetBottom());
            return insets;
        });
        LinearLayout body = new LinearLayout(this);
        body.setOrientation(LinearLayout.VERTICAL);
        body.setGravity(Gravity.CENTER_HORIZONTAL);
        body.setPadding(dp(22), dp(22), dp(22), dp(22));
        scroll.addView(body);
        setContentView(scroll);

        put(body, label("SPACE FORTRESS", 34, WHITE, true), 55, 2);
        put(body, label("CHOISIS TON AVENTURE", 20,
                        Color.rgb(255, 222, 120), true), 44, 1);
        put(body, label("CLASSIQUE  ·  DUEL  ·  VS IA\n"
                      + "COOPERATION  ·  CAMPAGNE", 18, MUTED, true), 75, 10);

        action(body, "JEU ORIGINAL", "Ouvrir le menu avec tous les modes historiques",
                Color.rgb(25, 86, 147), Color.rgb(24, 47, 94),
                SpaceFortressActivity.class);
        action(body, "MODE SOLO", "Mondes, niveaux et combats : prototype en test",
                Color.rgb(28, 143, 108), Color.rgb(14, 74, 87),
                SpaceFortressSoloActivity.class);

        put(body, label("TES MODES HISTORIQUES SONT PRESERVES", 16,
                        MUTED, true), 50, 7);
        put(body, label("Version de developpement · Aucun score original efface",
                        14, MUTED, false), 50, 2);
    }
}
