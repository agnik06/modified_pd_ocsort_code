package vizinsight.atl.object_detector;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.util.Log;
import android.widget.Toast;

import java.lang.OutOfMemoryError;
import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.List;

public class VZObjectDetector {
    static {
        System.loadLibrary("PetDetectorJNI");
    }
    private native boolean initializeJNI(int mode, String model_base_path);
    private native boolean executeJNI(ByteBuffer bitmap_buffer, int width, int height, int angular_rotation, int color_format, int mode);
    private native String  getBoxInfosJNI(int mode);
    private native String executeStartTackingJNI(ByteBuffer bitmap_buffer, int width, int height, int angular_rotation, int orieneation, int color_format, int mode, int touchX, int touchY);
    private native String executeKeepTrackingJNI(ByteBuffer bitmap_buffer, int angular_rotation,int mode);
    private native boolean executeWithTrackJNI(ByteBuffer bitmap_buffer, int width, int height, int angular_rotation, int color_format, int mode);
    private native void    releaseJNI(int mode);
    private long engine_reference;
    private long system_engine_reference;

    private final String TAG = "VZObjectDetector";

    public static final int MODE_SALIENCY   = 0;
    public static final int MODE_SALIENCY_SYSTEM   = 1;

    public static final int IMAGEFORMAT_NV21 = 0;
    public static final int IMAGEFORMAT_BGR  = 1;
    public static final int IMAGEFORMAT_RGBA = 2;
    public static final int IMAGEFORMAT_NV12 = 3;

    public boolean isValid = false;
    private int mode;
    private Boolean callStartTracking = true;
    public VZObjectDetector(int mode, String model_base_path) {
        this.setVZStatusChangeListener(new VZObjectDetector.VZStatusChangeListener() {
            @Override
            public void onInitComplete(boolean isSuccess) {
                final boolean is_success = isSuccess;

                if (is_success) {
                    Log.v("VZ Debug","Initialization of aic_l_o_detector models completed");
                } else
                    Log.v("VZ Debug","Initialization of aic_l_o_detector models failed");

            }});
        isValid = initializeJNI(mode, model_base_path);
        this.mode = mode;
        if(!isValid) {
            Log.e("VZ Debug", "Initialization of ObjectDetector failed");
        }
    }
    public int getMode() {
        return this.mode;
    }

    private VZStatusChangeListener statusChangeListener = null;

    public void resetStartTracking() {
        callStartTracking = true;
    }

    public interface VZStatusChangeListener {
        void onInitComplete(boolean isSuccess);
    }
    public void setVZStatusChangeListener(VZStatusChangeListener listener){
        this.statusChangeListener = listener;
    }

    private void callback(int status){ //called from jni
        int a=1;
        if(statusChangeListener != null)
            statusChangeListener.onInitComplete(status==1);
    }

    private ByteBuffer image_byte_buffer;
    public List<VZEntity> execute(byte[] bitmap_buffer, int width, int height, int angular_rotation, int color_format) {
        if(!isValid){
            return null;
        }
        image_byte_buffer = ByteBuffer.allocateDirect(bitmap_buffer.length);
        image_byte_buffer.put(bitmap_buffer);
        boolean execute_status = executeJNI(image_byte_buffer, width, height, angular_rotation, color_format, mode);
        //boolean execute_status = executeWithTrackJNI(image_byte_buffer, width, height, angular_rotation, color_format, mode);
        image_byte_buffer.clear();
        List<VZEntity> entities= getBoxEntities(execute_status);
        if(!(entities==null)) {
            for (int i = 0; i < entities.size(); i++) {
                Rect r_temp = new Rect(entities.get(i).left, entities.get(i).top, entities.get(i).right, entities.get(i).bottom);
                r_temp = Utils.rotation_corrected_roi(r_temp, width, height, angular_rotation);
                entities.get(i).left = r_temp.left;
                entities.get(i).top = r_temp.top;
                entities.get(i).right = r_temp.right;
                entities.get(i).bottom = r_temp.bottom;
            }
        }
        return entities;
    }

    public List<VZEntity> executeStartTracking(byte[] bitmap_buffer, int width, int height, int angular_rotation, int orientation, int color_format, int touchX, int touchY) {
        if(!isValid){
            return null;
        }
        image_byte_buffer = ByteBuffer.allocateDirect(bitmap_buffer.length);
        image_byte_buffer.put(bitmap_buffer);
        String track_info_json_str = executeStartTackingJNI(image_byte_buffer, width, height, angular_rotation, orientation, color_format, mode, touchX, touchY);
        image_byte_buffer.clear();

        Log.d(TAG,"executeStartTracking track_info_json_str : "+track_info_json_str);
        if(track_info_json_str.isEmpty()){
            return null;
        }
        List<VZEntity> result_entities = Utils.convert_result_info_to_track_entities(track_info_json_str );
        callStartTracking = false;

        return result_entities;
/*
        if(!(entities==null)) {
            for (int i = 0; i < entities.size(); i++) {
                Rect r_temp = new Rect(entities.get(i).left, entities.get(i).top, entities.get(i).right, entities.get(i).bottom);
                r_temp = Utils.rotation_corrected_roi(r_temp, width, height, angular_rotation);
                entities.get(i).left = r_temp.left;
                entities.get(i).top = r_temp.top;
                entities.get(i).right = r_temp.right;
                entities.get(i).bottom = r_temp.bottom;
            }
        }
        return entities;*/
    }

