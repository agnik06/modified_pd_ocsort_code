package vizinsight.atl.videoautofocus;

import android.net.Uri;
import android.os.Bundle;
import android.widget.MediaController;
import android.widget.VideoView;


import androidx.appcompat.app.AppCompatActivity;

public class VideoPlayActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.video_play_activity);

        VideoView originalVideoView =(VideoView)findViewById(R.id.original_video_videoview);
        VideoView trackVideoView = (VideoView) findViewById(R.id.track_video_videoview);

        String originalVideoUriString = getIntent().getStringExtra("originalVideoPath");
        String trackVideoUriString = getIntent().getStringExtra("trackVideoPath");

        Uri originalVideoUri=Uri.parse(originalVideoUriString);
        Uri trackVideoUri=Uri.parse(trackVideoUriString);


        MediaController originalVideoMediaController= new MediaController(this);
        originalVideoMediaController.setAnchorView(originalVideoView);

        MediaController trackVideoMediaController= new MediaController(this);
        trackVideoMediaController.setAnchorView(trackVideoView);

        originalVideoView.setMediaController(originalVideoMediaController);
        originalVideoView.setVideoURI(originalVideoUri);
        originalVideoView.requestFocus();
        originalVideoView.start();

        trackVideoView.setMediaController(trackVideoMediaController);
        trackVideoView.setVideoURI(trackVideoUri);
        trackVideoView.requestFocus();
        trackVideoView.start();
    }


}