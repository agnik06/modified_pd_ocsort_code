package vizinsight.atl.videoautofocus;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.util.Size;
import android.view.TextureView;
import android.widget.Toast;

import java.io.File;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.camera.core.CameraX;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageAnalysisConfig;
import androidx.camera.core.ImageCapture;
import androidx.camera.core.ImageCaptureConfig;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.core.PreviewConfig;
import androidx.lifecycle.LifecycleOwner;
import vizinsight.atl.object_detector.VZImageDecoder;

public class CameraHandler {

    public interface PreviewUpdateListener{
        void onPreviewUpdate(byte[] nv21_bytes, int width, int height, int rotationDegrees);
    }

    PreviewUpdateListener mPreviewUpdateListener;
    public void setPreviewUpdateListener(PreviewUpdateListener listener){
        mPreviewUpdateListener = listener;
    }

    public interface ImageCaptureListener{
        void onCapture(byte[] rgba_bytes, int width, int height, int rotationDegrees);
    }

    ImageCaptureListener mImageCaptureListener;
    public void setImageCaptureListener(ImageCaptureListener listener){
        mImageCaptureListener = listener;
    }

    Context mContext;
    TextureView mTextureView;
    int mImageWidth;
    int mImageHeight;

    ImageCapture imageCapture;
    ImageCapture.OnImageSavedListener mOnImageSavedListener;

    CameraX.LensFacing lensFacing = CameraX.LensFacing.BACK;

    public Size getViewResolution(){
        return new Size(mImageWidth, mImageHeight);
    }

    public CameraHandler(Context context, TextureView textureView){
        mContext = context;
        mTextureView = textureView;
        mImageWidth = context.getResources().getInteger(R.integer.camera_resolution_width);
        mImageHeight = context.getResources().getInteger(R.integer.camera_resolution_height);

        final int display_rotation = ((Activity)context).getWindowManager().getDefaultDisplay().getRotation();

        final PreviewConfig preview_config = new PreviewConfig.Builder()
                .setLensFacing(lensFacing)
                .setTargetResolution(new Size(mImageWidth,mImageHeight))
                .setTargetRotation(display_rotation)
                .build();
        Preview preview = new Preview(preview_config);
        preview.setOnPreviewOutputUpdateListener(new Preview.OnPreviewOutputUpdateListener() {
            @Override
            public void onUpdated(Preview.PreviewOutput output) {
                float centerX = mImageWidth  /2f;
                float centerY = mImageHeight /2f;

                int rotationDegrees = (360-Utils.convertSurfaceRotationToDegrees(display_rotation))%360;
                Matrix matrix = new Matrix();
                matrix.postRotate((float) -rotationDegrees, centerX, centerY);

                float bufferRatio = ((float) output.getTextureSize().getHeight()) / output.getTextureSize().getWidth();
                int scaledWidth, scaledHeight;
                if(mImageWidth > mImageHeight){
                    scaledHeight = mImageWidth;
                    scaledWidth  = Math.round(mImageWidth * bufferRatio);
                }
                else{
                    scaledHeight = mImageHeight;
                    scaledWidth  = Math.round(mImageHeight * bufferRatio);
                }
                float scaleX = ((float) scaledWidth) / mImageWidth;
                float scaleY = ((float) scaledHeight) / mImageHeight;
                matrix.preScale(scaleX, scaleY, -9.85f*centerX, centerY);

                mTextureView.setTransform(matrix);
                mTextureView.setSurfaceTexture(output.getSurfaceTexture());
            }
        });

       final ImageAnalysisConfig image_analysis_config =
                new ImageAnalysisConfig.Builder()
                        .setLensFacing(lensFacing)
                        .setTargetResolution(new Size(mImageWidth, mImageHeight))
                        .setImageReaderMode(ImageAnalysis.ImageReaderMode.ACQUIRE_LATEST_IMAGE)
                        .build();
        ImageAnalysis imageAnalysis = new ImageAnalysis(image_analysis_config);
        imageAnalysis.setAnalyzer(new ImageAnalysis.Analyzer() {
            @Override
            public void analyze(ImageProxy image, int rotationDegrees) {
                if(mPreviewUpdateListener != null){
                    byte[] bytes = Utils.convertYUV420ImageToPackedNV21(image);
                    int width = mImageWidth, height = mImageHeight;
                    if(rotationDegrees % 180 != 0){
                        width = mImageHeight;
                        height = mImageWidth;
                    }
                    mPreviewUpdateListener.onPreviewUpdate(bytes, width, height, rotationDegrees);
                }
            }
        });

        ImageCaptureConfig image_capture_config =
            new ImageCaptureConfig.Builder()
                .setLensFacing(lensFacing)
                .setTargetRotation(display_rotation)
                .setTargetResolution(new Size(mImageWidth, mImageHeight))
                .build();
        imageCapture = new ImageCapture(image_capture_config);
        mOnImageSavedListener = new ImageCapture.OnImageSavedListener() {
            @Override
            public void onImageSaved(@NonNull File file) {
                if(mImageCaptureListener != null){
                    Bitmap image_bitmap_unfliped = Utils.get_exif_corrected_bitmap(file.getPath());
                    Matrix matrix=new Matrix();
                    matrix.preScale(-1.0f,1.0f);
                    Bitmap image_bitmap=Bitmap.createBitmap(image_bitmap_unfliped,0,0,image_bitmap_unfliped.getWidth(),image_bitmap_unfliped.getHeight(),matrix,true);
                    byte[] rgba_bytes = VZImageDecoder.decodeImage(image_bitmap, VZImageDecoder.IMAGE_FORMAT_RGBA8888);
                    mImageCaptureListener.onCapture(rgba_bytes, image_bitmap.getWidth(), image_bitmap.getHeight(), 0);
                }
            }

            @Override
            public void onError(@NonNull ImageCapture.ImageCaptureError imageCaptureError, @NonNull String message, @Nullable Throwable cause) {
                Toast.makeText(mContext, "Error in capturing image", Toast.LENGTH_SHORT).show();
            }
        };

        CameraX.bindToLifecycle((LifecycleOwner)mContext, imageCapture, imageAnalysis, preview);
    }

    public void caputureImage(){
        File file = new File("/sdcard/capture.jpg");
        imageCapture.takePicture(file, mOnImageSavedListener);
    }

    public void release(){
        CameraX.unbindAll();
        mPreviewUpdateListener = null;
    }
}