    public List<VZEntity> executeKeepTracking(byte[] bitmap_buffer, int width, int height , int angular_rotation) {
        if(!isValid){
            return null;
        }
        image_byte_buffer = ByteBuffer.allocateDirect(bitmap_buffer.length);
        image_byte_buffer.put(bitmap_buffer);
        String track_info_json_str = executeKeepTrackingJNI(image_byte_buffer, angular_rotation,mode);
        image_byte_buffer.clear();

        if(track_info_json_str.isEmpty()){
            return null;
        }
        List<VZEntity> result_entities = Utils.convert_result_info_to_track_entities(track_info_json_str );

        return result_entities;
        /*List<VZEntity> entities= getBoxEntities(execute_status);
        if(!(entities==null)) {
            for (int i = 0; i < entities.size(); i++) {
                Rect r_temp = new Rect(entities.get(i).left, entities.get(i).top, entities.get(i).right, entities.get(i).bottom);
                r_temp = Utils.rotation_corrected_roi(r_temp, width, height, angular_rotation);
                entities.get(i).left = r_temp.left;
                entities.get(i).top = r_temp.top;
                entities.get(i).right = r_temp.right;
                entities.get(i).bottom = r_temp.bottom;
            }
        }
        return entities;*/
    }
    public List<VZEntity> execute(String image_path) throws OutOfMemoryError{
        Bitmap image = Utils.get_exif_corrected_bitmap(image_path);

        byte[] bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_RGBA8888);

        return execute(bitmap_bytes, image.getWidth(), image.getHeight(), 0, IMAGEFORMAT_RGBA);
    }

    public List<VZEntity> execute(String image_path, int color_format, String Mode) throws OutOfMemoryError{
        Bitmap image = Utils.get_exif_corrected_bitmap(image_path);
        if(image == null){
            Log.d(TAG,"execute(String image_path, int color_format) Bitmap is null");
            return null;
        }
        byte[] bitmap_bytes;
        switch (color_format){
            case VZImageDecoder.IMAGE_FORMAT_YUV420:
                bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_YUV420);
            break;
            default:
                color_format = IMAGEFORMAT_RGBA;
                bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_RGBA8888);
        }
        /*if(Mode.equals("Video")) {
            Log.d(TAG,"execute video Batch test");
            String fileName = image_path.substring(image_path.lastIndexOf("/") + 1);
            Log.d(TAG, "execute fileName : " + fileName + " callStartTracking : " + callStartTracking);
            if (callStartTracking) {
                return executeStartTracking(bitmap_bytes, image.getWidth(), image.getHeight(), 0, Utils.getImgeOrientation(image_path), color_format, -1, -1);
            } else {
                return executeKeepTracking(bitmap_bytes, image.getWidth(), image.getHeight(), 0);
            }
        }else{*/
            Log.d(TAG,"execute Image Batch test");
            return execute(bitmap_bytes, image.getWidth(), image.getHeight(), 0, color_format);
        //}
    }
    
    public List<VZEntity> execute(Bitmap image, int color_format) throws OutOfMemoryError{
        //Bitmap image = Utils.get_exif_corrected_bitmap(image_path);
        if(image == null){
            Log.d(TAG,"execute(String image_path, int color_format) Bitmap is null");
            return null;
        }
        byte[] bitmap_bytes;
        switch (color_format){
            case VZImageDecoder.IMAGE_FORMAT_YUV420:
                bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_YUV420);
                break;
            default:
                color_format = IMAGEFORMAT_RGBA;
                bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_RGBA8888);
        }
        //String fileName = image_path.substring(image_path.lastIndexOf("/")+1);
        Log.d(TAG,"execute callStartTracking : "+callStartTracking);
        if(callStartTracking){
            return  executeStartTracking(bitmap_bytes, image.getWidth(), image.getHeight(),0, 90 ,color_format, -1 , -1);
        }else {
            return executeKeepTracking(bitmap_bytes, image.getWidth(), image.getHeight(), 0);
        }
    }


    private List<VZEntity> getBoxEntities(boolean execute_status) {
        if(!isValid || !execute_status){
            return null;
        }
        String result_info_json_str = getBoxInfosJNI(mode);
        if(result_info_json_str.isEmpty()){
            return null;
        }
        List<VZEntity> result_entities = Utils.convert_result_info_to_box_entities(result_info_json_str );

        return result_entities;
    }

    public void release(int mode){
        releaseJNI(mode);
    }
}
