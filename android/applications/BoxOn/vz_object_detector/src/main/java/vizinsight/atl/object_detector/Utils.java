package vizinsight.atl.object_detector;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.ExifInterface;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static java.lang.StrictMath.max;
import static java.lang.StrictMath.min;

public class Utils {
    public static Bitmap convert_mask_buffer_to_bitmap(byte[] mask_buffer, int mask_width, int mask_height) {
        int[] bytes_color = new int[mask_buffer.length];
        for(int i=0; i < mask_buffer.length; i++){
            int val =  (mask_buffer[i]);
            bytes_color[i] = Color.argb(255, val, val, val);
            if(val < 128){
                bytes_color[i] = Color.argb(0, val, val, val);
            }
        }
        Bitmap image = Bitmap.createBitmap(bytes_color, mask_width, mask_height, Bitmap.Config.ARGB_8888);
//        Bitmap image = Bitmap.createBitmap(bytes_color, mask_width, mask_height, Bitmap.Config.ALPHA_8);
        image = Bitmap.createScaledBitmap(image, image.getWidth(), image.getHeight(), true);
        return image;
    }

    public static Bitmap convert_argmax_mask_buffer_to_bitmap(byte[] mask_buffer, int mask_width, int mask_height) {

        int[] bytes_color = new int[mask_buffer.length];

        for(int i=0; i < mask_buffer.length; i++){
            int val =  (mask_buffer[i]);
            bytes_color[i] = Color.argb(255, val, val, val);
            if(val <1 ){
                bytes_color[i] = Color.argb(0, val, val, val);
            }
        }
        Bitmap image = Bitmap.createBitmap(bytes_color, mask_width, mask_height, Bitmap.Config.ARGB_8888);
//        Bitmap image = Bitmap.createBitmap(bytes_color, mask_width, mask_height, Bitmap.Config.ALPHA_8);
        image = Bitmap.createScaledBitmap(image, image.getWidth(), image.getHeight(), true);
        return image;
    }

