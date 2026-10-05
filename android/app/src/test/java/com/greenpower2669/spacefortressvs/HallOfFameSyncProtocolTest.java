package com.greenpower2669.spacefortressvs;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.junit.Test;

public class HallOfFameSyncProtocolTest {
    private HallOfFameSyncProtocol.Upload sampleUpload() {
        HallOfFameSyncProtocol.Upload upload = new HallOfFameSyncProtocol.Upload();
        upload.submissionId = "4de285f2-110e-4dc1-a7ce-dc1c5cb9dfe9";
        upload.playerName = "TEST1";
        upload.pilots = new String[] {"ORION IA", "FAB"};
        upload.boss = 1;
        upload.difficulty = "APOCALYPSE";
        upload.durationMs = 87000;
        upload.points = 15;
        upload.stars = 9;
        upload.gameVersion = "1.4.0";
        upload.completedAtEpochSeconds = 1791217319L;
        upload.mode = "coop-ai";
        upload.encounter = 1;
        return upload;
    }

    @Test
    public void encodeUploadMapsExactProtocolFieldsAndUtcTime() throws Exception {
        JSONObject json = new JSONObject(HallOfFameSyncProtocol.encodeUpload(sampleUpload()));
        assertEquals("4de285f2-110e-4dc1-a7ce-dc1c5cb9dfe9", json.getString("submissionId"));
        assertEquals("TEST1", json.getString("playerName"));
        assertEquals("ORION IA", json.getJSONArray("pilots").getString(0));
        assertEquals("FAB", json.getJSONArray("pilots").getString(1));
        assertEquals(1, json.getInt("boss"));
        assertEquals("APOCALYPSE", json.getString("difficulty"));
        assertEquals(87000L, json.getLong("durationMs"));
        assertEquals(15, json.getInt("points"));
        assertEquals(9, json.getInt("stars"));
        assertEquals("1.4.0", json.getString("gameVersion"));
        assertEquals("2026-10-05T16:21:59Z", json.getString("completedAt"));
        assertEquals("coop-ai", json.getString("mode"));
        assertEquals(1, json.getInt("encounter"));
        assertEquals(12, json.length());
    }

    @Test
    public void authorizationUsesFakeKeyOnlyInTests() {
        assertEquals("Bearer test-key", HallOfFameSyncProtocol.authorizationValue("test-key"));
        assertEquals("", HallOfFameSyncProtocol.authorizationValue(""));
        assertEquals("", HallOfFameSyncProtocol.authorizationValue(null));
    }

    @Test
    public void uploadRepliesRecognizeCreatedAndAlreadyReceived() throws Exception {
        String accepted = "{\"id\":\"srv-1\",\"submissionId\":\"sub-1\",\"status\":\"accepted\",\"sequence\":1,\"receivedAt\":\"2026-10-05T16:22:00Z\"}";
        HallOfFameSyncProtocol.UploadReply first = HallOfFameSyncProtocol.decodeUploadReply(201, accepted);
        assertEquals("srv-1", first.id);
        assertFalse(first.alreadyReceived);

        String duplicate = "{\"id\":\"srv-1\",\"submissionId\":\"sub-1\",\"status\":\"already_received\",\"sequence\":1,\"receivedAt\":\"2026-10-05T16:22:00Z\"}";
        HallOfFameSyncProtocol.UploadReply again = HallOfFameSyncProtocol.decodeUploadReply(200, duplicate);
        assertEquals("srv-1", again.id);
        assertTrue(again.alreadyReceived);
    }

    @Test
    public void httpStatusClassificationSeparatesAuthServerAndProtocol() {
        assertEquals(HallOfFameSyncProtocol.ErrorKind.AUTH, HallOfFameSyncProtocol.classifyHttpStatus(401));
        assertEquals(HallOfFameSyncProtocol.ErrorKind.AUTH, HallOfFameSyncProtocol.classifyHttpStatus(403));
        assertEquals(HallOfFameSyncProtocol.ErrorKind.SERVER, HallOfFameSyncProtocol.classifyHttpStatus(500));
        assertEquals(HallOfFameSyncProtocol.ErrorKind.NONE, HallOfFameSyncProtocol.classifyHttpStatus(201));
        assertEquals(HallOfFameSyncProtocol.ErrorKind.PROTOCOL, HallOfFameSyncProtocol.classifyHttpStatus(429));
    }

    @Test
    public void syncPageParsesUnicodeCursorRankAndOptionalRank() throws Exception {
        JSONObject first = new JSONObject();
        first.put("id", "srv-1");
        first.put("submissionId", "sub-1");
        first.put("playerName", "ÉQUIPE TEST");
        first.put("pilots", new JSONArray().put("ORION IA").put("FAB"));
        first.put("boss", 200);
        first.put("difficulty", "APOCALYPSE");
        first.put("durationMs", 87000);
        first.put("points", 1466);
        first.put("stars", 9);
        first.put("completedAt", "2026-10-05T16:21:59Z");
        first.put("mode", "coop-ai");
        first.put("encounter", 200);
        first.put("rank", 3);

        JSONObject second = new JSONObject(first.toString());
        second.put("id", "srv-2");
        second.put("submissionId", "sub-2");
        second.remove("rank");

        JSONObject root = new JSONObject();
        root.put("entries", new JSONArray().put(first).put(second));
        root.put("nextCursor", 100);
        root.put("hasMore", true);

        HallOfFameSyncProtocol.SyncPage page = HallOfFameSyncProtocol.decodeSyncPage(root.toString());
        assertEquals(2, page.entries.size());
        assertEquals("ÉQUIPE TEST", page.entries.get(0).playerName);
        assertEquals(3, page.entries.get(0).serverRank);
        assertEquals(0, page.entries.get(1).serverRank);
        assertEquals("100", page.nextCursor);
        assertTrue(page.hasMore);
    }

    @Test
    public void malformedJsonAndInvalidRangesAreRejected() throws Exception {
        try {
            HallOfFameSyncProtocol.decodeSyncPage("{\"entries\":[],\"hasMore\":false}");
            fail("missing nextCursor must fail");
        } catch (JSONException expected) {
            // expected
        }

        HallOfFameSyncProtocol.Upload invalid = sampleUpload();
        invalid.stars = 10;
        try {
            HallOfFameSyncProtocol.encodeUpload(invalid);
            fail("invalid stars must fail");
        } catch (JSONException expected) {
            // expected
        }
    }
}
