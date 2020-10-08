package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class GLActivity extends AppCompatActivity {

    GLView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new GLView(getApplication());
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        GLJNILib.nativeOnPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        GLJNILib.nativeOnResume();
    }

    @Override
    protected void onStart() {
        super.onStart();
        GLJNILib.nativeOnStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        GLJNILib.nativeOnStop();
    }
}