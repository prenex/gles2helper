#ifndef __GLES_2_HELPER_H
#define __GLES_2_HELPER_H
/* *******************************************************************
 * Simple header-only library for setting up an opengl (es2) window. *
 * Uses either EGL or GLUT, choosable in compile time				 *
 *																	 *
 * Created by: prenex												 *
 * LICENCE: The UNLICENCE											 *
 * ******************************************************************/

/* TODO list:
 * - GLUT (there is no GLUT implementation at all now!)
 * - Mouse events
 * - Different color depths
 * - Fullscreen support
 * - etc.
 */

/* ****************** */
/* WORKING MODE SETUP */
/* ****************** */

/* Possible modes */
/*#define USE_FULL_GL 0/1 */
/*#define GLES2_HELPER_USE_EGL 1*/
/*#define GLES2_HELPER_USE_GLUT 1*/

/* EGL is priority when both is defined */
#ifdef GLES2_HELPER_USE_EGL
	#ifdef GLES2_HELPER_USE_GLUT
		#undef GLES2_HELPER_USE_GLUT
	#endif
#endif
/* EGL is default when nothing is given */
#ifndef GLES2_HELPER_USE_EGL
	#ifndef GLES2_HELPER_USE_GLUT
		#define GLES2_HELPER_USE_EGL
	#endif
#endif

/* ******** */
/* INCLUDES */
/* ******** */

/* Include OpenGL and GLU itself */
#if USE_FULL_GL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <Glut/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif /* __APPLE__ */
#else /* !USE_FULL_GL */
#include <GLES2/gl2.h>	/* use OpenGL ES 2.x */
#endif /* USE_FULL_GL */

/* Includes depend on the mode choice */
/* EGL */
#ifdef GLES2_HELPER_USE_EGL
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <EGL/egl.h>
#endif /* GLES2_HELPER_USE_EGL */
/* GLUT */
#ifdef GLES2_HELPER_USE_GLUT
#ifdef __APPLE__
#include <Glut/glut.h>
#else
#include <GL/glut.h>
#endif /*__APPLE__ */
#endif /* GLES2_HELPER_USE_GLUT */

/* Normal includes */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/* *********************** */
/* Keyevent handler values */
/* *********************** */

/* Fields */
#define KEYEVENT_IS_SPECIAL 1 /* Special key? */
#define KEYEVENT_ONPRESS 2		/* Keypress event */
#define KEYEVENT_ONRELEASE 4	/* Keyrelease event */

/* Special keys */
/* GLUT mapping */
#ifdef GLES2_HELPER_USE_GLUT
#define GLES2H_F2 GLUT_KEY_F2
#define GLES2H_F3 GLUT_KEY_F3
#define GLES2H_F4 GLUT_KEY_F4
#define GLES2H_F5 GLUT_KEY_F5
#define GLES2H_F6 GLUT_KEY_F6
#define GLES2H_F7 GLUT_KEY_F7
#define GLES2H_F8 GLUT_KEY_F8
#define GLES2H_F9 GLUT_KEY_F9
#define GLES2H_F10 GLUT_KEY_F10
#define GLES2H_F11 GLUT_KEY_F11
#define GLES2H_F12 GLUT_KEY_F12
#define GLES2H_LEFT GLUT_KEY_LEFT
#define GLES2H_UP GLUT_KEY_UP
#define GLES2H_RIGHT GLUT_KEY_RIGHT
#define GLES2H_DOWN GLUT_KEY_DOWN
#define GLES2H_PAGE_UP GLUT_KEY_PAGE_UP
#define GLES2H_PAGE_DOWN GLUT_KEY_PAGE_DOWN
#define GLES2H_HOME GLUT_KEY_HOME
#define GLES2H_END GLUT_KEY_END
#define GLES2H_INSERT GLUT_KEY_INSERT
#endif /* GLES2_HELPER_USE_GLUT */
/* EGL mapping */
#ifdef GLES2_HELPER_USE_EGL
#define GLES2H_F2 XK_F2
#define GLES2H_F3 XK_F3
#define GLES2H_F4 XK_F4
#define GLES2H_F5 XK_F5
#define GLES2H_F6 XK_F6
#define GLES2H_F7 XK_F7
#define GLES2H_F8 XK_F8
#define GLES2H_F9 XK_F9
#define GLES2H_F10 XK_F10
#define GLES2H_F11 XK_F11
#define GLES2H_F12 XK_F12
#define GLES2H_LEFT XK_Left
#define GLES2H_UP XK_Up
#define GLES2H_RIGHT XK_Right
#define GLES2H_DOWN XK_Down
#define GLES2H_PAGE_UP XK_Page_Up
#define GLES2H_PAGE_DOWN XK_Page_Down
#define GLES2H_HOME XK_Home
#define GLES2H_END XK_End
#define GLES2H_INSERT XK_Insert
#endif /* GLES2_HELPER_USE_EGL */

