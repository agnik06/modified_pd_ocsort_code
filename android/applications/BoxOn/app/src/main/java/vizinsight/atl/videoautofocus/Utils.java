package vizinsight.atl.videoautofocus;

import android.app.Activity;
import android.content.ContentUris;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;
import boofcv.struct.image.ImageUInt8;
import boofcv.struct.image.MultiSpectral;

import androidx.camera.core.ImageProxy;
import androidx.core.app.ActivityCompat;

import android.view.Surface;
import android.webkit.MimeTypeMap;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import georegression.struct.shapes.Quadrilateral_F64;

/**
 * Created by batman on 28/12/17.
 */

public class Utils {

    public static String getFilePath(Context context, Uri uri) throws URISyntaxException {
        String selection = null;
        String[] selectionArgs = null;
        // Uri is different in versions after KITKAT (Android 4.4), we need to
        if (Build.VERSION.SDK_INT >= 19 && DocumentsContract.isDocumentUri(context.getApplicationContext(), uri)) {
            if (isExternalStorageDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                return Environment.getExternalStorageDirectory() + "/" + split[1];
            } else if (isDownloadsDocument(uri)) {
                final String id = DocumentsContract.getDocumentId(uri);
                uri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));
            } else if (isMediaDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];
                if ("image".equals(type)) {
                    uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                } else if ("video".equals(type)) {
                    uri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                } else if ("audio".equals(type)) {
                    uri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                }
                selection = "_id=?";
                selectionArgs = new String[]{
                        split[1]
                };
            }
        }
        if ("content".equalsIgnoreCase(uri.getScheme())) {
            String[] projection = {
                    MediaStore.Images.Media.DATA
            };
            Cursor cursor = null;
            try {
                cursor = context.getContentResolver()
                        .query(uri, projection, selection, selectionArgs, null);
                int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                if (cursor.moveToFirst()) {
                    return cursor.getString(column_index);
                }
            } catch (Exception e) {
            }
        } else if ("file".equalsIgnoreCase(uri.getScheme())) {
            return uri.getPath();
        }
        return null;
    }

    public static boolean isExternalStorageDocument(Uri uri) {
        return "com.android.externalstorage.documents".equals(uri.getAuthority());
    }

    public static boolean isDownloadsDocument(Uri uri) {
        return "com.android.providers.downloads.documents".equals(uri.getAuthority());
    }

    public static boolean isMediaDocument(Uri uri) {
        return "com.android.providers.media.documents".equals(uri.getAuthority());
    }

    public static Bitmap rotateBitmap(Bitmap bitmap, int orientation) {

        Matrix matrix = new Matrix();
        switch (orientation) {
            case ExifInterface.ORIENTATION_NORMAL:
                return bitmap;
            case ExifInterface.ORIENTATION_FLIP_HORIZONTAL:
                matrix.setScale(-1, 1);
                break;
            case ExifInterface.ORIENTATION_ROTATE_180:
                matrix.setRotate(180);
                break;
            case ExifInterface.ORIENTATION_FLIP_VERTICAL:
                matrix.setRotate(180);
                matrix.postScale(-1, 1);
                break;
            case ExifInterface.ORIENTATION_TRANSPOSE:
                matrix.setRotate(90);
                matrix.postScale(-1, 1);
                break;
            case ExifInterface.ORIENTATION_ROTATE_90:
                matrix.setRotate(90);
                break;
            case ExifInterface.ORIENTATION_TRANSVERSE:
                matrix.setRotate(-90);
                matrix.postScale(-1, 1);
                break;
            case ExifInterface.ORIENTATION_ROTATE_270:
                matrix.setRotate(-90);
                break;
            default:
                return bitmap;
        }
        try {
            Bitmap bmRotated = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
            bitmap.recycle();
            return bmRotated;
        }
        catch (OutOfMemoryError e) {
            e.printStackTrace();
            return null;
        }
    }



    public static int getImageOrientation(String path){
        ExifInterface exif = null;
        try {
            exif = new ExifInterface(path);
        } catch (IOException e) {
            e.printStackTrace();
        }
        int orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                ExifInterface.ORIENTATION_UNDEFINED);
        return orientation;
    }

    public static void list_files(String directoryName, ArrayList<File> files) {
        File directory = new File(directoryName);

        // get all the files from a directory
        File[] fList = directory.listFiles();
        if(fList == null || fList.length == 0)
            return;
        for (File file : fList) {
            if (file.isFile() && isImageFile(file) && !isFileNameTooLarge(file)) {
                files.add(file);
            } else if (file.isDirectory()) {
                list_files(file.getAbsolutePath(), files);
            }
        }
    }

    public static boolean isFileNameTooLarge(File file){
        String fileName = file.getName();
        int fieNameLength = fileName.length();
        Log.d("Utils:","isImageFile fileName : "+fileName+"  fieNameLength : "+fieNameLength);

        if(fieNameLength > 1024){
            return true;
        }
        return false;
    }

   public static boolean isImageFile(File file){
        String fileName = file.getName();
        if(fileName.contains(".JPEG") || fileName.contains(".jpg") || fileName.contains(".JPG") ||fileName.contains(".jpeg") 
            || fileName.contains(".png") || fileName.contains(".PNG") || fileName.contains(".bmp") || fileName.contains(".BMP")){
                return true;
        }
        return false;
    }

    public static boolean isValidImagePath(String filepath){
        if(filepath != null && filepath.lastIndexOf(".") != -1){
            String ext = filepath.substring(filepath.lastIndexOf(".")+1);
            String mime_type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(ext);
            String type = mime_type.split("/")[0];
            return type.equals("image");
        }
        return false;
    }

    public static boolean verifyPermissions(Context context, String[] permission_list, int multiple_permissions) {
        int permission_status = 1;
        for(String permission: permission_list) {
            permission_status = ActivityCompat.checkSelfPermission(context, permission);
            if (permission_status != PackageManager.PERMISSION_GRANTED) {
                break;
            }
        }
        if(permission_status!=PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(
                    (Activity) context,
                    permission_list,
                    multiple_permissions
            );
            return false;
        }
        return true;
    }

    public static Bitmap overlay(Bitmap b1, Bitmap b2){
        Bitmap overlayedBitmap = Bitmap.createBitmap(b1.getWidth(), b1.getHeight(), b1.getConfig());
        Canvas canvas = new Canvas(overlayedBitmap);
        canvas.drawBitmap(b1, new Matrix(), null);
        canvas.drawBitmap(b2, new Matrix(), null);
        return overlayedBitmap;
    }

    public static byte[] convertYUV420ImageToPackedNV21(ImageProxy img) {
        final int w = img.getWidth();
        final int h = img.getHeight();
        final ImageProxy.PlaneProxy planeList[] = img.getPlanes();
        ByteBuffer y_buffer = planeList[0].getBuffer();
        ByteBuffer u_buffer = planeList[1].getBuffer();
        ByteBuffer v_buffer = planeList[2].getBuffer();
        byte[] dataCopy = new byte[y_buffer.capacity() + u_buffer.capacity() + v_buffer.capacity()];

        final int color_pixel_stride = planeList[1].getPixelStride();
        final int y_size = y_buffer.capacity();
        final int u_size = u_buffer.capacity();
        final int data_offset = w * h;
        for (int i = 0; i < y_size; i++) {
            dataCopy[i] = (byte) (y_buffer.get(i) & 255);
        }
        for (int i = 0; i < u_size / color_pixel_stride; i++) {
            dataCopy[data_offset + 2 * i] = v_buffer.get(i * color_pixel_stride);
            dataCopy[data_offset + 2 * i + 1] = u_buffer.get(i * color_pixel_stride);
        }
        return dataCopy;
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
            bm = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(), bm.getHeight(), matrix, false);

        } catch (Exception e) {
            e.printStackTrace();
        }
        return bm;
    }

    public static Bitmap get_thresholded_mask(Bitmap image){
        Bitmap image2=image.copy(Bitmap.Config.ARGB_8888,true);
        int h=image.getHeight(),w=image.getWidth();
        for(int i=0;i<w;i++){
            for(int j=0;j<h;j++){
                int id=image.getPixel(i,j);
                int color2=Color.argb(255, 2, 2, 2);
                int color1=Color.argb(255, 1, 1, 1);

                if(id==color2)
                    image2.setPixel(i,j,Color.argb(255, 255, 255, 255));
                else if(id==color1)
                    image2.setPixel(i,j,Color.argb(255, 128, 128, 128));
                else
                    image2.setPixel(i,j,Color.argb(255,  0, 0, 0));
            }
        }
        return image2;
    }


    public static Bitmap rotateDegreesBitmap(Bitmap bm, int rotation) {
        Matrix matrix = new Matrix();
        matrix.preRotate(rotation);
        bm = Bitmap.createBitmap(bm, 0, 0, bm.getWidth(), bm.getHeight(), matrix, false);
        return bm;
    }

    public static Bitmap YUV2Bitmap(byte[] nv21_bytes, int width, int height, int rotationDegrees){
        YuvImage yuvImage = new YuvImage(nv21_bytes, ImageFormat.NV21, width, height, null);
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        yuvImage.compressToJpeg(new Rect(0,0,width, height), 100, os);
        byte[] jpegByteArray = os.toByteArray();

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inMutable = true;
        options.outConfig = Bitmap.Config.ARGB_8888;
        Bitmap bitmap = BitmapFactory.decodeByteArray(jpegByteArray, 0, jpegByteArray.length, options);
        bitmap = rotateDegreesBitmap(bitmap, rotationDegrees);
//        bitmap = bitmap.copy(Bitmap.Config.RGBA_F16, true);
        return bitmap;
    }

    public static Bitmap RGBA2Bitmap(byte[] array, int width, int height, int rotationDegrees) {
        int[] data = new int[width * height];

        int count = 0;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                byte R = array[count++];
                byte G = array[count++];
                byte B = array[count++];
                byte A = array[count++];
//                data[index] = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
                data[index] = (255 & 0xff) << 24 | (R & 0xff) << 16 | (G & 0xff) << 8 | (B & 0xff);

                index++;
            }
        }
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.setPixels(data, 0, width, 0,0, width, height);
        return bitmap;
    }


    public static MultiSpectral<ImageUInt8> getMultiSpectral8(byte[] nv21_bytes, int width, int height, int scale_factor, boolean rotate){
        MultiSpectral<ImageUInt8> image;
        int width_bytes = width;
        int height_bytes = height;

        if(rotate){
            int temp = width/scale_factor;
            width    = height/scale_factor;
            height   = temp;

            image = new MultiSpectral<ImageUInt8>(ImageUInt8.class,width,height,1);
        }
        else{
            width  = width/scale_factor;
            height = height/scale_factor;
            image = new MultiSpectral<ImageUInt8>(ImageUInt8.class,width,height,1);
        }

        for(int i=0; i<height_bytes/scale_factor; i++){
            for(int j=0; j<width_bytes/scale_factor; j++){
//                Log.e("dim",image_width+","+image_height+" | "+image_uint8.getWidth()+","+image_uint8.getHeight()+" | "+i+","+j);
//                byte unit_byte = bytes[j*height_bytes+(height_bytes-1-i)];
                byte unit_byte = nv21_bytes[i*scale_factor*width_bytes + j*scale_factor];
                if(rotate){
                    image.getBand(0).set( height_bytes/scale_factor-1-i, j, unit_byte&0xff); //Greyscale image_uint8 extraction with Rotation of image_uint8
                }
                else{
//                    image.getBand(0).set( height_bytes/scale_factor-1-i, j, unit_byte&0xff); //Greyscale image_uint8 extraction with Rotation of image_uint8
                    image.getBand(0).set( j, i, unit_byte&0xff); //Greyscale image_uint8 extraction with Rotation of image_uint8
                }
            }
        }
        return image;
    }


    public static Quadrilateral_F64 getLocationQuad(int left, int top, int right, int bottom){
        Quadrilateral_F64 location = new Quadrilateral_F64();
        location.a.set(left,bottom);
        location.b.set(left,top);
        location.c.set(right,top);
        location.d.set(right,bottom);

        return location;
    }

    public static Quadrilateral_F64 getLocationQuad(Rect r){
        Quadrilateral_F64 location = new Quadrilateral_F64();
        location.a.set(r.left,r.bottom);
        location.b.set(r.left,r.top);
        location.c.set(r.right,r.top);
        location.d.set(r.right,r.bottom);
        return location;
    }
    public static Quadrilateral_F64 getLocationQuad(Rect r, int scale_factor){
        Quadrilateral_F64 location = new Quadrilateral_F64();
        r.set(r.left/scale_factor, r.top/scale_factor, r.right/scale_factor, r.bottom/scale_factor);
        location.d.set(r.left,r.bottom);
        location.a.set(r.left,r.top);
        location.b.set(r.right,r.top);
        location.c.set(r.right,r.bottom);

        return location;
    }

    public static Rect getLocationRect(Quadrilateral_F64 location, int scale_factor){
        Rect r = new Rect();
        r.left   = (int)location.a.x*scale_factor;
        r.top    = (int)location.a.y*scale_factor;
        r.right  = (int)location.c.x*scale_factor;
        r.bottom = (int)location.c.y*scale_factor;
        return r;
    }


    public static int convertSurfaceRotationToDegrees(int display_rotation) {
        switch(display_rotation){
            case Surface.ROTATION_0:
                return 0;
            case Surface.ROTATION_90:
                return 90;
            case Surface.ROTATION_180:
                return 180;
            case Surface.ROTATION_270:
                return 270;
            default:
                return -1;
        }
    }

    public static Bitmap resize(Bitmap image, int maxWidth, int maxHeight) {
        if (maxHeight > 0 && maxWidth > 0) {
            int width = image.getWidth();
            int height = image.getHeight();
            float ratioBitmap = (float) width / (float) height;
            float ratioMax = (float) maxWidth / (float) maxHeight;

            int finalWidth = maxWidth;
            int finalHeight = maxHeight;
            if (ratioMax > ratioBitmap) {
                finalWidth = (int) ((float)maxHeight * ratioBitmap);
            } else {
                finalHeight = (int) ((float)maxWidth / ratioBitmap);
            }
            image = Bitmap.createScaledBitmap(image, finalWidth, finalHeight, true);
            return image;
        } else {
            return image;
        }
    }



    public static Bitmap padResizeImage(Bitmap image, int maxWidth, int maxHeight){
        if (maxHeight > 0 && maxWidth > 0) {
            int width = image.getWidth();
            int height = image.getHeight();
            float ratioBitmap = (float) width / (float) height;
            float ratioMax = (float) maxWidth / (float) maxHeight;

            int finalWidth = maxWidth;
            int finalHeight = maxHeight;
            if (ratioMax > ratioBitmap) {
                finalWidth = (int) ((float)maxHeight * ratioBitmap);
            } else {
                finalHeight = (int) ((float)maxWidth / ratioBitmap);
            }
            image = Bitmap.createScaledBitmap(image, finalWidth, finalHeight, true);

            Bitmap bg_image = Bitmap.createBitmap(maxWidth, maxHeight, Bitmap.Config.ARGB_8888);
            bg_image.eraseColor(Color.BLACK);

            image = overlay(bg_image, image);
            return image;
        } else {
            return image;
        }
    }

    public static String getPath(Context context, Uri uri) {
        // just some safety built in
        if( uri == null ) {
            // TODO perform some logging or show user feedback
            return null;
        }
        // try to retrieve the image from the media store first
        // this will only work for images selected from gallery
        String[] projection = { MediaStore.Images.Media.DATA };
        Cursor cursor = context.getContentResolver().query(uri, projection, null, null, null);
        if( cursor != null ){
            int column_index = cursor
                    .getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            String path = cursor.getString(column_index);
            cursor.close();

            Log.d("Utils", "Returning from A " + path);
            return path;
        }
        // this is our fallback here
        Uri image_uri = uri;
        String path = null;
        try {
            path = Utils.getFilePath(context, image_uri);
        } catch (URISyntaxException e) {
            e.printStackTrace();
        }
        Log.d("Utils", "Returning from B " + path);
        return path;
    }

}
