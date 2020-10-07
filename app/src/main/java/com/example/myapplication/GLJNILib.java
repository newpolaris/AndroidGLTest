package com.example.myapplication;

public class GLJNILib {
    static {
        System.loadLibrary("gl2jni");
    }

    public static native void init();
    public static native void resize(int width, int height);
    public static native void step();
}
