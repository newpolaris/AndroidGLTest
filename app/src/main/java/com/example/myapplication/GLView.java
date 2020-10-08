package com.example.myapplication;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;

public class GLView extends SurfaceView implements SurfaceHolder.Callback {
    private static String TAG = "GL2JNIView";

    public GLView(Context context) {
        super(context);
        getHolder().addCallback(this);
        setOnClickListener(new OnClickListener() {
            public void onClick(View view) {
                Toast toast = Toast.makeText(getContext(),
                        "This demo combines Java UI and native EGL + OpenGL renderer",
                        Toast.LENGTH_LONG);
                toast.show();
            }});
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        GLJNILib.nativeSurfaceCreated(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int w, int h) {
        GLJNILib.nativeSurfaceChanged(surfaceHolder.getSurface(), format, w, h);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Surface surface = surfaceHolder.getSurface();
        GLJNILib.nativeSurfaceDestroyed();
    }
}
