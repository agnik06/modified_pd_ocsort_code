package vizinsight.atl.videoautofocus;

import android.Manifest;
import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.TextureView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

import vizinsight.atl.object_detector.VZEntity;
import vizinsight.atl.object_detector.VZObjectDetector;

import static android.Manifest.permission.WRITE_EXTERNAL_STORAGE;

public class CameraActivity extends CoreActivity {
    TextureView mTextureView;
    ImageView mMaskView;
    //ImageButton mProcessButton;
    //TextView mDisplayTextView;
    enum ButtonState{
        Hold,
        Release,
        Double_Tap,
        Click,
        None
    }
    ButtonState processButtonState = ButtonState.Double_Tap;
    GestureDetector gestureDetector;

    CameraHandler mCameraHandler;
    CameraHandler.PreviewUpdateListener mPreivewPreviewUpdateListener;
    CameraHandler.ImageCaptureListener mImageCaptureListener;

    SegmentResultHandler mResultHandler;
    ImageView mTrackIamgeView;


    private static final int MULTIPLE_PERMISSIONS = 1;
    private static String[] PERMISSIONS_LIST = {
            Manifest.permission.CAMERA,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            WRITE_EXTERNAL_STORAGE
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
//        odMode = VZObjectDetector.MODE_SALIENCY;
        Utils.verifyPermissions(this, PERMISSIONS_LIST, MULTIPLE_PERMISSIONS);

        invalidateOptionsMenu();

        super.onCreate(savedInstanceState);
        super.setInfo("Click capture button to get results \n\n" +
                "Hold capture button to find the results on preview \n\n" +
                "Double click the button to get continuous preview"
        );

        setContentView(R.layout.activity_camera);
//        setTitle("                  Object Detector");
        setTitle("");



        mTextureView = findViewById(R.id.texture_view_camera);
        mMaskView = findViewById(R.id.mask_view_camera);
        //mProcessButton = findViewById(R.id.process_button);
        //mDisplayTextView = findViewById(R.id.camera_display_text);
        mTrackIamgeView = findViewById(R.id.track_imageView);


        configureCameraHandler();
        configureResultHandler();
        configureCaptureButton();
    }

    public void onBackPressed() {
        clearDisplayedEntities();
        processButtonState = ButtonState.Release;
        super.onBackPressed();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }

    @SuppressLint("ClickableViewAccessibility")
    private void configureCaptureButton() {
        gestureDetector = new GestureDetector(this, new GestureDetector.SimpleOnGestureListener(){
            @Override
            public void onLongPress(MotionEvent e) {
                mCameraHandler.caputureImage();

                //mDisplayTextView.setText("Double tap to start/stop");
                processButtonState = ButtonState.Double_Tap;
                clearDisplayedEntities();
                return ;
            }
        });

        gestureDetector.setOnDoubleTapListener(new GestureDetector.OnDoubleTapListener() {
            @Override
            public boolean onSingleTapConfirmed(MotionEvent e) {
                //mDisplayTextView.setText("Double tap to start/stop");
                return true;
            }

            @Override
            public boolean onDoubleTap(MotionEvent e) {
                if(processButtonState != ButtonState.Double_Tap) {
                    processButtonState = ButtonState.Double_Tap;
                }
                else{
                    clearDisplayedEntities();
                    processButtonState = ButtonState.Release;
                }
                return true;
            }

            @Override
            public boolean onDoubleTapEvent(MotionEvent e) {
                return false;
            }
        });

        /*mProcessButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(gestureDetector.onTouchEvent(motionEvent)){
                    return true;
                }

                int action = motionEvent.getAction();
                if(action == MotionEvent.ACTION_DOWN){
//                    processButtonState = ButtonState.Hold;
                }
                else if (action == MotionEvent.ACTION_UP){
                    if(processButtonState == ButtonState.Hold){
                        processButtonState = ButtonState.Release;
                        clearDisplayedEntities();
                    }
                }
                return true;
            }
        });*/
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        boolean itemSelectStatus = super.onOptionsItemSelected(item);
        if(!itemSelectStatus) return false;
        configureResultHandler();
        return true;
    }

    private void configureResultHandler() {
        SegmentResultHandler.DisplayMode display_mode = SegmentResultHandler.DisplayMode.DIALOG_VIEW_MODE;
        mResultHandler = new SegmentResultHandler.Builder(this)
                .setMode(display_mode)
                .setSegmenter(object_detector)
                .build();
    }

