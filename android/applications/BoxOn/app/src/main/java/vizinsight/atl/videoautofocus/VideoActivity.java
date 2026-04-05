package vizinsight.atl.videoautofocus;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.AsyncTask;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.jcodec.api.android.AndroidSequenceEncoder;
import org.jcodec.common.io.NIOUtils;
import org.jcodec.common.io.SeekableByteChannel;
import org.jcodec.common.model.Rational;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import vizinsight.atl.object_detector.VZObjectDetector;
import vizinsight.atl.object_detector.VZEntity;
public class VideoActivity extends AppCompatActivity {
    private static final String TAG = "IR_TESTAPP_VideoActivity";
    private Context mContext;
    private String mediPathPrivate;
    private int count = 0;
    private int totalFramesPrivate = 0;
    private long totalEngineProcessingTime = 0;
    private long totalAppDecodeTime = 0;
    private Queue<Bitmap> frameListPrivate = null;
    Handler handle;
    final int SEND_FRAMES_TO_ENGINE = 1;
    final int PROCESSING_COMPLETE = 2;
    final int MAX_BATCH_SIZE = 8;
    private VZObjectDetector image_decoder = null;
    String result_folder_name = "";
    String frames_folder_name = "";
    boolean isMaskwriteRunning = false;
    MaskResultWriteTask maskResultWriteTask;
    PrepareVideoTask prepareVideoTask;
    String defaultModelPath= "/vendor/etc/saiv/image_understanding/db/";
    VZObjectDetector object_detector;
    int odMode = VZObjectDetector.MODE_SALIENCY;
    final int IMAGEFORMAT_NV21 = 0;
    ProgressBar progressBar;
    TextView textView;
    private boolean isPrepareVideoRunning=false;
    private String videoUriString="";
    AlertDialog.Builder dialogBuilder;
    AlertDialog prgrsDialog;
    ProgressBar pgsBar;
    TextView pgsTxtView;
    Queue<Bitmap> resultBitmapQueue = new LinkedList<>();


