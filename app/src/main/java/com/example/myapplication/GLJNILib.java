package com.example.myapplication;

import android.view.Surface;

public class GLJNILib {
    static {
        System.loadLibrary("gl2jni");
    }

    public static native void nativeOnStart();
    public static native void nativeOnResume();
    public static native void nativeOnPause();
    public static native void nativeOnStop();

    public static native void nativeSurfaceCreated(Surface surface);
    public static native void nativeSurfaceChanged(Surface surface, int format, int w, int h);
    public static native void nativeSurfaceDestroyed();

}
