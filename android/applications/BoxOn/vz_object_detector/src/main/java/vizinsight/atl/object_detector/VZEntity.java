package vizinsight.atl.object_detector;

import android.graphics.Rect;

public class VZEntity {
    public String tag;
    public int tagId, ID;
    public int left, top, right, bottom;
    public float score;
    public int trackId;
    public int associationId;


    public VZEntity(String tag, int width, int height, int left, int top, int right, int bottom,float score,int trackid) {
        this.tag = tag;
        this.tagId = width;
        this.ID = height;
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.score = ((float)(int)(score*100))/100;
        this.trackId = trackid;
    }

    public Rect getRegion()
    {
        return new Rect(top,left,bottom,right);
    }
    public void setRegion(Rect r){

    }




}
