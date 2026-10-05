package com.greenpower2669.spacefortressvs;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.TimeZone;

public final class HallOfFameSyncProtocol {
    private HallOfFameSyncProtocol() {}

    public enum ErrorKind {
        NONE,
        OFFLINE,
        TIMEOUT,
        AUTH,
        SERVER,
        PROTOCOL,
        NOT_CONFIGURED
    }

    public static final class Upload {
        public String submissionId = "";
        public String playerName = "";
        public String[] pilots = new String[] {"", ""};
        public int boss;
        public String difficulty = "";
        public long durationMs;
        public int points;
        public int stars;
        public String gameVersion = "";
        public long completedAtEpochSeconds;
        public String mode = "";
        public int encounter;
    }

    public static final class UploadReply {
        public String id = "";
        public String submissionId = "";
        public String status = "";
        public long sequence;
        public String receivedAt = "";
        public boolean alreadyReceived;
    }

    public static final class SyncEntry {
        public String id = "";
        public String submissionId = "";
        public String playerName = "";
        public String[] pilots = new String[] {"", ""};
        public int boss;
        public String difficulty = "";
        public long durationMs;
        public int points;
        public int stars;
        public String completedAt = "";
        public String mode = "";
        public int encounter;
        public int serverRank;
    }

    public static final class SyncPage {
        public final List<SyncEntry> entries = new ArrayList<>();
        public String nextCursor = "0";
        public boolean hasMore;
    }

    public static String encodeUpload(Upload upload) throws JSONException {
        requireUpload(upload);
        JSONObject root = new JSONObject();
        root.put("submissionId", upload.submissionId);
        root.put("playerName", upload.playerName);
        JSONArray pilots = new JSONArray();
        pilots.put(upload.pilots[0]);
        pilots.put(upload.pilots[1]);
        root.put("pilots", pilots);
        root.put("boss", upload.boss);
        root.put("difficulty", upload.difficulty);
        root.put("durationMs", upload.durationMs);
        root.put("points", upload.points);
        root.put("stars", upload.stars);
        root.put("gameVersion", upload.gameVersion);
        root.put("completedAt", formatUtc(upload.completedAtEpochSeconds));
        root.put("mode", upload.mode);
        root.put("encounter", upload.encounter);
        return root.toString();
    }

    public static String authorizationValue(String key) {
        return key == null || key.isEmpty() ? "" : "Bearer " + key;
    }

    public static UploadReply decodeUploadReply(int httpCode, String body) throws JSONException {
        if (httpCode != 201 && httpCode != 200) {
            throw new JSONException("unexpected upload status " + httpCode);
        }
        JSONObject root = new JSONObject(body == null ? "" : body);
        UploadReply reply = new UploadReply();
        reply.id = requiredString(root, "id");
        reply.submissionId = requiredString(root, "submissionId");
        reply.status = requiredString(root, "status");
        reply.sequence = root.optLong("sequence", 0L);
        reply.receivedAt = requiredString(root, "receivedAt");
        if (httpCode == 201) {
            if (!"accepted".equals(reply.status)) throw new JSONException("201 without accepted");
        } else {
            if (!"already_received".equals(reply.status)) throw new JSONException("200 without already_received");
            reply.alreadyReceived = true;
        }
        return reply;
    }

    public static SyncPage decodeSyncPage(String body) throws JSONException {
        JSONObject root = new JSONObject(body == null ? "" : body);
        JSONArray array = root.getJSONArray("entries");
        SyncPage page = new SyncPage();
        Object rawCursor = root.get("nextCursor");
        page.nextCursor = String.valueOf(rawCursor);
        if (page.nextCursor.isEmpty() || "null".equals(page.nextCursor)) throw new JSONException("missing nextCursor");
        page.hasMore = root.getBoolean("hasMore");
        for (int i = 0; i < array.length(); ++i) {
            JSONObject item = array.getJSONObject(i);
            SyncEntry entry = new SyncEntry();
            entry.id = requiredString(item, "id");
            entry.submissionId = item.optString("submissionId", "");
            entry.playerName = requiredString(item, "playerName");
            JSONArray pilots = item.getJSONArray("pilots");
            if (pilots.length() != 2) throw new JSONException("pilots must contain two names");
            entry.pilots[0] = pilots.getString(0);
            entry.pilots[1] = pilots.getString(1);
            if (entry.pilots[0].isEmpty() || entry.pilots[1].isEmpty()) throw new JSONException("empty pilot");
            entry.boss = item.getInt("boss");
            entry.difficulty = requiredString(item, "difficulty");
            entry.durationMs = item.getLong("durationMs");
            entry.points = item.getInt("points");
            entry.stars = item.getInt("stars");
            entry.completedAt = requiredString(item, "completedAt");
            entry.mode = requiredString(item, "mode");
            entry.encounter = item.getInt("encounter");
            entry.serverRank = item.has("rank") && !item.isNull("rank") ? item.getInt("rank") : 0;
            validateEntry(entry);
            page.entries.add(entry);
        }
        return page;
    }

    public static ErrorKind classifyHttpStatus(int status) {
        if (status == 401 || status == 403) return ErrorKind.AUTH;
        if (status >= 500 && status <= 599) return ErrorKind.SERVER;
        if (status >= 200 && status <= 299) return ErrorKind.NONE;
        return ErrorKind.PROTOCOL;
    }

    private static String formatUtc(long epochSeconds) {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'", Locale.US);
        format.setTimeZone(TimeZone.getTimeZone("UTC"));
        return format.format(new Date(epochSeconds * 1000L));
    }

    private static String requiredString(JSONObject object, String key) throws JSONException {
        String value = object.getString(key);
        if (value == null || value.isEmpty()) throw new JSONException("missing " + key);
        return value;
    }

    private static void requireUpload(Upload upload) throws JSONException {
        if (upload == null) throw new JSONException("null upload");
        if (upload.submissionId == null || upload.submissionId.isEmpty()) throw new JSONException("submissionId");
        if (upload.playerName == null || upload.playerName.isEmpty()) throw new JSONException("playerName");
        if (upload.pilots == null || upload.pilots.length != 2 || upload.pilots[0] == null || upload.pilots[0].isEmpty() || upload.pilots[1] == null || upload.pilots[1].isEmpty()) throw new JSONException("pilots");
        if (upload.boss < 1 || upload.boss > 200 || upload.encounter < 1 || upload.encounter > 200) throw new JSONException("boss/encounter");
        if (upload.difficulty == null || upload.difficulty.isEmpty() || upload.stars < 1 || upload.stars > 9) throw new JSONException("difficulty/stars");
        if (upload.durationMs < 0 || upload.points < 0) throw new JSONException("duration/points");
        if (upload.gameVersion == null || upload.gameVersion.isEmpty()) throw new JSONException("gameVersion");
        if (upload.mode == null || upload.mode.isEmpty()) throw new JSONException("mode");
    }

    private static void validateEntry(SyncEntry entry) throws JSONException {
        if (entry.boss < 1 || entry.boss > 200 || entry.encounter < 1 || entry.encounter > 200) throw new JSONException("remote boss/encounter");
        if (entry.stars < 1 || entry.stars > 9 || entry.durationMs < 0 || entry.points < 0 || entry.serverRank < 0) throw new JSONException("remote ranges");
    }
}
