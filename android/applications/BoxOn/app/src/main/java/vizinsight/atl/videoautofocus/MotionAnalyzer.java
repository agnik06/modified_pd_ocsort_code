package vizinsight.atl.videoautofocus;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class MotionAnalyzer {

    private Context mContext;

    private static SensorManager sensorService;
    private Sensor sensor;
    private SensorEventListener mySensorEventListener;

    private double deviceAccelerationThreshold = 0.2;
    private boolean isDeviceInMotion = false;

    public MotionAnalyzer(Context context){
        mContext = context;

        sensorService = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        sensor = sensorService.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mySensorEventListener = new SensorEventListener() {
            @Override
            public void onAccuracyChanged(Sensor sensor, int accuracy) {
            }
            @Override
            public void onSensorChanged(SensorEvent event) {
                double device_acceleration = Math.abs(Math.sqrt(
                          Math.pow(event.values[0],2)
                        + Math.pow(event.values[1],2)
                        + Math.pow(event.values[2],2))
                        - 9.8);

                isDeviceInMotion = device_acceleration > deviceAccelerationThreshold;
            }
        };
        sensorService.registerListener(mySensorEventListener, sensor,
                SensorManager.SENSOR_DELAY_UI);
    }

    public boolean isInMotion(){
        return isDeviceInMotion;
    }

    public void release(){
        sensorService.unregisterListener(mySensorEventListener);
    }
}