    @SuppressLint("LongLogTag")
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        mContext = this;
        //progressBar = (ProgressBar)findViewById(R.id.progressBar1);
        pgsBar = (ProgressBar)findViewById(R.id.pgsBar);
        pgsTxtView = (TextView)findViewById(R.id.textView);
        pgsBar.setVisibility(View.GONE);
        object_detector = new VZObjectDetector(odMode, defaultModelPath);
        createVideoProcessHandler();
        videoUriString = getIntent().getStringExtra("videoUri");
        final Uri videoUri = Uri.parse(videoUriString);
        if(videoUri != null){
            if (videoUri.toString().contains("video")) {
                mediPathPrivate = Utils.getPath(mContext, videoUri);

                MediaMetadataRetriever retriever = new MediaMetadataRetriever();
                retriever.setDataSource(mContext, videoUri);
                totalFramesPrivate = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
                Log.d(TAG, "number of frames = " + totalFramesPrivate);
                int result = 0;
                try {
                    count = 0;
                    totalEngineProcessingTime = 0;
                    frameListPrivate = new LinkedList<>();
                    //result = image_classifier.initVideoInfo(mediPathPrivate, totalFramesPrivate);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                Log.d(TAG, "result = " + result);

                if (result == 0) {
                    //progressBar.setVisibility(View.VISIBLE);
                    ShowProgressDialog();
                    Log.d(TAG, "Before thread ");
                    new Thread() {
                        @RequiresApi(api = Build.VERSION_CODES.P)
                        @Override
                        public void run() {
                            Log.d(TAG, "Inside thread");
                            result_folder_name = get_results_folder_name("LOD_VIDEO");
                            File dir = new File(result_folder_name);
                            if(dir.exists()) {
                                deleteDirectory(dir);
                            }
                            dir.mkdirs();
                            frames_folder_name = get_frames_folder_name("LOD_VIDEO");
                            File frameDir = new File(frames_folder_name);
                            if(frameDir.exists()) {
                                deleteDirectory(frameDir);
                            }
                            frameDir.mkdirs();
                            //showToast("Results will be stored in \""+result_folder_name+"\" directory");
                            try {
                                decodeVideoFrames(videoUri, mediPathPrivate);

                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }.start();
                } else if (result == 1) {
                    Log.d(TAG, "Video file already processed earlier, so just get the tags");
                    Message msg = handle.obtainMessage();
                    msg.what = PROCESSING_COMPLETE;
                    handle.sendMessage(msg);
                } else {
                    Log.d(TAG, "Video decode failed");
                }
            } else {
                Toast.makeText(getApplicationContext(), "Please select a video file", Toast.LENGTH_SHORT).show();
            }
        }
    }

   /* @Override
    public void onBackPressed() {
        super.onBackPressed();
        Log.d(TAG,"onBackPressed Resetting the start track flag");
        object_detector.resetStartTracking();
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                object_detector.resetStartTracking();
                File dir = new File(result_folder_name);
                if(dir.exists()) {
                    deleteDirectory(dir);
                }
                frames_folder_name = get_frames_folder_name("LOD_VIDEO");
                File frameDir = new File(frames_folder_name);
                if(frameDir.exists()) {
                    deleteDirectory(frameDir);
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG,"onDestroy Resetting the start track flag");
        object_detector.resetStartTracking();
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                object_detector.resetStartTracking();
                File dir = new File(result_folder_name);
                if(dir.exists()) {
                    deleteDirectory(dir);
                }
                frames_folder_name = get_frames_folder_name("LOD_VIDEO");
                File frameDir = new File(frames_folder_name);
                if(frameDir.exists()) {
                    deleteDirectory(frameDir);
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();

    }*/

    void deleteDirectory(File dirPath){


        for (File subFile : dirPath.listFiles()) {
            if(subFile.isDirectory()) {
                deleteDirectory(subFile);
            } else {
                subFile.delete();
            }
        }
        dirPath.delete();
    }

    private void ShowProgressDialog() {
        /*dialogBuilder = new AlertDialog.Builder(VideoActivity.this);
        LayoutInflater inflater = (LayoutInflater) getSystemService( Context.LAYOUT_INFLATER_SERVICE );
        View dialogView = inflater.inflate(R.layout.progressbar_layout, null);
        dialogBuilder.setView(dialogView);
        dialogBuilder.setCancelable(false);
        prgrsDialog = dialogBuilder.create();
        prgrsDialog.show();*/

        /*pgsBar = (ProgressBar) findViewById(R.id.progressBar);
        pgsTxtView = (TextView) findViewById(R.id.textView);*/
        pgsBar.setVisibility(View.VISIBLE);
        pgsTxtView.setVisibility(View.VISIBLE);
    }
    public void HideProgressDialog(){
        pgsBar.setVisibility(View.GONE);
        pgsTxtView.setVisibility(View.GONE);
    }

    private String get_results_folder_name(String folder_path) {
        //String result_folder_path = "/sdcard/LOD_Video_results/";
        String result_folder_path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_results/";
        //result_folder_path += folder_path.split("/storage/emulated/0/")[1];
        Log.d(TAG,"get_results_folder_name : "+result_folder_path);
        return result_folder_path;
    }

    private String get_frames_folder_name(String folder_path) {
        //String frames_folder_path = "/sdcard/LOD_Video_Frames/";
        String frames_folder_path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_Frames/";
        //result_folder_path += folder_path.split("/storage/emulated/0/")[1];
        Log.d(TAG,"get_results_folder_name : "+frames_folder_path);
        return frames_folder_path;
    }

    private void createVideoProcessHandler() {
        handle = new Handler() {
            @SuppressLint("LongLogTag")
            @Override
            public void handleMessage(Message msg) {
                long millis_startTime = 0, millis_endTime = 0;
                super.handleMessage(msg);

                if (msg.what == SEND_FRAMES_TO_ENGINE) {
                    if(!isMaskwriteRunning){
                        writeMaskToResultFolder();
                    }

                } else if (msg.what == PROCESSING_COMPLETE) {
                    Log.d(TAG, "task complete. total time taken to send " + count + " frames = " + totalEngineProcessingTime);
                    /*int i = 0;
                    long timeForLocalisationModel = 0;
                    int[] videoStartEndTime = {0, -1, 30000, 50000, 73000, 122000};
                    double[] scores = {0.0f, 0.0f, 0.0f};
                    String[] videoClassNames = {"Full video", "Skiing", "Sports"};

                    *//*millis_startTime = System.currentTimeMillis();

                    millis_endTime = System.currentTimeMillis();
                    timeForLocalisationModel = millis_endTime - millis_startTime;*/


                } else {
                    Log.d(TAG, "Invalid message" + msg.what);
                }
            }
        };
    }

    private void writeMaskToResultFolder(){
        Log.d(TAG,"writeMaskToResultFolder");
        maskResultWriteTask = new MaskResultWriteTask();
        maskResultWriteTask.execute("");
        isMaskwriteRunning = true;
    }

    private class MaskResultWriteTask extends AsyncTask<String, Integer, Integer>{

        private void showToast(final String message){
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(mContext, message, Toast.LENGTH_SHORT).show();
                }
            });
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
            int progressPercent = count * 100 / (totalFramesPrivate);
            pgsBar.setProgress(progressPercent);
            pgsTxtView.setText("Processing Frame : " +count+"/" + (totalFramesPrivate));
        }