/* ********* */
/* Functions */
/* ********* */

/* EGL */
/* *** */

#ifdef GLES2_HELPER_USE_EGL
/**
 * Create an RGB, double-buffered X window.
 * Return the window and context handles.
 *
 * @returns Non-zero (for ex. -1) on errors, 0 otherwise
 */
static int make_x_window(Display *x_dpy, EGLDisplay egl_dpy,
				const char *name,
				int x, int y, int width, int height,
				Window *winRet,
				EGLContext *ctxRet,
				EGLSurface *surfRet) {
	static const EGLint attribs[] = {
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_DEPTH_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
#if USE_FULL_GL
	static const EGLint ctx_attribs[] = {
		EGL_NONE
	};
#else
	static const EGLint ctx_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
#endif

	int scrnum;
	XSetWindowAttributes attr;
	unsigned long mask;
	Window root;
	Window win;
	XVisualInfo *visInfo, visTemplate;
	int num_visuals;
	EGLContext ctx;
	EGLConfig config;
	EGLint num_configs;
	EGLint vid;

	scrnum = DefaultScreen( x_dpy );
	root = RootWindow( x_dpy, scrnum );

	if (!eglChooseConfig( egl_dpy, attribs, &config, 1, &num_configs)) {
		printf("Error: couldn't get an EGL visual config\n");
		return 1;
	}

	assert(config);
	assert(num_configs > 0);

	if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
		printf("Error: eglGetConfigAttrib() failed\n");
		return(1);
	}

	/* The X window visual must match the EGL config */
	visTemplate.visualid = vid;
	visInfo = XGetVisualInfo(x_dpy, VisualIDMask, &visTemplate, &num_visuals);
	if (!visInfo) {
		printf("Error: couldn't get X visual\n");
		return 1;
	}

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap( x_dpy, root, visInfo->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateWindow( x_dpy, root, 0, 0, width, height,
				0, visInfo->depth, InputOutput,
				visInfo->visual, mask, &attr );

	/* set hints and properties */
	{
		XSizeHints sizehints;
		sizehints.x = x;
		sizehints.y = y;
		sizehints.width  = width;
		sizehints.height = height;
		sizehints.flags = USSize | USPosition;
		XSetNormalHints(x_dpy, win, &sizehints);
		XSetStandardProperties(x_dpy, win, name, name,
								None, (char **)NULL, 0, &sizehints);
	}

#if USE_FULL_GL /* XXX fix this when eglBindAPI() works */
	eglBindAPI(EGL_OPENGL_API);
#else
	eglBindAPI(EGL_OPENGL_ES_API);
#endif

	ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs );
	if (!ctx) {
		printf("Error: eglCreateContext failed\n");
		return 1;
	}

#if !USE_FULL_GL
	/* test eglQueryContext() */
	{
		EGLint val;
		eglQueryContext(egl_dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &val);
		assert(val == 2);
	}
#endif

	*surfRet = eglCreateWindowSurface(egl_dpy, config, win, NULL);
	if (!*surfRet) {
		printf("Error: eglCreateWindowSurface failed\n");
		return 1;
	}

	/* sanity checks */
	{
		EGLint val;
		eglQuerySurface(egl_dpy, *surfRet, EGL_WIDTH, &val);
		assert(val == width);
		eglQuerySurface(egl_dpy, *surfRet, EGL_HEIGHT, &val);
		assert(val == height);
		assert(eglGetConfigAttrib(egl_dpy, config, EGL_SURFACE_TYPE, &val));
		assert(val & EGL_WINDOW_BIT);
	}

	XFree(visInfo);

	*winRet = win;
	*ctxRet = ctx;
}

