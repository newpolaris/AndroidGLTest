#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#include "renderer.h"

static ANativeWindow *window = nullptr;
static Renderer *renderer = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeOnStart(JNIEnv *env, jclass clazz) {
    renderer = new Renderer();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeOnResume(JNIEnv *env, jclass clazz) {
    if (renderer)
        renderer->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeOnPause(JNIEnv *env, jclass clazz) {
    if (renderer)
        renderer->stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeOnStop(JNIEnv *env, jclass clazz) {
    delete renderer;
    renderer = nullptr;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeSurfaceCreated(JNIEnv *env, jclass clazz,
                                                             jobject surface) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeSurfaceChanged(JNIEnv *env, jclass clazz,
                                                             jobject surface, jint format, jint w,
                                                             jint h) {
    if (surface != nullptr) {
        window = ANativeWindow_fromSurface(env, surface);
        if (renderer)
            renderer->setWindow(window);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_nativeSurfaceDestroyed(JNIEnv *env, jclass clazz) {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
}