    private void configureCameraHandler() {
        mCameraHandler = new CameraHandler(this, mTextureView);
        int frameCount = 0;

        mPreivewPreviewUpdateListener = new CameraHandler.PreviewUpdateListener() {
            @Override
            public void onPreviewUpdate(byte[] nv21_bytes, int width, int height, int rotationDegrees) {
                if(object_detector != null){
                    if(processButtonState == ButtonState.Hold || processButtonState == ButtonState.Double_Tap){
                        long startTime = System.currentTimeMillis();
                        List<VZEntity> entities = object_detector.execute(nv21_bytes, width,
                                height, (360-rotationDegrees)%360, VZObjectDetector.IMAGEFORMAT_NV21);
                        Log.e("GOD_VZ Debug","(Java) Time taken for execute+extract: "+(System.currentTimeMillis()-startTime));

                        startTime = System.currentTimeMillis();
                        displayEntities(nv21_bytes, width, height, rotationDegrees, entities);
                        Log.e("GOD_VZ Debug","(Java) Time taken for displaying: "+(System.currentTimeMillis()-startTime));
                    }
                }
            }
        };

        mImageCaptureListener = new CameraHandler.ImageCaptureListener() {
            @Override
            public void onCapture(byte[] rgba_bytes, int width, int height, int rotationDegrees) {
                if(object_detector != null){
                    mResultHandler.generate(rgba_bytes, width, height, rotationDegrees, VZObjectDetector.IMAGEFORMAT_RGBA);
//                    List<VZEntity> entities = object_detector.execute(rgba_bytes, image_width,
//                            image_height, rotationDegrees, VZObjectDetector.IMAGEFORMAT_RGBA);
//                    displayEntities(entities);
                }
            }
        };
        mCameraHandler.setPreviewUpdateListener(mPreivewPreviewUpdateListener);
        mCameraHandler.setImageCaptureListener(mImageCaptureListener);
    }

    private void displayEntities(byte[] nv21_bytes, int width, int height, int rotation, List<VZEntity> entities) {
        if(entities == null || entities.isEmpty()){
            clearDisplayedEntities();
            return;
        }
        Bitmap image = Utils.YUV2Bitmap(nv21_bytes, width, height, rotation);
        Matrix matrix=new Matrix();
        matrix.preScale(1.0f,1.0f);

        //Draw Box
        String s = "#007FFF"; //Bounding box color selection
        int color = Color.parseColor(s);

        Paint paint_box = new Paint();
        paint_box.setStyle(Paint.Style.STROKE);
        paint_box.setAntiAlias(true);
        paint_box.setColor(color); //Color.BLUE
        paint_box.setStrokeWidth(width/320.0f);

        Paint paint_text = new Paint();
        paint_text.setStyle(Paint.Style.FILL_AND_STROKE);
        paint_text.setAntiAlias(true);
        paint_text.setColor(Color.WHITE);
        paint_text.setTextSize(width/25);

        Paint paint_text_box = new Paint(paint_box);
        paint_text_box.setStyle(Paint.Style.FILL);
        paint_text_box.setTextSize(width/22);

        image= Bitmap.createBitmap(image,0,0,image.getWidth(),image.getHeight(),matrix,true);
        Canvas canvas = new Canvas(image);
        for(VZEntity e: entities){
            if(e.tag.equals("object")){
                continue;
            }
            //VZEntity entity = new VZEntity(e.tag, e.image_width, e.image_height, e.image_width - e.right, e.top, e.image_width - e.left, e.bottom);
            VZEntity entity = new VZEntity(e.tag, e.tagId, e.ID, e.left, e.top, e.right, e.bottom,e.score,-1);

            if(entity.tag.contains("et")){
                paint_text.setColor(Color.RED);
                paint_text_box.setColor(Color.WHITE);
                paint_box.setColor(Color.RED);
            }
            else{
                paint_text.setColor(Color.BLUE);
                paint_text_box.setColor(Color.TRANSPARENT);
                paint_box.setColor(Color.BLUE);
            }
            String categoryClass = " ";
            if (entity.tagId == 5) {
                categoryClass = "F";
            } else if (entity.tagId == 8) {
                categoryClass = "P";
            }
            else if (entity.tagId == 6) {
                categoryClass = "A";
            }
            canvas.drawRect(entity.left, entity.top, entity.right, entity.bottom, paint_box);

            Rect text_bounds = new Rect();
            paint_text_box.getTextBounds("#" + entity.ID + "_" + categoryClass, 0, ("#" + entity.ID + "_" + categoryClass).length(), text_bounds);

            canvas.drawRect(entity.left , entity.top, entity.left + text_bounds.width(), entity.top - text_bounds.height(), paint_text_box);
            int tb_w = 0;//text_bounds.width();
            int tb_h = 0;//text_bounds.height();
            //canvas.drawText("#" + entity.ID + "_" + categoryClass, entity.left + 50, entity.top + 50, paint_box);
            canvas.drawText("#" + entity.ID + "_" + categoryClass, entity.left + tb_w / 20f, entity.top - tb_h/5f, paint_text);
            /*Rect text_bounds = new Rect();
            canvas.drawRect(entity.left, entity.top, entity.right, entity.bottom, paint_box);
            paint_text_box.getTextBounds("#" + entity.ID + "_" + categoryClass, 0, ("#" + entity.ID + "_" + categoryClass).length(), text_bounds);
            canvas.drawRect(entity.left + 50, entity.top + 50, entity.left + 50 + text_bounds.width(), entity.top + 50 - text_bounds.height(), paint_text_box);

            canvas.drawText("#" + entity.ID + "_" + categoryClass, entity.left + 50, entity.top + 50, paint_box);*/
        }

//        mDisplayTextView.setText(entities.get(0).tag);
        //mMaskView.setImageBitmap(image);
        mTrackIamgeView.setImageBitmap(image);

    }

    private void clearDisplayedEntities() {
//        mDisplayTextView.setText("");
        //mMaskView.setImageResource(android.R.color.transparent);
        mTrackIamgeView.setImageResource(android.R.color.transparent);
    }
}
