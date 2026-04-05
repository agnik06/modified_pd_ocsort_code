package vizinsight.atl.videoautofocus;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import androidx.appcompat.app.AppCompatActivity;

import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.Window;
import android.widget.ImageButton;

import static android.Manifest.permission.WRITE_EXTERNAL_STORAGE;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    Context context;
    private static final int MULTIPLE_PERMISSIONS = 1;
    private Uri selectedMediaUriPrivate;
    private static String[] PERMISSIONS_LIST = {
            Manifest.permission.CAMERA,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            WRITE_EXTERNAL_STORAGE
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        //getSupportActionBar().hide();
        setContentView(R.layout.activity_main);

        this.context = this.getApplicationContext();
        Utils.verifyPermissions(this, PERMISSIONS_LIST, MULTIPLE_PERMISSIONS);

        ImageButton camera_button = findViewById(R.id.camera_select_button);
        camera_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                Toast.makeText(context, "Not yet implemented", Toast.LENGTH_SHORT).show();
                start_camera_activity();
            }
        });

        ImageButton folder_select_button = findViewById(R.id.folder_select_button);
        folder_select_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                start_folder_select_activity();
            }
        });

        ImageButton video_folder_select_button = findViewById(R.id.scene_select_button);
        video_folder_select_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                start_batch_test_activity("Video");
            }
        });

        ImageButton image_folder_select_button = findViewById(R.id.image_batch_select_button);
        image_folder_select_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                start_batch_test_activity("Image");
            }
        });

        /*ImageButton night_mode_btn = findViewById(R.id.nightmode_button);
        night_mode_btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                Log.d("MainActivity:","NightMode Click");
                start_night_mode_activity();
            }
        });*/
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        String result_folder_name = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_results/";
        File dir = new File(result_folder_name);
        if(dir.exists()) {
            deleteDirectory(dir);
        }
        String frames_folder_name = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_Frames/";
        File frameDir = new File(frames_folder_name);
        if(frameDir.exists()) {
            deleteDirectory(frameDir);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        String result_folder_name = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_results/";
        File dir = new File(result_folder_name);
        if(dir.exists()) {
            deleteDirectory(dir);
        }
        String frames_folder_name = Environment.getExternalStorageDirectory().getAbsolutePath()+"/LOD_Video_Frames/";
        File frameDir = new File(frames_folder_name);
        if(frameDir.exists()) {
            deleteDirectory(frameDir);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        //Camera
        /*if (requestCode == RC_TAKE_PHOTO && resultCode == RESULT_OK) {
//            Log.e("Camera", data.getData()+"");
//            classify_camera_img(Utils.getFilePath(this, mCapturedImageURI));
            Log.e("Camera path", mCapturedImagePath);
            startActivityForSegments(mCapturedImagePath);
        }*/

        //Gallery
        if(requestCode == SELECT_FILE && resultCode == RESULT_OK){
            if(data!=null){
                selectedMediaUriPrivate = data.getData();
                startActivityForVideoSegments(selectedMediaUriPrivate);
            }
        }
    }

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

    final int SELECT_FILE = 2;
    public void start_folder_select_activity(){
        /*Intent intent = new Intent(this, BatchTestActivity.class);
        startActivity(intent);*/
        Intent pickIntent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        pickIntent.setType("video/*");
        startActivityForResult(pickIntent, SELECT_FILE);
    }

    public void start_batch_test_activity(String mode){
        Intent intent = new Intent(this, BatchTestActivity.class);
        intent.putExtra("mode",mode);
        startActivity(intent);
        /*Intent pickIntent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        pickIntent.setType("video/*");
        startActivityForResult(pickIntent, SELECT_FILE);*/
    }

    public void start_camera_activity(){
        Intent intent = new Intent(this, CameraActivity.class);
        startActivity(intent);
    }

    public void start_night_mode_activity(){
        Intent intent = new Intent(this, NightModeActivity.class);
        startActivity(intent);
    }

    private void startActivityForVideoSegments(Uri videoUri) {
        Intent intent = new Intent(this, VideoPlayActivityDummy.class);
        intent.putExtra("videoUri", videoUri.toString());
        startActivity(intent);
    }

}