    public static VZEntity convert_segment_info_to_segment_entity(String json_str) {
        try {
            JSONObject reader= new JSONObject(json_str);
            String tag  = reader.getString("tag");
            int width   = reader.getInt("image_width");
            int height  = reader.getInt("image_height");
            int left    = reader.getInt("left");
            int top     = reader.getInt("top");
            int right   = reader.getInt("right");
            int bottom  = reader.getInt("bottom");
            float score = (float)reader.getDouble("score");
            Log.e("AJ Debug",tag+" w,h"+width+","+height+" - "+left+","+top+","+right+","+bottom);

//            return new VZEntity(null, tag, image_width, image_height, left, top, right, bottom);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return null;
    }

    public static List<VZEntity> convert_result_info_to_box_entities(String json_str) {
        Log.d("UTils:","convert_result_info_to_box_entities");
        try {
            ArrayList<VZEntity> entities = new ArrayList<>();
            JSONArray arr_reader = new JSONArray(json_str);
            for(int i=0; i<arr_reader.length(); i++){
                JSONObject reader= arr_reader.getJSONObject(i);

                String tag  = reader.getString("tag");
                int width   = reader.getInt("image_width");
                int height  = reader.getInt("image_height");
                int left    = reader.getInt("left");
                int top     = reader.getInt("top");
                int right   = reader.getInt("right");
                int bottom  = reader.getInt("bottom");
                float score = (float)reader.getDouble("score");
                int trackid = reader.getInt("track_id");
                Log.e("AJ Debug",tag+" w,h"+width+","+height+" - "+left+","+top+","+right+","+bottom+","+trackid);

                entities.add(new VZEntity(tag, width, height, left, top, right, bottom,score,trackid));
            }
            Log.d("UTils:","exit convert_result_info_to_box_entities");
            return entities;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        Log.d("UTils:","exit convert_result_info_to_box_entities");
        return null;
    }

    public static List<VZEntity> convert_result_info_to_track_entities(String json_str) {
        Log.d("UTils:","convert_result_info_to_track_entities");
        try {
            ArrayList<VZEntity> entities = new ArrayList<>();
            JSONArray arr_reader = new JSONArray(json_str);
            for(int i=0; i<arr_reader.length(); i++){
                JSONObject reader= arr_reader.getJSONObject(i);

                String tag  = " ";//reader.getString("tag");
                int tagId   = reader.getInt("tag_id");
                int ID  = reader.getInt("id");
                int left    = (int)reader.getDouble("left");
                int top     = (int)reader.getDouble("top");
                int right   = (int)reader.getDouble("right");
                int bottom  = (int)reader.getDouble("bottom");
                float score = (float)reader.getDouble("score");
                Log.e("AJ Debug",tag+" tagId,ID"+tagId+","+ID+" - "+left+","+top+","+right+","+bottom);

                entities.add(new VZEntity(tag, tagId, ID, left, top, right, bottom, score, ID));
            }
            Log.d("UTils:"," exit convert_result_info_to_track_entities");
            return entities;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        Log.d("UTils:","convert_result_info_to_track_entities");
        return null;
    }

    public static Bitmap get_exif_corrected_bitmap(String image_path){
        Bitmap bm = BitmapFactory.decodeFile(image_path);
        try {
            ExifInterface exif = new ExifInterface(image_path);
            int rotation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
            Matrix matrix = new Matrix();
            switch(rotation){
                case ExifInterface.ORIENTATION_ROTATE_90:
                    matrix.preRotate(90);
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    matrix.preRotate(180);
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    matrix.preRotate(270);
                    break;
            }
            if(bm != null) {
                bm = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(), bm.getHeight(), matrix, false);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return bm;
    }

    private static Bitmap rotateBitmap(Bitmap bm, int rotation) {
        Matrix matrix = new Matrix();
        switch(rotation){
            case ExifInterface.ORIENTATION_ROTATE_90:
                matrix.preRotate(270);
                break;
            case ExifInterface.ORIENTATION_ROTATE_180:
                matrix.preRotate(180);
                break;
            case ExifInterface.ORIENTATION_ROTATE_270:
                matrix.preRotate(90);
                break;
        }
        bm = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(), bm.getHeight(), matrix, false);
        return bm;
    }

    public static Bitmap YUV2Bitmap(byte[] nv21_bytes, int width, int height, int rotationDegrees){
        YuvImage yuvImage = new YuvImage(nv21_bytes, ImageFormat.NV21, width, height, null);
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        yuvImage.compressToJpeg(new Rect(0,0,width, height), 100, os);
        byte[] jpegByteArray = os.toByteArray();
        Bitmap bitmap = BitmapFactory.decodeByteArray(jpegByteArray, 0, jpegByteArray.length);
        bitmap = rotateBitmap(bitmap, rotationDegrees);
        return bitmap;
    }

    public static Rect rotation_corrected_roi(Rect roi,int width,int height,int angular_clockwise_rotation){
        if(roi.left == 0 && roi.right == 0 && roi.top == 0 && roi.bottom == 0){
            return roi;
        }
        Rect bounds=new Rect(roi.left, roi.top,
                roi.right, roi.bottom);
        int image_width  = width;
        int image_height = height;
        switch(angular_clockwise_rotation){
            case 270:
                bounds = new Rect(image_height - bounds.bottom, bounds.left, image_height - bounds.top, bounds.right);
                break;
            case 180:
                bounds = new Rect(image_width - bounds.right, image_height - bounds.bottom, image_width - bounds.left, image_height - bounds.top);
                break;
            case 90:
                bounds = new Rect(bounds.top, image_width - bounds.right, bounds.bottom, image_width - bounds.left);
                break;
        }
        return bounds;
    }

    public static int getImgeOrientation(String imagePath){
        ExifInterface exif = null;
        try {
            exif = new ExifInterface(imagePath);
        } catch (IOException e) {
            e.printStackTrace();
        }
        int rotation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);

        return rotation;
    }

}
