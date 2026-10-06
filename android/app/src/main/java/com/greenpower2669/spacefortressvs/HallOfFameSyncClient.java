package com.greenpower2669.spacefortressvs;

import org.json.JSONException;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.net.HttpURLConnection;
import java.net.NoRouteToHostException;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.net.URLEncoder;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import javax.net.ssl.HttpsURLConnection;

public final class HallOfFameSyncClient {
    private HallOfFameSyncClient() {}

    public static final String BASE_URL = "https://fab-hall-of-fame.gnrationsia.chatgpt.site";
    private static final String SCORE_PATH = "/api/v1/games/spacefortressvs/scores";
    private static final String SYNC_PATH = "/api/v1/games/spacefortressvs/sync";
    private static final int CONNECT_TIMEOUT_MS = 8000;
    private static final int READ_TIMEOUT_MS = 10000;

    public static final int ERROR_NONE = 0;
    public static final int ERROR_OFFLINE = 1;
    public static final int ERROR_TIMEOUT = 2;
    public static final int ERROR_AUTH = 3;
    public static final int ERROR_SERVER = 4;
    public static final int ERROR_PROTOCOL = 5;
    public static final int ERROR_NOT_CONFIGURED = 6;

    private static final ExecutorService EXECUTOR = Executors.newSingleThreadExecutor(r -> {
        Thread thread = new Thread(r, "SpaceFortress-HallSync");
        thread.setDaemon(true);
        return thread;
    });

    public static boolean isUploadConfigured() {
        return BuildConfig.HOF_API_KEY != null && !BuildConfig.HOF_API_KEY.isEmpty();
    }

    public static String gameVersion() {
        return BuildConfig.VERSION_NAME;
    }

    public static void submit(String submissionId,
                              String playerName,
                              String pilot0,
                              String pilot1,
                              int boss,
                              String difficulty,
                              long durationMs,
                              int points,
                              int stars,
                              String gameVersion,
                              long completedAtEpochSeconds,
                              String mode,
                              int encounter) {
        if (!isUploadConfigured()) {
            nativeUploadFailed(submissionId, ERROR_NOT_CONFIGURED);
            return;
        }
        HallOfFameSyncProtocol.Upload upload = new HallOfFameSyncProtocol.Upload();
        upload.submissionId = submissionId;
        upload.playerName = playerName;
        upload.pilots = new String[] {pilot0, pilot1};
        upload.boss = boss;
        upload.difficulty = difficulty;
        upload.durationMs = durationMs;
        upload.points = points;
        upload.stars = stars;
        upload.gameVersion = gameVersion;
        upload.completedAtEpochSeconds = completedAtEpochSeconds;
        upload.mode = mode;
        upload.encounter = encounter;
        EXECUTOR.execute(() -> doSubmit(upload));
    }

    public static void syncPage(long cycleId, String cursor, int limit) {
        EXECUTOR.execute(() -> doSyncPage(cycleId, cursor, limit));
    }

    private static void doSubmit(HallOfFameSyncProtocol.Upload upload) {
        HttpsURLConnection connection = null;
        try {
            byte[] payload = HallOfFameSyncProtocol.encodeUpload(upload).getBytes(StandardCharsets.UTF_8);
            connection = open(new URL(BASE_URL + SCORE_PATH));
            connection.setRequestMethod("POST");
            connection.setDoOutput(true);
            connection.setRequestProperty("Content-Type", "application/json; charset=utf-8");
            connection.setRequestProperty("Accept", "application/json");
            connection.setRequestProperty("Authorization", HallOfFameSyncProtocol.authorizationValue(BuildConfig.HOF_API_KEY));
            connection.setFixedLengthStreamingMode(payload.length);
            try (OutputStream out = connection.getOutputStream()) {
                out.write(payload);
            }
            int code = connection.getResponseCode();
            String body = readBody(connection, code);
            if (code == HttpURLConnection.HTTP_CREATED || code == HttpURLConnection.HTTP_OK) {
                HallOfFameSyncProtocol.UploadReply reply = HallOfFameSyncProtocol.decodeUploadReply(code, body);
                if (!upload.submissionId.equals(reply.submissionId)) {
                    nativeUploadFailed(upload.submissionId, ERROR_PROTOCOL);
                    return;
                }
                nativeUploadAccepted(upload.submissionId, reply.id);
                return;
            }
            nativeUploadFailed(upload.submissionId, errorCodeForHttp(code));
        } catch (SocketTimeoutException e) {
            nativeUploadFailed(upload.submissionId, ERROR_TIMEOUT);
        } catch (UnknownHostException | ConnectException | NoRouteToHostException e) {
            nativeUploadFailed(upload.submissionId, ERROR_OFFLINE);
        } catch (JSONException e) {
            nativeUploadFailed(upload.submissionId, ERROR_PROTOCOL);
        } catch (IOException e) {
            nativeUploadFailed(upload.submissionId, ERROR_OFFLINE);
        } catch (RuntimeException e) {
            nativeUploadFailed(upload.submissionId, ERROR_PROTOCOL);
        } finally {
            if (connection != null) connection.disconnect();
        }
    }

