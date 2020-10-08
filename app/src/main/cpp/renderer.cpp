#include "renderer.h"
#include <EGL/egl.h>
#include <android/native_window.h>

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::start() {

}

void Renderer::stop() {
    destory();
}

void Renderer::setWindow(ANativeWindow *window) {
    _window = window;

    initialize();
}

bool Renderer::initialize() {

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };


    EGLint major = 0;
    EGLint minor = 0;
    EGLConfig config = nullptr;
    EGLint numConfigs = 0;
    EGLint format = 0;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    EGLint width = 0;
    EGLint height = 0;

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
        goto cleanup;

    if (!eglInitialize(display, &major, &minor))
        goto cleanup;

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
        goto cleanup;

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
        goto cleanup;

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, nullptr)))
        goto cleanup;

    if (!(context = eglCreateContext(display, config, 0, nullptr)))
        goto cleanup;

    if (!eglMakeCurrent(display, surface, surface, context))
        goto cleanup;

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        goto cleanup;
    }

    _display = display;
    _surface = surface;
    _context = context;

    return true;

cleanup:

    EGLint error = eglGetError();

    if (context) {
        eglDestroyContext(display, context);
    }
    if (surface) {
        eglDestroySurface(display, surface);
    }
    if (display) {
        eglTerminate(display);
    }
    return false;
}

void Renderer::destory() {
    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (_context) {
        eglDestroyContext(_display, _context);
    }
    if (_surface) {
        eglDestroySurface(_display, _surface);
    }
    if (_display) {
        eglTerminate(_display);
    }

    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;
}