        @Override
        protected Integer doInBackground(String... strings) {
            long millis_startTime = 0, millis_endTime = 0;
            Bitmap bitmap = null;
            int sentCount = 0;
            showToast("Results will be stored in \""+result_folder_name+"\" directory");
            //int total24thFrames = ((totalFramesPrivate - 1) / 24) + 1;
            while (frameListPrivate.size() > 0) {
                bitmap = frameListPrivate.remove();
                //bitmapBytesArray[sentCount] = image_decoder.decodeImage(bitmap, VZImageDecoder.IMAGE_FORMAT_RGBA8888);
                List<VZEntity> entities = object_detector.execute(bitmap, IMAGEFORMAT_NV21);

                if(!(entities== null) && !entities.isEmpty()) {
                    Log.d(TAG,"Entities count : "+entities.size());
                    int width;
                    String image_path = frames_folder_name + count + ".png";
                    String out_filepath = image_path.replace(frames_folder_name, result_folder_name);
                    /*Bitmap bm = BitmapFactory.decodeFile(image_path);
                    Bitmap temp_mask;

                    Log.d(TAG, "image_path : " + image_path);
                    bm = Utils.rotateBitmap(bm, Utils.getImageOrientation(image_path));
                    if(bm == null){
                        bm=bitmap;
                    }
                    temp_mask = bm.copy(Bitmap.Config.ARGB_8888, true);*/
                    width = bitmap.getWidth();
                    Paint p = new Paint();
                    p.setStyle(Paint.Style.STROKE);
                    p.setAntiAlias(true);
                    p.setFilterBitmap(true);
                    p.setDither(true);
                    p.setColor(Color.BLUE);
                    p.setTextSize(width / 30);

                    Rect text_bounds = new Rect();
                    Paint paint_box = new Paint();
                    paint_box.setStyle(Paint.Style.FILL_AND_STROKE);
                    paint_box.setAntiAlias(true);
                    paint_box.setColor(Color.WHITE); //Color.BLUE
                    paint_box.setStrokeWidth(width / 320.0f);
                    Paint paint_text_box = new Paint(paint_box);
                    paint_text_box.setStyle(Paint.Style.FILL);
                    paint_text_box.setTextSize(width / 30);
                    paint_box.setTextSize(width / 30);

                    Canvas c = new Canvas(bitmap);
                    for (int idx = 0; idx < entities.size(); idx++) {
                        VZEntity ent = entities.get(idx);
                        if (ent.tag.contains("et")) {
                            p.setColor(Color.RED);
                            paint_text_box.setColor(Color.WHITE);
                            paint_box.setColor(Color.RED);
                        } else {
                            p.setColor(Color.GREEN);
                            paint_text_box.setColor(Color.TRANSPARENT);
                            paint_box.setColor(Color.GREEN);
                        }
                        String categoryClass = " ";
                        if (ent.tagId == 5) {
                            categoryClass = "Flower";
                        } else if (ent.tagId == 8) {
                            categoryClass = "Petface";
                        }else if (ent.tagId == 6) {
                            categoryClass = "Animal";
                        }
                        c.drawRect(ent.left, ent.top, ent.right, ent.bottom, p);
                        paint_text_box.getTextBounds("#" + ent.ID + "_" + categoryClass, 0, ("#" + ent.ID + "_" + categoryClass).length(), text_bounds);
                        c.drawRect(ent.left + 50, ent.top + 50, ent.left + 50 + text_bounds.width(), ent.top + 50 - text_bounds.height(), paint_text_box);

                        c.drawText("#" + ent.ID + "_" + categoryClass, ent.left + 50, ent.top + 50, paint_box);

                    }
                    resultBitmapQueue.add(bitmap);
                    try (FileOutputStream out_stream = new FileOutputStream(out_filepath)) {
                        bitmap.compress(Bitmap.CompressFormat.PNG, 100, out_stream);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }





               /* if(entities!= null && !entities.isEmpty()){
		            VZEntity ent = entities.get(0);
                     String out_filepath = result_folder_name +count+".png";
                     Bitmap mask = null;
                    Log.d(TAG,"MaskResultWriteTask count : "+count);
                    //out_filepath=(out_filepath.split(".j"))[0]+".png";
                    try{

                        File f=new File("/sdcard/", "test_dumped_image.png");
                        mask=BitmapFactory.decodeStream(new FileInputStream(f));
                    }catch (FileNotFoundException e){
                            e.printStackTrace();
                    }

                    try (FileOutputStream out_stream = new FileOutputStream(out_filepath)) {

                        mask.compress(Bitmap.CompressFormat.PNG, 100, out_stream);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }*/
                publishProgress();
                count++;

            }
                        /*if (sentCount % MAX_BATCH_SIZE == 0 || frameListPrivate.size() <= 0) {
                            millis_startTime = System.currentTimeMillis();
                            try {
                                segmenter.execute(bitmap);
                                *//*if (MAX_BATCH_SIZE == 1)
                                    image_classifier.setVideoFrames(bitmapBytesArray[0], count, bitmap.getWidth(), bitmap.getHeight(), VZGalleryClassifier.COLOR_FORMAT.IMAGE_FORMAT_RGBA8888.getValue());
                                else
                                    image_classifier.setMultipleVideoFrames(bitmapBytesArray, sentCount, bitmap.getWidth(), bitmap.getHeight(), VZGalleryClassifier.COLOR_FORMAT.IMAGE_FORMAT_RGBA8888.getValue());*//*
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                            count += sentCount;
                            //int progressPercent = count * 100 / total24thFrames;
                            *//*pgsBar.setProgress(progressPercent);
                            pgsTxtView.setText(progressPercent + " %");*//*
                            millis_endTime = System.currentTimeMillis();
                            break;
                        }*/
            //}
            totalEngineProcessingTime += (millis_endTime - millis_startTime);
            Log.d(TAG, "Sent " + sentCount + " 24th frames." + " Time taken by engine to store & process the frames = " + (millis_endTime - millis_startTime));

            /*while(mastEntityListPrivate.size()>0){
                VZEntity ent = mastEntityListPrivate.remove();
                String out_filepath = result_folder_name + "/frame"+count+".png";
                Log.d(TAG,"MaskResultWriteTask count : "+count);
                //out_filepath=(out_filepath.split(".j"))[0]+".png";

                try (FileOutputStream out_stream = new FileOutputStream(out_filepath)) {

                    ent.mask.compress(Bitmap.CompressFormat.PNG, 100, out_stream);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                count++;
            }*/
            return null;
        }

        @Override
        protected void onPostExecute(Integer integer) {
            super.onPostExecute(integer);
            isMaskwriteRunning = false;
            HideProgressDialog();
            preapreResultVideo();
           /* progressBar.setVisibility(View.INVISIBLE);
            String msgText = "Process completed.";
            textView.setText(msgText);
            showToast("Video Test Completed");
            Message msg = handle.obtainMessage();
            msg.what = PROCESSING_COMPLETE;
            handle.sendMessage(msg);*/

        }

    }

