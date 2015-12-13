package org.wysaid.ndkopenglbackdraw;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.opengl.EGL14;
import android.util.Log;

public class GLHelpFunctions {

	public static native void getGLBackDrawImage(Bitmap bm);

	// use GLES2.0.
	static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
	static int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
	static private int[] version = new int[2];
	static EGLConfig[] configs = new EGLConfig[1];
	static int[] num_config = new int[1];

	@SuppressLint("InlinedApi") static int[] configSpec = { EGL10.EGL_SURFACE_TYPE, EGL10.EGL_PBUFFER_BIT,
			EGL10.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT,
			EGL10.EGL_RED_SIZE, 8, EGL10.EGL_GREEN_SIZE, 8,
			EGL10.EGL_BLUE_SIZE, 8, EGL10.EGL_ALPHA_SIZE, 8, EGL10.EGL_NONE };
	// eglCreatePbufferSurface used this config
	static int attribListPbuffer[] = {
			// The NDK code would never draw to Pbuffer, so it's not neccessary to
			// match anything.
			EGL10.EGL_WIDTH, 32, EGL10.EGL_HEIGHT, 32, EGL10.EGL_NONE };
	static EGL10 mEgl;
	static GL10 gl;
	static javax.microedition.khronos.egl.EGLSurface mEglPBSurface;
	static EGLContext mEglContext;
	static EGLConfig mEglConfig;
	static EGLDisplay mEglDisplay;

	static public void initEGL() {
		mEgl = (EGL10) EGLContext.getEGL();
		mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
		mEgl.eglInitialize(mEglDisplay, version);
		mEgl.eglChooseConfig(mEglDisplay, configSpec, configs, 1, num_config);
		mEglConfig = configs[0];
		mEglContext = mEgl.eglCreateContext(mEglDisplay, mEglConfig,
				EGL10.EGL_NO_CONTEXT, attrib_list);
		if (mEglContext == EGL10.EGL_NO_CONTEXT) {
			Log.d("ERROR:", "eglCreateContext Failed!");
		}
		mEglPBSurface = mEgl.eglCreatePbufferSurface(mEglDisplay, mEglConfig,
				attribListPbuffer);
		if (mEglPBSurface == EGL10.EGL_NO_SURFACE) {			
				Log.d("ERROR:", "eglCreatePbufferSurface Failed!");
		}

		if (!mEgl.eglMakeCurrent(mEglDisplay, mEglPBSurface, mEglPBSurface, mEglContext)) {
			Log.d("ERROR:", "eglMakeCurrent failed:" + mEgl.eglGetError());
		}
		// You can do some works using OpenGL with java code. But this demo would do that within NDK.
		gl = (GL10) mEglContext.getGL();
	}
	
	static public void enableEGL() {
		if (!mEgl.eglMakeCurrent(mEglDisplay, mEglPBSurface, mEglPBSurface,	mEglContext))
		{
			Log.d("ERROR:", "eglMakeCurrent failed:" + mEgl.eglGetError());
		}
	}
}
