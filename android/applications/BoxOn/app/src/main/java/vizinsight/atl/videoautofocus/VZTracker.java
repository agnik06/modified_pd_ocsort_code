package vizinsight.atl.videoautofocus;

import android.graphics.Rect;
import android.util.Log;

import boofcv.abst.tracker.ConfigTld;
import boofcv.abst.tracker.TrackerObjectQuad;
import boofcv.core.image.ConvertImage;
import boofcv.factory.tracker.FactoryTrackerObjectQuad;
import boofcv.struct.image.ImageType;
import boofcv.struct.image.ImageUInt8;
import boofcv.struct.image.MultiSpectral;
import georegression.struct.shapes.Quadrilateral_F64;
import vizinsight.atl.object_detector.VZEntity;

//in Application's gradle
//dependencies {
//        .....
//        ['recognition','android'].each { String a -> compile group: 'org.boofcv', name: a, version: '0.17' }
//}

public class VZTracker{

    public static int INVISIBLE = 0;
    public static int VISIBLE   = 1;

    private TrackerObjectQuad tracker = null;
    private VZEntity entity = null;
    private Rect init_location = null;
    private String label = null;
    private String parent_label = null;
    public static int OT_scale_factor = 4;
    public Rect current_location = null;
    public int visibility = 0;

    private int invisible_counter = 0;

    static int frame_width  = 480;
    static int frame_height = 640;

    public VZTracker(){
        tracker = FactoryTrackerObjectQuad.tld(new ConfigTld(false),ImageUInt8.class);
    }

    public VZTracker(String label){
        this.setLabel(label);
        //TLD - tracker
        tracker = FactoryTrackerObjectQuad.tld(new ConfigTld(false),ImageUInt8.class);

        //SparseFlow
//        SfotConfig config = new SfotConfig();
//        config.numberOfSamples = 10;
//        config.robustMaxError = 30;
//        tracker = FactoryTrackerObjectQuad.sparseFlow(config,ImageUInt8.class,null);
    }

    public static ImageUInt8 getImageUInt8(byte[] nv21_bytes, int width, int height,boolean rotate){//}, boolean rotate){ TODO: for landscape mode
//        if(rotate){
//            int temp = image_width;
//            image_width = image_height;
//            image_height = temp;
//        }
        MultiSpectral<ImageUInt8> image = Utils.getMultiSpectral8(nv21_bytes, width, height, OT_scale_factor, rotate);
        ImageType<ImageUInt8> imageType = ImageType.single(ImageUInt8.class);
        ImageUInt8 image_uint8 = imageType.createImage(1,1);
//        inputImage.reshape(image_uint8.image_width, image_uint8.image_height);
        if(rotate) {
            image_uint8.reshape(height / OT_scale_factor, width/ OT_scale_factor);
            ConvertImage.average(image,image_uint8);
            frame_width  = height;
            frame_height = width;
        }
        else{
            image_uint8.reshape(width / OT_scale_factor, height / OT_scale_factor);
            ConvertImage.average(image,image_uint8);
            frame_width  = width;
            frame_height = height;
        }
        return image_uint8;
    }

    public static ImageUInt8 getImageUInt8(byte[] nv21_bytes, int width, int height){//}, boolean rotate){ TODO: for landscape mode
        return getImageUInt8(nv21_bytes,width,height,true);
    }
    public void setLabel(String label){
        this.label = label;
        this.parent_label = label;
        Log.e("Split",label.split("\\(")[0]);
        if(label.contains("(")){
            this.parent_label = label.split("\\(")[0];
        }
    }
    public String getLabel(){
        return this.label;
//        return this.parent_label;
    }
    public void initialize(ImageUInt8 image,Rect location){
        if(location.left < 0)               location.left = 0;
        if(location.right > frame_width)    location.left = frame_width-1;
        if(location.top < 0)                location.top = 0;
        if(location.bottom > frame_height)  location.bottom = frame_height-1;

        this.init_location = new Rect(location);
        this.current_location = this.init_location;
        Quadrilateral_F64 qLocation = Utils.getLocationQuad(location, OT_scale_factor);
        if(tracker!=null){
            tracker.initialize(image,qLocation);
        }
    }
    public Rect process(ImageUInt8 image){
        Quadrilateral_F64 location = new Quadrilateral_F64();
        boolean tracker_visible = tracker.process(image,location);
        if(!tracker_visible) {
            if(visibility==INVISIBLE) invisible_counter++;
            visibility = INVISIBLE;
            return null;
        }
        visibility = VISIBLE;
        invisible_counter = 0;
        this.current_location = Utils.getLocationRect(location, OT_scale_factor);
        return current_location;
    }

    public boolean isVisible(){
        if(invisible_counter >= 5)
            return false;
        return true;
    }

    public boolean isSimilar(VZEntity entity) {
        if(current_location == null || entity == null) return false;

        Rect region = entity.getRegion();

        double distance = Math.pow((region.left-current_location.left),2) +
                Math.pow((region.top-current_location.top),2) +
                Math.pow((region.right-current_location.right),2) +
                Math.pow((region.bottom-current_location.bottom),2) ;
        distance = Math.sqrt(distance);

//        Log.e("Distance ",this.label+"-"+entity.getLabel()+" >> "+distance+"");

        if( distance < 250 && (entity.tag.contains(this.parent_label) ) ) return true;
//        if( distance < 250 && (this.label.equals(entity.getLabel())) ) return true;
        if(distance > 150) return false;

        return true;
    }
}
