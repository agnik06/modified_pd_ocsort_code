package vizinsight.atl.videoautofocus.ui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;

public class ToggleImageButton extends ImageButton{
    Context mContext;
    public ToggleImageButton(Context context) {
        super(context);
        mContext =  context;
        initOnTouchChanges();
    }
    public ToggleImageButton(Context context, AttributeSet attrs){
        super(context, attrs);
        mContext =  context;
        initOnTouchChanges();
    }

    OnTouchListener onTouchListener = null;

    @Override
    public void setOnTouchListener(OnTouchListener l) {
        onTouchListener = l;
//        super.setOnTouchListener(l);
    }

    public class ToggleTouchListener implements OnTouchListener{

        public ToggleTouchListener(){
            super();
        }
        float originalScaleX;
        float originalScaleY;
        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            int action = motionEvent.getAction();
            if(action == MotionEvent.ACTION_DOWN){
                originalScaleX = view.getScaleX();
                originalScaleY = view.getScaleY();
                view.setScaleX(originalScaleX * 0.90f);
                view.setScaleY(originalScaleY * 0.90f);
            }
            else if (action == MotionEvent.ACTION_UP){
                view.setScaleX(originalScaleX);
                view.setScaleY(originalScaleY);
            }

            if(onTouchListener != null){
                onTouchListener.onTouch(view, motionEvent);
            }
            return false;
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    private void initOnTouchChanges() {
        super.setOnTouchListener(new ToggleTouchListener());
    }

}