/**
 * The EGL event loop handler
 *
 * @param draw User-specified draw function
 * @param reshape User-specified reshape-resize(width, height) function
 * @param idle User-specified idle function
 * @param keyevent User-specified key handler(code, fields) function. See KEYEVENT_* defines for bit fields.
 * @param dpy The display ptr
 * @param win The window
 * @param egl_dpy The EGL display
 * @param egl_surf The EGL surface
 *
 * returns Only returns when the event loop is ending. That case it returns the proposed app return value.
 */
static int event_loop(
		void (*draw)(), /* User-specified draw function */
		void (*reshape)(int, int), /* The reshape-resize function */
		void (*idle)(), /* User-specified idle function */
		int (*keyevent)(int, int), /* User-specified key handler(code, fields) */
		Display *dpy, Window win, EGLDisplay egl_dpy,
		EGLSurface egl_surf) {
	while (1) {
		int redraw = 0;
		XEvent event;

		XNextEvent(dpy, &event);

		switch (event.type) {
			case Expose:
			 redraw = 1;
			 break;
			case ConfigureNotify:
			 /* Call user-defined reshape */
			 reshape(event.xconfigure.width, event.xconfigure.height);
			 break;
			case KeyPress:
			case KeyRelease:
			{
				/* Handle case if they are not interested in keys */
				if(keyevent == NULL) {
					break;
				}

				/* Handle keys */
				char ascii_buffer[10];
				int r, code, fields, ret;
				code = XLookupKeysym(&event.xkey, 0);
				if(event.type == KeyPress) {
					fields = KEYEVENT_ONPRESS;
				} else {
					fields = KEYEVENT_ONRELEASE;
				}
				/* FIXME: either optimize this or at least use X-macros to remove duplication of code listing! */
				switch(code) {
					case GLES2H_F2:
					case GLES2H_F3:
					case GLES2H_F4:
					case GLES2H_F5:
					case GLES2H_F6:
					case GLES2H_F7:
					case GLES2H_F8:
					case GLES2H_F9:
					case GLES2H_F10:
					case GLES2H_F11:
					case GLES2H_F12:
					case GLES2H_LEFT:
					case GLES2H_UP:
					case GLES2H_RIGHT:
					case GLES2H_DOWN:
					case GLES2H_PAGE_UP:
					case GLES2H_PAGE_DOWN:
					case GLES2H_HOME:
					case GLES2H_END:
					case GLES2H_INSERT:
						fields |= KEYEVENT_IS_SPECIAL;
						ret = keyevent(code, fields);
						if(ret) {
							return ret-1; /* Exit program */
						}
						break;
					default:
						r = XLookupString(&event.xkey, ascii_buffer, sizeof(ascii_buffer),
									 NULL, NULL);
						ret = keyevent(ascii_buffer[0], fields);
						if(ret) {
							return ret-1; /* Exit program */
						}
						break;
				}	

				/* Expect to redraw when there is some keyboard even! */
				redraw = 1;
				break;
			}
		default:
			if(idle != NULL) {
				idle(); /* User-defined idle fun or NO-OP */
			}
			break;
		}

		if (redraw) {
		 draw(); /* User-defined draw func */
		 eglSwapBuffers(egl_dpy, egl_surf); /* swap */
		}
	}
}

