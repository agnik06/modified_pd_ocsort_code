package vizinsight.atl.videoautofocus;

import android.os.Bundle;
import android.util.Log;


import androidx.appcompat.app.AppCompatActivity;
import vizinsight.atl.object_detector.VZObjectDetector;


public class NightModeActivity extends AppCompatActivity {
    
    VZObjectDetector object_detector;
    String nightModedetectorModelsPath = "/system/saiv/image_understanding/db/";


    int odMode = VZObjectDetector.MODE_SALIENCY;
    String defaultModelPath=nightModedetectorModelsPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("NightModeActivity :"," onCreate");
        defaultModelPath="/system/saiv/image_understanding/db/";
        odMode = VZObjectDetector.MODE_SALIENCY_SYSTEM;
        super.onCreate(savedInstanceState);
        object_detector = new VZObjectDetector(odMode, defaultModelPath);
    }

    @Override
    protected void onDestroy(){
        object_detector.release(odMode);
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        object_detector.release(odMode);
        super.onBackPressed();
    }


}
