package vizinsight.atl.object_detector;

import android.graphics.Bitmap;
import android.util.Log;

import java.lang.OutOfMemoryError;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

/**
 * Created by user on 14/12/16.
 */
public class VZImageDecoder
{

    public final static int IMAGE_FORMAT_YUV420 = 0;
    public final static int IMAGE_FORMAT_ARGB8888 = 1;
    public final static int IMAGE_FORMAT_RGBA8888 = 2;
    public static byte [] decodeImage(Bitmap bitmap, int image_format) throws OutOfMemoryError
    {
        Log.i("BIPLAB_DEBUG", "Decoding Image ...");
        if(image_format == IMAGE_FORMAT_ARGB8888)
        {
            return getARGB(bitmap.getWidth(), bitmap.getHeight(), bitmap);
        }
        else if(image_format == IMAGE_FORMAT_YUV420)
        {
            return getNV21(bitmap.getWidth(), bitmap.getHeight(), bitmap);
        }
        else if(image_format == IMAGE_FORMAT_RGBA8888)
        {
            return getRGBA(bitmap.getWidth(), bitmap.getHeight(), bitmap);
        }
        Log.i("BIPLAB_DEBUG", "Decoding Image completed sucessfully.");
        return null;

    }

    static byte [] getRGBA(int inputWidth, int inputHeight, Bitmap bitmap) throws OutOfMemoryError
    {
        int [] data = new int[inputWidth * inputHeight];
        byte [] array = new byte[4* inputWidth * inputHeight];
        bitmap.getPixels(data, 0, inputWidth, 0, 0, inputWidth, inputHeight);

        //Do the conversion
        int count = 0;
        int index = 0;
        int a, R, G, B;
        for (int j = 0; j < inputHeight; j++) {
            for (int i = 0; i < inputWidth; i++) {

                a = (data[index] & 0xff000000) >> 24; // a is not used obviously
                R = (data[index] & 0xff0000) >> 16;
                G = (data[index] & 0xff00) >> 8;
                B = (data[index] & 0xff);

                array[count++] = (byte) ((R < 0) ? 0 : ((R > 255) ? 255 : R));
                array[count++] = (byte) ((G < 0) ? 0 : ((G > 255) ? 255 : G));
                array[count++] = (byte) ((B < 0) ? 0 : ((B > 255) ? 255 : B));
                array[count++] = (byte) ((a < 0) ? 0 : ((a > 255) ? 255 : a));

                index++;
            }
        }

        return  array;
    }
    static byte [] getNV21(int inputWidth, int inputHeight, Bitmap bitmap)
    {

        int [] argb = new int[inputWidth * inputHeight];
        bitmap.getPixels(argb, 0, inputWidth, 0, 0, inputWidth, inputHeight);
        byte [] yuv = new byte[inputWidth*inputHeight*3];
        encodeYUV420SP(yuv, argb, inputWidth, inputHeight);
        return yuv;
    }

    static byte [] getARGB(int inputWidth, int inputHeight, Bitmap bitmap)
    {
        int [] data = new int[inputWidth * inputHeight];
        bitmap.getPixels(data, 0, inputWidth, 0, 0, inputWidth, inputHeight);
        ByteBuffer byteBuffer = ByteBuffer.allocate(data.length * 4);
        IntBuffer intBuffer = byteBuffer.asIntBuffer();
        intBuffer.put(data);
        byte[] array = byteBuffer.array();
        return  array;
    }

    static void encodeYUV420SP(byte[] yuv420sp, int[] argb, int width, int height)
    {
        final int frameSize = width * height;

        int yIndex = 0;
        int uvIndex = frameSize;

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {

                a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
                U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
                V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                yuv420sp[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    yuv420sp[uvIndex++] = (byte)((V<0) ? 0 : ((V > 255) ? 255 : V));
                    yuv420sp[uvIndex++] = (byte)((U<0) ? 0 : ((U > 255) ? 255 : U));
                }

                index ++;
            }
        }
    }
}
