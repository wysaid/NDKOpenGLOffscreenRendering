#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef LOG_TAG
#define LOG_TAG "NDKOpenGLBackDraw"
#endif

#define LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define SHADER_STRING(...) #__VA_ARGS__

void printGLString(const char* name, GLenum em)
{
    const char *s = (const char*)glGetString(em);
    LOG_INFO("GL_INFO %s = %s\n", name, s);
}

void checkGLError(const char* op)
{
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        LOG_INFO("after %s() glError (0x%x)\n", op, error);
    }
}

const char* const g_defaultVertexShaderString = SHADER_STRING
(
attribute vec2 vPosition;
varying vec2 textureCoordinate;
void main()
{
   gl_Position = vec4(vPosition, 0.0, 1.0);
   textureCoordinate = (vPosition.xy + 1.0) / 2.0;
}
);

const char* const g_defaultFragmentShaderString = SHADER_STRING
(
precision mediump float;
varying vec2 textureCoordinate;
uniform sampler2D myTexture;
void main()
{
    vec4 textureColor = texture2D(myTexture, textureCoordinate);
    textureColor.rb += textureCoordinate.xy;
    gl_FragColor = textureColor;
}
);

const GLfloat g_vertices[] =
{
	-1.0f, 1.0f,
	1.0f, 1.0f,
	-1.0f, -1.0f,
	1.0f, -1.0f
};

void runBackDraw(char* row, int w, int h)
{
    GLuint texture, renderBuffer, frameBuffer;
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    LOG_INFO("Input Image Texture id %d\n", texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, row);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &frameBuffer);
    glGenRenderbuffers(1, &renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER, renderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("Image Handler initImageFBO failed!\n");        
    }
    else
    {
        glViewport(0, 0, w, h);

        ////////////////////////////////////
        GLuint vsh, fsh, program;
        vsh = glCreateShader(GL_VERTEX_SHADER);
        fsh = glCreateShader(GL_FRAGMENT_SHADER);
        program = glCreateProgram();
        glShaderSource(vsh, 1, (const GLchar**)&g_defaultVertexShaderString, NULL);
        glShaderSource(fsh, 1, (const GLchar**)&g_defaultFragmentShaderString, NULL);
        glCompileShader(vsh);
        glCompileShader(fsh);
        glAttachShader(program, vsh);
        glAttachShader(program, fsh);
        glLinkProgram(program);
        glDeleteShader(vsh);
        glDeleteShader(fsh);
        glUseProgram(program);
        GLuint vPosition = glGetAttribLocation(program, "vPosition");
        checkGLError("glGetAttribLocation");
        LOG_INFO("glGetAttribLocation(\"vPosition\") = %d\n", vPosition);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, g_vertices);
        glEnableVertexAttribArray(vPosition);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, row);
    }

    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &renderBuffer);
}


JNIEXPORT void JNICALL Java_org_wysaid_ndkopenglbackdraw_GLHelpFunctions_getGLBackDrawImage(JNIEnv *env, jclass cls, jobject bitmap)
{
    AndroidBitmapInfo info;
    int w, h, ret;
    char* row;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0)
    {
        LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    LOG_INFO("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d", info.width, info.height, info.stride, info.format, info.flags);

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        LOG_ERROR("Bitmap format is not RGBA_8888 !");
        return;
    }

    w = info.width;
    h = info.height;
    ret = AndroidBitmap_lockPixels(env, bitmap, (void**) &row);

    if (ret < 0)
    {
        LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return ;
    }

    runBackDraw(row, w, h);
    LOG_INFO("unlocking pixels");
    AndroidBitmap_unlockPixels(env, bitmap);
}