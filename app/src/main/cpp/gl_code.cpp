
#include <jni.h>
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *)glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

auto gVertexShader =
        "attribute vec4 vPosition;\n"
        "void main() {\n"
        "  gl_Position = vPosition;\n"
        "}\n";

auto gFragmentShader =
        "precision mediump float;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, nullptr, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, nullptr, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

struct RendererES2
{
    RendererES2();
    virtual ~RendererES2();

    bool init();
    void resize(int w, int h);
    void render();

private:

    void step();
    void draw();

    EGLContext mEglContext;
    GLuint mProgram;
    GLint mPositionHandle;

    float mGray;
    uint64_t mLastFrameNs;
};

RendererES2::RendererES2() :
        mEglContext(eglGetCurrentContext()),
        mProgram(0),
        mPositionHandle(0),
        mGray(0.f),
        mLastFrameNs(0)
{
}

RendererES2::~RendererES2() {
    // it's due to GLSurfaceView's implementation

    /* The destructor may be called after the context has already been
     * destroyed, in which case our objects have already been destroyed.
     *
     * If the context exists, it must be current. This only happens when we're
     * cleaning up after a failed init().
     */
    if (eglGetCurrentContext() != mEglContext)
        return;
    glDeleteProgram(mProgram);
    mProgram = 0;
    mPositionHandle = 0;
}

bool RendererES2::init() {
    mProgram = createProgram(gVertexShader, gFragmentShader);
    if (!mProgram) {
        LOGE("Could not create program.");
        return false;
    }
    mPositionHandle = glGetAttribLocation(mProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n", mPositionHandle);

    return true;
}

void RendererES2::step()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    auto nowNs = now.tv_sec*1000000000ull + now.tv_nsec;

    if (mLastFrameNs > 0) {
        float dt = float(nowNs - mLastFrameNs) * 0.000000001f;
        mGray += dt;
        if (mGray > 1.0f) {
            mGray = 0.0f;
        }
    }
    mLastFrameNs = nowNs;
}
void RendererES2::render()
{
    step();

    glClearColor(mGray, mGray, mGray, 0.8f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    draw();
    checkGlError("Renderer::render");
}

void RendererES2::draw() {
    const GLfloat triangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
                                          0.5f, -0.5f };

    glUseProgram(mProgram);
    checkGlError("glUseProgram");

    glVertexAttribPointer(mPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, triangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(mPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

void RendererES2::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
}

static RendererES2* g_renderer = nullptr;

RendererES2* createES2Renderer() {
    RendererES2* renderer = new RendererES2;
    if (!renderer->init()) {
        delete renderer;
        return nullptr;
    }
    return renderer;
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_myapplication_GLJNILib_init(JNIEnv *env, jclass clazz) {
    if (g_renderer) {
        delete g_renderer;
        g_renderer = nullptr;
    }

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    g_renderer = createES2Renderer();
}

extern "C"
JNIEXPORT void JNICALL Java_com_example_myapplication_GLJNILib_step(JNIEnv *env, jclass clazz) {
    if (g_renderer) {
        g_renderer->render();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_GLJNILib_resize(JNIEnv *env, jclass clazz, jint width, jint height) {
    if (g_renderer)
        g_renderer->resize(width, height);
    LOGI("init(%d, %d)", width, height);
}
