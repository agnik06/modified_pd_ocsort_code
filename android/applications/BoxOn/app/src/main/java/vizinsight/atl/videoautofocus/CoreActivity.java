package vizinsight.atl.videoautofocus;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import vizinsight.atl.object_detector.VZObjectDetector;

public class CoreActivity extends AppCompatActivity {

    Context mContext;
    VZObjectDetector object_detector;

    String detectorModelsPath = "/vendor/etc/saiv/image_understanding/db/";

    String nightModedetectorModelsPath = "/system/saiv/image_understanding/db/";

//    String detectorModelsPath = "/sdcard/saiv/image_understanding/db/image_segmentation/";
//    String detectorModelsPath_scene = "/sdcard/saiv/image_understanding/db/scene_segmentation/";
//    String detectorModelsPath_pet = "/sdcard/saiv/image_understanding/db/pet_segmentation/";

    int odMode = VZObjectDetector.MODE_SALIENCY;
    String defaultModelPath=detectorModelsPath;

    SegmentFilter.Style mBGStyle = SegmentFilter.Style.BLUR;
    SegmentFilter.Style mFGStyle = SegmentFilter.Style.COLOR;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = this;
        Log.d("CoreActivity:","odMode = "+odMode+"  defaultModelPath = "+defaultModelPath);
        object_detector = new VZObjectDetector(odMode, defaultModelPath);
//        object_detector.release();
//        object_detector = new VZObjectDetector(odMode, defaultModelPath);
//        object_detector.release();
//        object_detector = new VZObjectDetector(odMode, defaultModelPath);
    }

    Menu menu;
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        this.menu = menu;
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.core_activity_menu, menu);
        checkMode();
        menu.findItem(R.id.style_blur).setChecked(true);

        return true;
//        return super.onCreateOptionsMenu(menu);
    }

    private void checkMode(){
        int mode = odMode;
        if(object_detector != null){
            mode = object_detector.getMode();
        }
        switch(mode){
            case VZObjectDetector.MODE_SALIENCY:
                menu.findItem(R.id.mode_saliency).setChecked(true);
                break;
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        item.setChecked(true);
        switch(item.getItemId()){
            //Mode Selection
            case R.id.mode_saliency:
                Toast.makeText(this, "Model1 Mode selected", Toast.LENGTH_SHORT).show();
                object_detector.release(odMode);
                object_detector = new VZObjectDetector(VZObjectDetector.MODE_SALIENCY, detectorModelsPath);
                return true;

            // Style Selection
            case R.id.style_greyscale:
                Toast.makeText(this, "Grey-Scale filter selected", Toast.LENGTH_SHORT).show();
                mBGStyle = SegmentFilter.Style.GREY_SCALE;
                mFGStyle = SegmentFilter.Style.COLOR;
                return true;

            case R.id.style_blur:
                Toast.makeText(this, "Blur filter selected", Toast.LENGTH_SHORT).show();
                mBGStyle = SegmentFilter.Style.BLUR;
                mFGStyle = SegmentFilter.Style.COLOR;
                return true;

            case R.id.style_white:
                Toast.makeText(this, "Blur filter selected", Toast.LENGTH_SHORT).show();
                mBGStyle = SegmentFilter.Style.WHITE;
                mFGStyle = SegmentFilter.Style.COLOR;
                return true;

//            case R.id.models_directory_select:
//                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public void showCloseDialog(){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("Do you want to exit?");

        builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                closeActivity();
            }
        });
        builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {

            }
        });
        builder.show();
    }

    public void closeActivity(){
        object_detector.release(odMode);
        super.onBackPressed();
    }
    @Override
    public void onBackPressed() {
        showCloseDialog();
    }

    String info = null;
    public void setInfo(String info){
        this.info = info;
    }
    public void showInfoPopup(MenuItem item) {
        if(item.getItemId() == R.id.info_panel){
            if(info == null)
                return;
            AlertDialog dialog = new AlertDialog.Builder(mContext).create();
//            dialog.setTitle("Info");
            dialog.setMessage(info);
            dialog.show();
        }
    }
}
