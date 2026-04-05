package vizinsight.atl.videoautofocus;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.OutOfMemoryError;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import lib.folderpicker.FolderPicker;
import vizinsight.atl.object_detector.VZEntity;

public class BatchTestActivity extends CoreActivity {
    Context mContext;
    boolean ENABLE_BOX=false;
    TextView directoryText;
    ImageButton select_button;
    ImageButton play_stop_button;
    ImageView progress_image;
    TextView progress_text;
    String image_folder = null;
    boolean isTestRunning = false;
    TestRunner test_runner;
    final int IMAGEFORMAT_NV21 = 0;
    final int IMAGEFORMAT_BGR  = 1;
    final int IMAGEFORMAT_RGBA = 2;
    String Mode;

    private String TAG = "UOD_BatchTestActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        defaultModelPath="/vendor/etc/saiv/image_understanding/db/";
        super.onCreate(savedInstanceState);
        super.setInfo("Select the folder with images\n\n" +
                "After processing the results are generated in '/sdcard/results/' directory\n\n" +
                "Batch Testing is not supported in Live-Focus mode");

        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_FULLSCREEN;
        decorView.setSystemUiVisibility(uiOptions);
        setContentView(R.layout.activity_batch_test);
        setTitle("Batch Test");
        Mode = getIntent().getStringExtra("mode");
        mContext = this.getApplicationContext();

        directoryText       = findViewById(R.id.directory_text);
        play_stop_button    = findViewById(R.id.play_stop_folder);
        progress_image      = findViewById(R.id.progress_image);
        progress_text       = findViewById(R.id.progress_text);

