package com.greenpower2669.spacefortressvs;

import android.app.ActivityManager;
import android.app.ApplicationExitInfo;
import android.content.Context;
import android.os.Build;
import android.os.Process;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * DEBUG APK ONLY. A small persistent journal that survives the game closing.
 * Nothing is uploaded, no runtime permissions or phone-wide logcat access.
 * Users choose where to export the resulting UTF-8 text file.
 */
public final class SpaceFortressDebugLog {
    private static final String TAG="SpaceFortressDebug";
    private static final int MAX_BYTES=256 * 1024;
    private static final int MAX_EXPORT_BYTES=MAX_BYTES * 3;
    private static final Object LOCK=new Object();
    private SpaceFortressDebugLog(){}

    private static String when(long time) {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS Z", Locale.FRANCE)
                .format(new Date(time));
    }

    private static File file(Context context) {
        // One journal per process: SOLO and the original game cannot overwrite
        // each other's last recorded event.
        String suffix="main";
        if(Build.VERSION.SDK_INT>=28) {
            String process=android.app.Application.getProcessName();
            if(process!=null && process.endsWith(":solo")) suffix="solo";
        }
        return new File(context.getFilesDir(),"spacefortress-java-"+suffix+".log");
    }

    public static void record(Context context,String event) {
        if(context==null || event==null)return;
        String line=when(System.currentTimeMillis())+" pid="+Process.myPid()+
                " "+event.replace('\n',' ').replace('\r',' ')+"\n";
        synchronized(LOCK) {
            try {
                File output=file(context);
                if(output.length()>MAX_BYTES) {
                    // Explicit rotation. The header records loss of old events.
                    try(FileOutputStream reset=new FileOutputStream(output,false)){
                        reset.write(("Journal limite a 256 Ko; anciens evenements effaces.\n")
                                .getBytes(StandardCharsets.UTF_8));
                    }
                }
                try(FileOutputStream stream=new FileOutputStream(output,true)){
                    stream.write(line.getBytes(StandardCharsets.UTF_8));
                    stream.flush();
                }
            }catch(Exception e){
                Log.w(TAG,"Impossible d'enregistrer le journal",e);
            }
        }
    }

    public static void recordCrash(Context context,Thread thread,Throwable error) {
        try {
            StringWriter text=new StringWriter();
            PrintWriter printer=new PrintWriter(text);
            if(error!=null)error.printStackTrace(printer);
            printer.flush();
            record(context,"JAVA_EXCEPTION thread="+
                    (thread==null?"unknown":thread.getName())+"\n"+
                    text.toString().replace("\r",""));
        }catch(Throwable ignored){
            // Never obscure the original exception.
        }
    }

    private static String tail(File file) {
        if(!file.isFile())return "(aucun journal dans cette version)\n";
        try(FileInputStream in=new FileInputStream(file)){
            long size=file.length();
            long skip=Math.max(0,size-MAX_BYTES);
            while(skip>0){
                long n=in.skip(skip);
                if(n<=0)break;
                skip-=n;
            }
            byte[] bytes=new byte[(int)Math.min(MAX_BYTES,file.length())];
            int offset=0;
            while(offset<bytes.length){
                int n=in.read(bytes,offset,bytes.length-offset);
                if(n<0)break;
                offset+=n;
            }
            return new String(bytes,0,offset,StandardCharsets.UTF_8)+"\n";
        }catch(IOException e){
            return "(lecture impossible: "+e.getClass().getSimpleName()+")\n";
        }
    }

    private static String reason(int value) {
        switch(value){
            case ApplicationExitInfo.REASON_ANR:return "ANR";
            case ApplicationExitInfo.REASON_CRASH:return "JAVA_CRASH";
            case ApplicationExitInfo.REASON_CRASH_NATIVE:return "NATIVE_CRASH";
            case ApplicationExitInfo.REASON_LOW_MEMORY:return "LOW_MEMORY";
            case ApplicationExitInfo.REASON_SIGNALED:return "SIGNAL";
            case ApplicationExitInfo.REASON_USER_REQUESTED:return "USER_REQUESTED";
            case ApplicationExitInfo.REASON_EXIT_SELF:return "EXIT_SELF";
            case ApplicationExitInfo.REASON_DEPENDENCY_DIED:return "DEPENDENCY_DIED";
            default:return "OTHER_"+value;
        }
    }

    public static String export(Context context) {
        StringBuilder out=new StringBuilder();
        out.append("SPACEFORTRESS — DIAGNOSTIC LOCAL (APK DEBUG)\n");
        out.append("Date: ").append(when(System.currentTimeMillis())).append('\n');
        out.append("Android: ").append(Build.VERSION.SDK_INT).append('\n');
        out.append("Modele: ").append(Build.MANUFACTURER).append(' ')
                .append(Build.MODEL).append('\n');
        out.append("Application: ").append(context.getPackageName()).append('\n');
        out.append("La compilation GitHub ne contient PAS les plantages du telephone.\n");
        out.append("Ce rapport n'inclut pas le logcat global ni la memoire du telephone.\n");

        out.append("\n===== SORTIES ANDROID DES PROCESSUS =====\n");
        if(Build.VERSION.SDK_INT>=30) {
            try {
                ActivityManager manager=(ActivityManager)
                        context.getSystemService(Context.ACTIVITY_SERVICE);
                List<ApplicationExitInfo> exits=manager==null?null:
                        manager.getHistoricalProcessExitReasons(
                                context.getPackageName(),0,12);
                if(exits==null || exits.isEmpty())out.append("Aucune sortie connue.\n");
                else for(ApplicationExitInfo e:exits){
                    out.append(when(e.getTimestamp())).append(" process=")
                       .append(e.getProcessName()).append(" cause=")
                       .append(reason(e.getReason())).append(" status=")
                       .append(e.getStatus()).append("\n");
                }
            }catch(Exception e){
                out.append("Indisponible: ").append(e.getClass().getSimpleName()).append("\n");
            }
        }else out.append("Requiert Android 11+.\n");

        String[] files={
                "spacefortress-java-main.log","spacefortress-java-solo.log",
                "spacefortress-native-classic.log","spacefortress-native-solo.log"
        };
        for(String name:files){
            out.append("\n===== ").append(name).append(" =====\n");
            out.append(tail(new File(context.getFilesDir(),name)));
        }
        if(out.length()>MAX_EXPORT_BYTES)out.setLength(MAX_EXPORT_BYTES);
        out.append("\n===== FIN DIAGNOSTIC =====\n");
        return out.toString();
    }
}
