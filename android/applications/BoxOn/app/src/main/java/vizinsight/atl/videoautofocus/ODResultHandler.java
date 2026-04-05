package vizinsight.atl.videoautofocus;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;

import java.util.List;

import vizinsight.atl.object_detector.VZEntity;
import vizinsight.atl.object_detector.VZImageDecoder;
import vizinsight.atl.object_detector.VZObjectDetector;

public class ODResultHandler {

    Context mContext;
    DisplayMode mDisplayMode;
    VZObjectDetector objectDetector;

    enum DisplayMode{
        DIALOG_VIEW_MODE,
        NONE
    }

    public ODResultHandler(Builder builder){
        mContext = builder.mContext;
        mDisplayMode = builder.mDisplayMode;
        objectDetector = builder.mImageSegmenter;
    }

    public void generate(byte[] bytes, int width, int height, int rotationDegrees, int imageFormat){
        switch(mDisplayMode){
            case DIALOG_VIEW_MODE:
                generate_result_on_image(bytes, width, height, rotationDegrees, imageFormat);
                break;

        }
    }

    private Bitmap getBitmapFromBytes(byte[] bytes, int width, int height, int rotationDegrees, int imageFormat) {
        int bytes_width = width;
        int bytes_height = height;
        if(rotationDegrees % 180 != 0){
            bytes_width = height;
            bytes_height = width;
        }

        Bitmap image = null;
        if(imageFormat == VZObjectDetector.IMAGEFORMAT_NV21){
            image = Utils.YUV2Bitmap(bytes, bytes_width, bytes_height, rotationDegrees);
        }
        else if(imageFormat == VZObjectDetector.IMAGEFORMAT_RGBA){
            image = Utils.RGBA2Bitmap(bytes, bytes_width, bytes_height, rotationDegrees);
        }
        return image;
    }

    ImageView touch_image;
    ImageView mask_image;
    ProgressBar loadingProgressBar;

    private void generate_result_on_image(byte[] bytes, int width, int height, int rotationDegrees, int imageFormat) {
        Bitmap image = getBitmapFromBytes(bytes, width, height, rotationDegrees, imageFormat);

        final Dialog dialog = new Dialog(mContext);
        dialog.setContentView(R.layout.dialog_segment_result);
        dialog.setTitle("Select object");

        touch_image = dialog.findViewById(R.id.segment_input_image);
        mask_image = dialog.findViewById(R.id.segment_result_mask);
        loadingProgressBar = dialog.findViewById(R.id.loading_progress);

        touch_image.setImageBitmap(image);
        dialog.show();

        SegmenterRunner runner = new SegmenterRunner();
        runner.setImage(image);
        runner.execute();
    }

    private class SegmenterRunner extends AsyncTask<String, Integer, Integer>{

        Bitmap image;
        int touchX = -1, touchY = -1;
        public void setImage(Bitmap image){
            this.image = image;
//            touch_image.setImageBitmap(image);
        }
        public void setTouchLocation(int x, int y){
            touchX = x;
            touchY = y;
        }
        List<VZEntity> entities;
        @Override
        protected Integer doInBackground(String... strings) {
            byte[] bitmap_bytes = VZImageDecoder.decodeImage(image, VZImageDecoder.IMAGE_FORMAT_RGBA8888);
            if(touchX == -1){
                entities = objectDetector.execute(bitmap_bytes, image.getWidth(), image.getHeight(), 0, VZObjectDetector.IMAGEFORMAT_RGBA);
            }
            return null;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            loadingProgressBar.setVisibility(View.VISIBLE);
        }

        @Override
        protected void onPostExecute(Integer integer) {
            super.onPostExecute(integer);

            if(entities!= null && !entities.isEmpty()){
//                VZEntity ent = entities.get(0);
//                Bitmap mask = Bitmap.createScaledBitmap(ent.mask, image.getWidth(), image.getHeight(), false);
//
//                mask = SegmentFilter.filter(mContext, image, mask, ((CoreActivity)mContext).mBGStyle, ((CoreActivity)mContext).mFGStyle);
//                mask_image.setAlpha(1.0f);
//
//
//                mask_image.setImageBitmap(mask);


            }
            else{
                mask_image.setImageResource(android.R.color.transparent);
            }
            loadingProgressBar.setVisibility(View.INVISIBLE);
        }
    }


    public static class Builder{
        private Context mContext;
        private DisplayMode mDisplayMode = DisplayMode.NONE;
        private VZObjectDetector mImageSegmenter;

        public Builder(Context context){
            mContext = context;
        }
        public Builder setMode(DisplayMode mode){
            mDisplayMode = mode;
            return this;
        }
        public Builder setSegmenter(VZObjectDetector segmenter){
            mImageSegmenter = segmenter;
            return this;
        }
        public ODResultHandler build(){
            validateBuilder();
            return new ODResultHandler(this);
        }
        private boolean validateBuilder(){
            if(mImageSegmenter == null){
                Log.e("SegmentResultHandler","Segmenter cannot be null");
                return false;
            }
            return true;
        }
    }
}