    private static void doSyncPage(long cycleId, String cursor, int limit) {
        HttpsURLConnection connection = null;
        String requestedCursor = cursor == null ? "0" : cursor;
        try {
            String query = "?cursor=" + URLEncoder.encode(requestedCursor, "UTF-8") + "&limit=" + Math.max(1, Math.min(limit, 100));
            connection = open(new URL(BASE_URL + SYNC_PATH + query));
            connection.setRequestMethod("GET");
            connection.setRequestProperty("Accept", "application/json");
            int code = connection.getResponseCode();
            String body = readBody(connection, code);
            if (code != HttpURLConnection.HTTP_OK) {
                nativePageFailed(cycleId, requestedCursor, errorCodeForHttp(code));
                return;
            }
            HallOfFameSyncProtocol.SyncPage page = HallOfFameSyncProtocol.decodeSyncPage(body);
            for (HallOfFameSyncProtocol.SyncEntry entry : page.entries) {
                nativeRemoteEntry(cycleId,
                        entry.id,
                        entry.submissionId,
                        entry.playerName,
                        entry.pilots[0],
                        entry.pilots[1],
                        entry.boss,
                        entry.difficulty,
                        entry.durationMs,
                        entry.points,
                        entry.stars,
                        entry.completedAt,
                        entry.mode,
                        entry.encounter,
                        entry.serverRank);
            }
            nativePageDone(cycleId, requestedCursor, page.nextCursor, page.hasMore);
        } catch (SocketTimeoutException e) {
            nativePageFailed(cycleId, requestedCursor, ERROR_TIMEOUT);
        } catch (UnknownHostException | ConnectException | NoRouteToHostException e) {
            nativePageFailed(cycleId, requestedCursor, ERROR_OFFLINE);
        } catch (JSONException e) {
            nativePageFailed(cycleId, requestedCursor, ERROR_PROTOCOL);
        } catch (IOException e) {
            nativePageFailed(cycleId, requestedCursor, ERROR_OFFLINE);
        } catch (RuntimeException e) {
            nativePageFailed(cycleId, requestedCursor, ERROR_PROTOCOL);
        } finally {
            if (connection != null) connection.disconnect();
        }
    }

    private static HttpsURLConnection open(URL url) throws IOException {
        HttpsURLConnection connection = (HttpsURLConnection) url.openConnection();
        connection.setConnectTimeout(CONNECT_TIMEOUT_MS);
        connection.setReadTimeout(READ_TIMEOUT_MS);
        connection.setUseCaches(false);
        return connection;
    }

    private static String readBody(HttpsURLConnection connection, int code) throws IOException {
        InputStream stream = code >= 200 && code < 400 ? connection.getInputStream() : connection.getErrorStream();
        if (stream == null) return "";
        try (InputStream in = stream; ByteArrayOutputStream out = new ByteArrayOutputStream()) {
            byte[] buffer = new byte[4096];
            int read;
            while ((read = in.read(buffer)) != -1) out.write(buffer, 0, read);
            return new String(out.toByteArray(), StandardCharsets.UTF_8);
        }
    }

    private static int errorCodeForHttp(int status) {
        HallOfFameSyncProtocol.ErrorKind kind = HallOfFameSyncProtocol.classifyHttpStatus(status);
        switch (kind) {
            case AUTH: return ERROR_AUTH;
            case SERVER: return ERROR_SERVER;
            case NONE: return ERROR_NONE;
            default: return ERROR_PROTOCOL;
        }
    }

    private static native void nativeUploadAccepted(String submissionId, String serverId);
    private static native void nativeUploadFailed(String submissionId, int errorCode);
    private static native void nativeRemoteEntry(long cycleId,
                                                 String id,
                                                 String submissionId,
                                                 String playerName,
                                                 String pilot0,
                                                 String pilot1,
                                                 int boss,
                                                 String difficulty,
                                                 long durationMs,
                                                 int points,
                                                 int stars,
                                                 String completedAt,
                                                 String mode,
                                                 int encounter,
                                                 int serverRank);
    private static native void nativePageDone(long cycleId, String requestedCursor, String nextCursor, boolean hasMore);
    private static native void nativePageFailed(long cycleId, String requestedCursor, int errorCode);
}