/**
 * Initializes and runs an OpenGL (ES2) window with the given parameters.
 * This creates the event loops calling your specified callbacks.
 *
 * @param init User-specified init function. Will be called before any draw() calls! Cannot be NULL!
 * @param draw User-specified draw function. Cannot be NULL!
 * @param reshape User-specified reshape-resize(width, height) function. Cannot be NULL!
 * @param idle User-specified idle function. Can be NULL!
 * @param keyevent User-specified key handler(code, fields) function. See KEYEVENT_* defines for bit fields. Can be NULL!
 * @param title The window title
 * @param width The width
 * @param height The height
 * @param printInfo When non-zero print more debug information to console.
 * @param dpyName In case it it not NULL, the DISPLAY variable value to use (other monitors)
 *
 * @return The proposed return value of the application where -1 indicates setup issues, 0 success and other values some later errors.
 */
int gles2run(
		void (*init)(), /* User-specified init function */
		void (*draw)(), /* User-specified draw function */
		void (*reshape)(int, int), /* The reshape-resize function */
		void (*idle)(), /* User-specified idle function */
		int (*keyevent)(int, int), /* User-specified key handler(code, fields) */
		const char *title, int width, int height, GLboolean printInfo, char *dpyName) {
	Display *x_dpy;
	Window win;
	EGLSurface egl_surf;
	EGLContext egl_ctx;
	EGLDisplay egl_dpy;
	EGLint egl_major, egl_minor;
	const char *s;
	int retval = 0;

	x_dpy = XOpenDisplay(dpyName);
	if (!x_dpy) {
		fprintf(stderr, "Error: couldn't open display %s\n",
		 dpyName ? dpyName : getenv("DISPLAY"));
		return -1;
	}

	egl_dpy = eglGetDisplay(x_dpy);
	if (!egl_dpy) {
		fprintf(stderr, "Error: eglGetDisplay() failed\n");
		return -1;
	}

	if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
		fprintf(stderr, "Error: eglInitialize() failed\n");
		return -1;
	}

	s = eglQueryString(egl_dpy, EGL_VERSION);
	if(printInfo) printf("EGL_VERSION = %s\n", s);

	s = eglQueryString(egl_dpy, EGL_VENDOR);
	if(printInfo) printf("EGL_VENDOR = %s\n", s);

	s = eglQueryString(egl_dpy, EGL_EXTENSIONS);
	if(printInfo) printf("EGL_EXTENSIONS = %s\n", s);

	s = eglQueryString(egl_dpy, EGL_CLIENT_APIS);
	if(printInfo) printf("EGL_CLIENT_APIS = %s\n", s);

	if(!make_x_window(
			x_dpy, egl_dpy,
			title, 0, 0, width, height,
			&win, &egl_ctx, &egl_surf)) {
		fprintf(stderr, "Failed to make window!\n");
		return -1;
	}

	XMapWindow(x_dpy, win);
	if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx)) {
		fprintf(stderr, "Error: eglMakeCurrent() failed\n");
		return -1;
	}

	if (printInfo) {
		printf("GL_RENDERER	= %s\n", (char *) glGetString(GL_RENDERER));
		printf("GL_VERSION	= %s\n", (char *) glGetString(GL_VERSION));
		printf("GL_VENDOR		= %s\n", (char *) glGetString(GL_VENDOR));
		printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
	}

	init();

	/*
	 * Set initial projection/viewing transformation.
	 * We can't be sure we'll get a ConfigureNotify event when the window
	 * first appears.
	 */
	reshape(width, height);

	/* Exits on keyboard or other event handler callbacks */
	/* Otherwise loops "forever" basically */
	retval = event_loop(draw, reshape, idle, keyevent, x_dpy, win, egl_dpy, egl_surf);

	/* Cleanup*/
	eglDestroyContext(egl_dpy, egl_ctx);
	eglDestroySurface(egl_dpy, egl_surf);
	eglTerminate(egl_dpy);

	XDestroyWindow(x_dpy, win);
	XCloseDisplay(x_dpy);

	/* EXIT */
	return retval;
}
#endif /* GLES2_HELPER_USE_EGL */


/* GLUT */
/* **** */

#ifdef GLES2_HELPER_USE_GLUT
/* TODO: GLUT implementation */
#endif /* GLES2_HELPER_USE_GLUT */

#endif /* __GLES_2_HELPER_H */
/* vim: set ts=4 sw=4 tw=0 noet : */
