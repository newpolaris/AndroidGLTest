#ifndef MY_APPLICATION_RENDERER_H
#define MY_APPLICATION_RENDERER_H

#include <EGL/egl.h>

typedef struct ANativeWindow ANativeWindow;

class Renderer
{
public:

    Renderer();
    virtual ~Renderer();

    // Following methods can be called from any thread.
    // They send message to render thread which executes required actions.
    void start();
    void stop();
    void setWindow(ANativeWindow* window);
    void destory();

private:

    bool initialize();

    ANativeWindow* _window = nullptr;
    EGLDisplay _display = EGL_NO_DISPLAY;
    EGLSurface _surface = EGL_NO_SURFACE;
    EGLContext _context = EGL_NO_CONTEXT;
};

#endif //MY_APPLICATION_RENDERER_H
