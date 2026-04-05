package vizinsight.atl.videoautofocus;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicBlur;

public class SegmentFilter {

    enum Style{
        COLOR,
        GREY_SCALE,
        SEPIA,
        BLUR,
        WHITE,
        BLACK,
        NONE
    }
    public static Bitmap filter(Context context, Bitmap image, Bitmap mask, Style BG_STYLE, Style FG_STYLE){
        int width = image.getWidth();
        int height = image.getHeight();

        image = image.copy(Bitmap.Config.ARGB_8888, true);
        mask = mask.copy(Bitmap.Config.ARGB_8888, true);
        if(mask.getWidth() != width || mask.getHeight() != height){
            mask = Bitmap.createScaledBitmap(mask, width, height, false);
        }

        Bitmap bgImage = image.copy(Bitmap.Config.ARGB_8888, true);
        switch(BG_STYLE){
            case COLOR:
                break;
            case GREY_SCALE:
                bgImage = convertToGreyscaleImage(bgImage);
                break;
            case SEPIA:
                bgImage = convertToSepiaImage(bgImage);
                break;
            case BLUR:
                bgImage = convertToBlurImage(context, bgImage);
                break;
            case WHITE:
                bgImage.eraseColor(Color.WHITE);
                break;
            case BLACK:
                bgImage.eraseColor(Color.BLACK);
                break;
            case NONE:
                bgImage.eraseColor(Color.TRANSPARENT);
                break;
        }

        Bitmap maskImage = getMaskedImage(image, mask);
        switch(FG_STYLE){
            case COLOR:
                break;
            case GREY_SCALE:
                maskImage  = convertToGreyscaleImage(maskImage);
                break;
            case SEPIA:
                maskImage = convertToSepiaImage(maskImage);
                break;
            case BLUR:
                maskImage  = convertToBlurImage(context, maskImage);
                break;
            case WHITE:
                maskImage.eraseColor(Color.WHITE);
                break;
            case NONE:
                maskImage .eraseColor(Color.TRANSPARENT);
                break;
        }

        if(BG_STYLE == Style.NONE){
            return maskImage;
        }
        Bitmap outputImage = overlayMask(bgImage , maskImage);

        return outputImage;
    }


    private static Bitmap overlayMask(Bitmap bgImage, Bitmap maskImage) {
        Paint paint = new Paint(Paint.FILTER_BITMAP_FLAG);
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.FILL);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.XOR));
        paint.setAlpha(255);

        Bitmap output = Bitmap.createBitmap(bgImage);
        Canvas canvas = new Canvas(output);
        canvas.drawBitmap(maskImage, new Matrix(),paint);

        Bitmap output2 = Bitmap.createBitmap(maskImage);
        Canvas canvas2 = new Canvas(output2);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_ATOP));
        canvas2.drawBitmap(bgImage, new Matrix(),paint);

        return output2;
    }

    private static Bitmap getMaskedImage(Bitmap image, Bitmap mask) {
        Paint paint = new Paint(Paint.FILTER_BITMAP_FLAG);
        paint.setAntiAlias(true);
//        BitmapShader shader = new BitmapShader(image, Shader.TileMode.CLAMP, Shader.TileMode.CLAMP);
//        paint.setShader(shader);
        paint.setStyle(Paint.Style.FILL);
//        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.OVERLAY));
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));

        Bitmap output = Bitmap.createBitmap(image);
        Canvas canvas = new Canvas(output);
        canvas.drawBitmap(mask, 0,0,paint);
        return output;
    }

    private static Bitmap convertToBlurImage(Context context, Bitmap image){
        float BLUR_RADIUS = 10f;
        Bitmap output = Bitmap.createBitmap(image);
        final RenderScript renderScript = RenderScript.create(context);
        Allocation alIn = Allocation.createFromBitmap(renderScript, image);
        Allocation alOut = Allocation.createFromBitmap(renderScript, output);

        ScriptIntrinsicBlur intrinsic = ScriptIntrinsicBlur.create(renderScript, Element.U8_4(renderScript));
        intrinsic.setRadius(BLUR_RADIUS);
        intrinsic.setInput(alIn);
        intrinsic.forEach(alOut);
        alOut.copyTo(output);
        return output;
    }

    private static Bitmap convertToGreyscaleImage(Bitmap image){
        float[] matrix = new float[]{
                0.3f, 0.59f, 0.11f, 0, 0,
                0.3f, 0.59f, 0.11f, 0, 0,
                0.3f, 0.59f, 0.11f, 0, 0,
                0,    0,     0,     1, 0,
        };
//        float[] matrix = new float[]{
//                0.393f, 0.769f, 0.189f, 0, 0,
//                0.349f, 0.686f, 0.168f, 0, 0,
//                0.272f, 0.534f, 0.131f, 0, 0,
//                0,    0,     0,     1, 0,
//        };
        Bitmap greyscaleImage = applyKernelRGB(image, matrix);

        return greyscaleImage;
    }

    private static Bitmap convertToSepiaImage(Bitmap image){
        float[] matrix = new float[]{
                0.393f, 0.769f, 0.189f, 0, 0,
                0.349f, 0.686f, 0.168f, 0, 0,
                0.272f, 0.534f, 0.131f, 0, 0,
                0,    0,     0,     1, 0,
        };
        Bitmap greyscaleImage = applyKernelRGB(image, matrix);

        return greyscaleImage;
    }

    private static Bitmap applyKernelRGB(Bitmap image, float[] matrix) {
        int width = image.getWidth();
        int height = image.getHeight();
        Bitmap filteredImage = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(filteredImage);
        Paint paint = new Paint();
        ColorMatrixColorFilter filter = new ColorMatrixColorFilter(matrix);
        paint.setColorFilter(filter);
        canvas.drawBitmap(image, 0,0,paint);
        return filteredImage;
    }

    private static Bitmap convertToAlphaMask(Bitmap mask){
//        Bitmap alphaMask = Bitmap.createBitmap(mask.getWidth(), mask.getHeight(), Bitmap.Config.ALPHA_8);
//        Canvas canvas = new Canvas(alphaMask);
//        canvas.drawBitmap(mask, 0,0,null);
        Bitmap alphaMask = mask.extractAlpha();
        return alphaMask;
    }
}