    private void preapreResultVideo() {
        Log.d(TAG,"preapreResultVideo");
        ShowProgressDialog();
        prepareVideoTask = new PrepareVideoTask();
        prepareVideoTask.execute("");
        isPrepareVideoRunning = true;
    }

    private class PrepareVideoTask extends AsyncTask<String, Integer, Integer>{

        private void showToast(final String message){
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(mContext, message, Toast.LENGTH_SHORT).show();
                }
            });
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
            int progressPercent = values[0] * 100 / count;
            pgsBar.setProgress(progressPercent);
            pgsTxtView.setText("Preparing Video : "+progressPercent + " %");
        }

        @Override
        protected Integer doInBackground(String... strings) {

            //Making the video

            SeekableByteChannel out = null;
            try {
                ArrayList image_filepaths = new ArrayList();
                Utils.list_files(result_folder_name, image_filepaths);
                int n_images = image_filepaths.size();
                Collections.sort(image_filepaths);

                out = NIOUtils.writableFileChannel(result_folder_name+"Track_Video.mp4");
                // for Android use: AndroidSequenceEncoder
                AndroidSequenceEncoder encoder = new AndroidSequenceEncoder(out, Rational.R(25, 1));
                for (int i =0; i < count; i++) {
                    String image_path = result_folder_name+i+".png";
                    File imageFile = new File(image_path);
                    if(imageFile.exists()) {
                        Log.d(TAG,"preapre Video image "+image_path+" existed");
                        Bitmap iamgeBitmap = BitmapFactory.decodeFile(imageFile.getAbsolutePath());
                        // Encode the image
                        encoder.encodeImage(iamgeBitmap);
                    }else{
                        Log.d(TAG,"preapre Video image "+image_path+" not existed");
                    }
                    publishProgress(i);
                }
                // Finalize the encoding, i.e. clear the buffers, write the header, etc.
                encoder.finish();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                NIOUtils.closeQuietly(out);
            }
	    SaveGif();
            return null;
        }

        @Override
        protected void onPostExecute(Integer integer) {
            super.onPostExecute(integer);
            isPrepareVideoRunning = false;

            Message msg = handle.obtainMessage();
            msg.what = PROCESSING_COMPLETE;
            handle.sendMessage(msg);

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    HideProgressDialog();
                    Intent intent = new Intent(getApplicationContext(), VideoPlayActivity.class);
                    intent.putExtra("originalVideoPath",videoUriString);
                    intent.putExtra("trackVideoPath",result_folder_name+"Track_Video.mp4");
                    startActivity(intent);
                    finish();
                }
            });
        }

       private byte[] generateGIF() {
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            AnimatedGifEncoder encoder = new AnimatedGifEncoder();
            encoder.start(bos);
            Bitmap bitmap;
            Log.d(TAG,"generateGIF resultBitmapQueue size:"+resultBitmapQueue.size());
            while (resultBitmapQueue.size() > 0) {
                bitmap = resultBitmapQueue.remove();
                encoder.addFrame(bitmap);
            }
             encoder.finish();
            return bos.toByteArray();
        }

        private void SaveGif() {
            try {
                String gifName = result_folder_name+"test.gif";
                Log.d(TAG,"SaveGif gifName : "+gifName);
                FileOutputStream outStream = new FileOutputStream(gifName);
                outStream.write(generateGIF());
                outStream.close();
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

    }

    private void saveFramesBitmapToImage(Bitmap bitmap,int filename) {

        String out_filepath = frames_folder_name +filename+".png";
        Log.d(TAG,"saveFramesBitmapToImage out_filepath : "+out_filepath);

        FileOutputStream out = null;
        try {
            out = new FileOutputStream(out_filepath);
            bitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
            out.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @SuppressLint("LongLogTag")
    void decodeVideoFrames(Uri selectedMediaUri, String path) throws Exception {
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        retriever.setDataSource(mContext, selectedMediaUri);
        int numOfVideoFrames = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_FRAME_COUNT));
        Log.d(TAG, "number of frames = " + numOfVideoFrames);

        List<Bitmap> frameList = null;
        int startFrameIndex = 0;
        int framesToGet = 0;
        int totalFramesDecoded = 0;
        int chunkSize = MAX_BATCH_SIZE * 1;
        totalAppDecodeTime = 0;
        while (startFrameIndex < numOfVideoFrames) {
            if ((startFrameIndex + chunkSize) >= numOfVideoFrames) {
                framesToGet = numOfVideoFrames - startFrameIndex;
            } else {
                framesToGet = chunkSize;
            }
            long millis_startTime = System.currentTimeMillis();
            frameList = retriever.getFramesAtIndex(startFrameIndex, framesToGet);
            long millis_endTime = System.currentTimeMillis();
            totalAppDecodeTime += (millis_endTime - millis_startTime);
            Log.d(TAG, "Time taken by app to decode " + frameList.size() + " frames [" + startFrameIndex + " to " + (startFrameIndex + framesToGet - 1) + "] = " + (millis_endTime - millis_startTime));
            for (int i = 0; i < frameList.size(); i += 1) {
                frameListPrivate.add(frameList.get(i));
                saveFramesBitmapToImage(frameList.get(i),totalFramesDecoded);
                ++totalFramesDecoded;
            }

            Message msg = handle.obtainMessage();
            msg.what = SEND_FRAMES_TO_ENGINE;
            handle.sendMessage(msg);

            startFrameIndex += chunkSize;
        }

        Log.d(TAG, "current Que size = " + frameListPrivate.size() + " total time taken to decode " + totalFramesDecoded + " frames = " + totalAppDecodeTime);
        retriever.release();

        return;
    }

}