        select_button = findViewById(R.id.select_folder_button);
        select_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                select_folder();
            }
        });

        play_stop_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(isTestRunning){
                    Toast.makeText(mContext, "Testing stopped", Toast.LENGTH_SHORT).show();
                    stopTesting();
                }
                else{
                    startTesting();
                }
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        if(true){
            return true;
        }
        boolean val = super.onCreateOptionsMenu(menu);
        menu.findItem(R.id.style_select).setEnabled(false);
        return val;
    }

    private void startTesting() {
        test_runner = new TestRunner();
        test_runner.execute(image_folder);
        play_stop_button.setImageResource(R.drawable.stop);
        select_button.setAlpha(0.5f);
        select_button.setClickable(false);
        isTestRunning = true;
    }

    private void stopTesting() {
        if(test_runner!= null && !test_runner.isCancelled()){
            test_runner.cancel(true);
        }
        clearProgress();
        play_stop_button.setImageResource(R.drawable.play);
        select_button.setAlpha(1f);
        select_button.setClickable(true);
        isTestRunning = false;
    }
    private void clearProgress(){
        progress_image.setImageResource(android.R.color.transparent);
        progress_text.setText("");

    }
    int SELECT_FOLDER_REQUEST_CODE = 0;
    void select_folder(){
        Intent intent = new Intent(this, FolderPicker.class);
        startActivityForResult(intent, SELECT_FOLDER_REQUEST_CODE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == SELECT_FOLDER_REQUEST_CODE && resultCode != 0){
            image_folder = data.getExtras().getString("data");
            directoryText.setText(Html.fromHtml("Image Directory: <u>"+image_folder+"</u>"));
            play_stop_button.setVisibility(View.VISIBLE);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        boolean itemSelectStatus = super.onOptionsItemSelected(item);
        if(!itemSelectStatus) return false;
        switch(item.getItemId()){
            case R.id.mode_live_focus:
                Toast.makeText(this, "Live-Focus Mode not supported in Batch Testing", Toast.LENGTH_LONG).show();
                return true;
            default:
                if(isTestRunning){
                    stopTesting();
                }
                clearProgress();
                return true;
        }
    }

    private class TestRunner extends AsyncTask<String, Integer, Integer>{
        Bitmap progressbar_bitmap_full;
        Bitmap progressbar_bitmap_boundary;
        Bitmap progressbar_bitmap;
        final int progressbar_size = 500;
        int progress = 0;
        int previous_progress = 0;
        String result_folder_name = "";

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            progressbar_bitmap_full = BitmapFactory.decodeResource(getResources(), R.drawable.mask_on_logo);
            progressbar_bitmap_full = progressbar_bitmap_full.createScaledBitmap(progressbar_bitmap_full, progressbar_size, progressbar_size, false);
            progressbar_bitmap = Bitmap.createBitmap(progressbar_size, progressbar_size, Bitmap.Config.ARGB_8888);
            progressbar_bitmap_boundary = BitmapFactory.decodeResource(getResources(), R.drawable.mask_on_boundary);
            progressbar_bitmap_boundary = progressbar_bitmap_boundary.createScaledBitmap(progressbar_bitmap_boundary, progressbar_size, progressbar_size, false);

        }

        private void update_progress_text(final String image_path, final int abs_progress, final int total_count){
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(isTestRunning) {
                        progress_text.setText("(" + abs_progress + "/" + total_count + "): " /*+ image_path*/);
                    }
                }
            });
        }

        private void showToast(final String message){
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(mContext, message, Toast.LENGTH_SHORT).show();
                }
            });
        }

        @Override
        protected Integer doInBackground(String... strings) {
            if(object_detector == null || !object_detector.isValid){
                return null;
            }
            String image_folder_name = strings[0];
            Log.d(TAG,"doInBackground image_folder_name : "+image_folder_name);

            // Make result folder
            result_folder_name = get_results_folder_name(image_folder_name);
            Log.d(TAG,"doInBackground : result_folder_name : "+result_folder_name);
            File dir = new File(result_folder_name);
            if(dir.exists()){
                dir.delete();
            }
            dir.mkdirs();
            showToast("Results will be stored in \""+result_folder_name+"\" directory");

            // List images in the folder
            ArrayList image_filepaths = new ArrayList();
            Utils.list_files(image_folder_name, image_filepaths);
            int n_images = image_filepaths.size();
            //Collections.sort(image_filepaths);

            Log.v(TAG, "Starting test on "+n_images+" images");
            Log.d(TAG,"doInBackground Mode : "+Mode);

            // Execute on images
            for(int i = 0; i < n_images && !isCancelled(); i++){
                String image_path;
                if(Mode.equals("Image")){
                    image_path = image_filepaths.get(i).toString();
                    //image_path = image_folder_name + "/" + i + ".png";
                }else {
                    image_path = image_folder_name + "/" + i + ".png";
                }
                Log.d(TAG, "( " + i + " /  " + n_images + "  ) " + image_path );
                if(!Utils.isValidImagePath(image_path)){
                    continue;
                }
                List<VZEntity> entities = null;
                try{
                    entities = object_detector.execute(image_path,IMAGEFORMAT_RGBA,Mode);

                }catch(OutOfMemoryError e){
                    showToast(image_path+" size is large. So ignoring the image to proess");
                }
                int width;
                // Save results in results folder
                HashMap<String,Integer> classes=new HashMap<String, Integer>();
                classes.put("object",1);
                classes.put("person",2);
                classes.put("petface",8);
                classes.put("pets",10);
                classes.put("peteyes",14);
                
                String out_filepath = image_path.replace(image_folder_name, result_folder_name);
                String out_filepath_txt ;
                if(out_filepath.contains(".JPEG")){
                    out_filepath_txt = (out_filepath.split("[.]JP"))[0] + ".txt";
                }
                else if(out_filepath.contains(".jpg")){
                    out_filepath_txt = (out_filepath.split("[.]jp"))[0] + ".txt";
                }
                else if(out_filepath.contains(".jpeg")){
                    out_filepath_txt = (out_filepath.split("[.]jp"))[0] + ".txt";
                }
                else if(out_filepath.contains(".png")){
                    out_filepath_txt = (out_filepath.split("[.]pn"))[0] + ".txt";
                }
                else if(out_filepath.contains(".PNG")){
                    out_filepath_txt = (out_filepath.split("[.]PN"))[0] + ".txt";
                }
                else if(out_filepath.contains(".bmp")){
                    out_filepath_txt = (out_filepath.split("[.]bm"))[0] + ".txt";
                }
                else if(out_filepath.contains(".BMP")){
                    out_filepath_txt = (out_filepath.split("[.]BM"))[0] + ".txt";
                }
                else{
                    out_filepath_txt=out_filepath+".txt";
                }
                String txt = "";

                if(!(entities== null) && !entities.isEmpty()) {
                    Bitmap bm = BitmapFactory.decodeFile(image_path);
                    Bitmap temp_mask;


                    bm = Utils.rotateBitmap(bm, Utils.getImageOrientation(image_path));
                    temp_mask = bm.copy(Bitmap.Config.ARGB_8888,true);
                    width=bm.getWidth();
                    Paint p = new Paint();
                    p.setStyle(Paint.Style.STROKE);
                    p.setAntiAlias(true);
                    p.setFilterBitmap(true);
                    p.setDither(true);
                    p.setColor(Color.BLUE);
                    p.setTextSize(width/25);

                    Rect text_bounds = new Rect();
                    Paint paint_box = new Paint();
                    paint_box.setStyle(Paint.Style.FILL);
                    paint_box.setAntiAlias(true);
                    paint_box.setColor(Color.WHITE); //Color.BLUE
                    paint_box.setStrokeWidth(width/320.0f);
                    Paint paint_text_box = new Paint(paint_box);
                    paint_text_box.setStyle(Paint.Style.FILL);
                    paint_text_box.setTextSize(width/25);
                    paint_box.setTextSize(width/25);

                    Canvas c = new Canvas(temp_mask);

                    for (int idx = 0; idx < entities.size(); idx++) {
                        VZEntity ent = entities.get(idx);
                        if(ent.tag.contains("et")){
                            p.setColor(Color.RED);
                            paint_text_box.setColor(Color.WHITE);
                            paint_box.setColor(Color.RED);
                        }
                        else{
                            p.setColor(Color.BLUE);
                            paint_text_box.setColor(Color.TRANSPARENT);
                            paint_box.setColor(Color.BLUE);
                        }
                        /*if(Mode.equals("Video")) {
                            Log.d(TAG,"Drawing the box for video tracking.");
                            String categoryClass = " ";
                            if (ent.tagId == 5) {
                                categoryClass = "Flower";
                            } else if (ent.tagId == 8) {
                                categoryClass = "Petface";
                            } else if (ent.tagId == 10) {
                                categoryClass = "Pet";
                            }
                            c.drawRect(ent.left, ent.top, ent.right, ent.bottom, p);
                            paint_text_box.getTextBounds("#" + ent.ID + "_" + categoryClass, 0, ("#" + ent.ID + "_" + categoryClass).length(), text_bounds);
                            c.drawRect(ent.left + 50, ent.top + 50, ent.left + 50 + text_bounds.width(), ent.top + 50 - text_bounds.height(), paint_text_box);

                            c.drawText("#" + ent.ID + "_" + categoryClass, ent.left + 50, ent.top + 50, paint_box);
                            txt = txt + ent.ID + " ";
                            txt = txt + categoryClass + " ";
                            txt = txt + ent.score + " ";
                            txt = txt + ent.left + " ";
                            txt = txt + ent.top + " ";
                            txt = txt + ent.right + " ";
                            txt = txt + ent.bottom + "\n";
                        }else{*/
                            Log.d(TAG,"Drawing the box for Images.");
                            c.drawRect(ent.left, ent.top, ent.right, ent.bottom, p);
                            paint_text_box.getTextBounds(ent.trackId+"_"+ent.tag+"_"+(int)(ent.score*100), 0, (ent.associationId+"_"+ent.tag+"_"+(int)(ent.score*100)).length(), text_bounds);
                            c.drawRect(ent.left, ent.top, ent.left + text_bounds.width(), ent.top - text_bounds.height(), paint_text_box);

                            c.drawText(ent.trackId+"_"+ent.tag + "_" + (int)(ent.score*100), ent.left, ent.top, paint_box);
                            txt = txt + ent.trackId + " ";
                            txt = txt + classes.get(ent.tag) + " ";
                            txt = txt + ent.score + " ";
                            txt = txt + ent.left + " ";
                            txt = txt + ent.top + " ";
                            txt = txt + ent.right+ " ";
                            txt = txt + ent.bottom + "\n";
                        //}
                    }



                    try (FileOutputStream out_stream = new FileOutputStream(out_filepath)) {
                        temp_mask.compress(Bitmap.CompressFormat.PNG, 100, out_stream);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                try (FileOutputStream out_stream = new FileOutputStream(out_filepath_txt)) {
                    out_stream.write(txt.getBytes());
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                // Update progress
                progress++;
                publishProgress(progress * 100 / n_images);
                update_progress_text(image_path.split(image_folder_name+"/")[1], progress, n_images);
            }
            return null;
        }

        private String get_results_folder_name(String folder_path) {
//            return folder_path+"_results";
            String result_folder_path;
            if(Mode.equals("Video")) {
                result_folder_path = "/sdcard/results_UOD/Video";
            }else {
                result_folder_path = "/sdcard/results_UOD/Image";
            }
            result_folder_path += folder_path.split("/storage/emulated/0/")[1];

            /*SimpleDateFormat sdf1 = new SimpleDateFormat("yyyy.MM.dd.HH.mm.ss");
            Timestamp timestamp = new Timestamp(System.currentTimeMillis());
            String currentTimeStamp = sdf1.format(timestamp);

            result_folder_path += "_"+currentTimeStamp;*/

            return result_folder_path;
        }

        @Override
        protected void onPostExecute(Integer integer) {
            super.onPostExecute(integer);
            stopTesting();
            isTestRunning = false;
            if(progress != 0) {
                Toast.makeText(mContext, "Test Completed", Toast.LENGTH_SHORT).show();
                progress_image.setImageBitmap(progressbar_bitmap_full);
            }
            progress_text.setText("Results are stored in \""+result_folder_name+"\"");
            object_detector.resetStartTracking();
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
//            int pro = progress*progressbar_size/100;
            int pro = values[0] * progressbar_size / 100;
            int px[] = new int[progressbar_size *(pro - previous_progress)];
            progressbar_bitmap_full.getPixels(px,0, progressbar_size, 0, progressbar_size -pro, progressbar_size, pro- previous_progress);
            progressbar_bitmap.setPixels(px, 0, progressbar_size, 0, progressbar_size -pro, progressbar_size, pro- previous_progress);
            progressbar_bitmap = Utils.overlay(progressbar_bitmap_boundary, progressbar_bitmap);
            progress_image.setImageBitmap(progressbar_bitmap);
            previous_progress = pro;
        }
    }

}
