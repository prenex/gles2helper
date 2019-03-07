#ifndef __GLES_2_HELPER_H
#define __GLES_2_HELPER_H
/* *******************************************************************
 * Simple header-only library for setting up an opengl (es2) window. *
 * Uses either EGL or GLUT, choosable in compile time                *
 *                                                                   *
 * Created by: prenex                                                *
 * LICENCE: The UNLICENCE                                            *
 * ******************************************************************/

/* TODO list:
 * - GLUT (there is no GLUT implementation at all now!)
 * - Mouse events
 * - Different color depths
 * - Fullscreen support
 * - Internationalized key support and testing
 * - etc.
 */

/* ****************** */
/* WORKING MODE SETUP */
/* ****************** */

/* Possible modes */
/*#define USE_FULL_GL 0/1 */
/*#define GLES2_HELPER_USE_EGL*/
/*#define GLES2_HELPER_USE_GLUT*/
/*#define GLES2_H_USE_AUTOREPEAT_HACKZ_DETECTION*/ /* Beware with this: broken hackz on many-many machines */

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

#if USE_FULL_GL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <Glut/glu.h>
#else
#include <GL/glew.h>
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
#include <X11/XKBlib.h>
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
#include <string.h>

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

	if (!eglChooseConfig(egl_dpy, attribs, &config, 1, &num_configs)) {
		fprintf(stderr, "Error: couldn't get an EGL visual config\n");
		return 1;
	}

	assert(config);
	assert(num_configs > 0);

	if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
		fprintf(stderr, "Error: eglGetConfigAttrib() failed\n");
		return(1);
	}

	/* The X window visual must match the EGL config */
	visTemplate.visualid = vid;
	visInfo = XGetVisualInfo(x_dpy, VisualIDMask, &visTemplate, &num_visuals);
	if (!visInfo) {
		fprintf(stderr, "Error: couldn't get X visual\n");
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
		fprintf(stderr, "Error: eglCreateContext failed\n");
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
		fprintf(stderr, "Error: eglCreateWindowSurface failed\n");
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
 * @param drawUpdate User-specified drawUpdate function, its parameter is true when redraw_hint is hinted!
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
/* Rem.: Some ideas here are mine completely, some are from godot engine
       See https://github.com/godotengine/godot/blob/master/platform/x11/os_x11.cpp*/
static int event_loop(
		int (*drawUpdate)(int), /* User-specified drawUpdate function */
		void (*reshape)(int, int), /* The reshape-resize function */
		void (*idle)(), /* User-specified idle function */
		int (*keyevent)(int, int), /* User-specified key handler(code, fields) */
		Display *dpy, Window win, EGLDisplay egl_dpy,
		EGLSurface egl_surf) {
	while (1) {
		/** To avoid blocking, only see if there is pending events */
		int redraw_hint = 0;
		if (XPending(dpy) > 0) {
			XEvent event;

			XNextEvent(dpy, &event);

			switch (event.type) {
				case Expose:
				 redraw_hint = 1;
				 break;
				case ConfigureNotify:
				 /* Call user-defined reshape */
				 reshape(event.xconfigure.width, event.xconfigure.height);
				 break;
				case KeyRelease:
				/* We just fall through to KeyPress except if we do HACKZ... */
#ifdef GLES2_H_USE_AUTOREPEAT_HACKZ_DETECTION
				/*
				 * When repeat is on, X can send a release immediately
				 * after the keypress when the repeat time is reached.
				 * This makes it impossible for out clients to tell 
				 * when a real, physical button release was, because 
				 * there are PRE-RE, PRE-RE pairs from that on. With 
				 * the following "trick" or "hackz", we can remove the 
				 * releases coming only from the repeat, but we only
				 * need to do so except if we are using the function
				 * XkbSetDetectableAutoRepeat() specifically for this.
				 *
				 * This seems to be broken on many-many machines, so
				 * we only do this instead of XkbSetDetectableAutoRepeat
				 * when the user especially asks for this solution!
				 *
				 * See: https://stackoverflow.com/questions/2100654/ignore-auto-repeat-in-x11-applications
				 */
				if(keyevent != NULL) {
					if(event.type == KeyRelease && XEventsQueued(dpy, QueuedAfterReading)) {
						XEvent nev;

						XPeekEvent(dpy, &nev);

						if (nev.type == KeyPress && nev.xkey.time == event.xkey.time &&
							nev.xkey.keycode == event.xkey.keycode) {
							/* Key wasn’t actually released */
							/* Ignore it with a break */
							break;
						}
					}
				}
#endif /* GLES2_H_USE_AUTOREPEAT_HACKZ_DETECTION */
				case KeyPress:
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

					/* Expect to redraw_hint when there is some keyboard even! */
					redraw_hint = 1;
					break;
				}
			default:
				if(idle != NULL) {
					idle(); /* User-defined idle fun or NO-OP */
				}
				break;
			}
		}

		int redraw = drawUpdate(redraw_hint); /* User-defined drawUpdate func */
		if (redraw) {
			eglSwapBuffers(egl_dpy, egl_surf); /* swap */
		}
	}
}

/**
 * Initializes and runs an OpenGL (ES2) window with the given parameters.
 * This creates the event loops calling your specified callbacks.
 *
 * Remark(keyevent): The fields parameter tells you if the key is special or ascii,
 *                   also tells if this is keypress or keyrelease. Multiple keydowns
 *                   might be sent before a keyrelease, but always in order and with 
 *                   only a single keyrelease. You can create your own logic using 
 *                   this both for games and for normal applications!
 *
 * @param init User-specified init function. Will be called before any drawUpdate() calls! Cannot be NULL!
 * @param drawUpdate User-specified drawUpdate function, its parameter is true when redraw_hint is hinted!
 * @param reshape User-specified reshape-resize(width, height) function. Cannot be NULL!
 * @param idle User-specified idle function. Can be NULL!
 * @param keyevent User-specified key handler(code, fields) function. See KEYEVENT_* #defines for bit fields. Can be NULL!
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
		int (*drawUpdate)(int), /* User-specified drawUpdate function */
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
	Bool selectable_repeat_success = 0;
	int retval = 0;

	x_dpy = XOpenDisplay(dpyName);
	if (!x_dpy) {
		fprintf(stderr, "Error: couldn't open display %s\n",
		 dpyName ? dpyName : getenv("DISPLAY"));
		return -1;
	}

#ifndef GLES2_H_USE_AUTOREPEAT_HACKZ_DETECTION
	XkbSetDetectableAutoRepeat(x_dpy, true, &selectable_repeat_success);
	if(!selectable_repeat_success) {
		fprintf(stderr, "Error: XkbSetDetectableAutoRepeat cannot set true! Try rebuilding with HACKZ!\n");
	}
#endif /* GLES2_H_USE_AUTOREPEAT_HACKZ_DETECTION */

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
			(title != NULL) ? title : "gles2run", 0, 0, width, height,
			&win, &egl_ctx, &egl_surf)) {
		fprintf(stderr, "Failed to make window!\n");
		return -1;
	}

	/*
	 * THIS IS NECESSARY OTHERWISE WE MIGHT NOT GET SOME EVENTS!
	 * We want the keyboard and mouse events and the exposure (resize) and configureNotify
	 *
	 * See: https://tronche.com/gui/x/xlib/events/processing-overview.html
	 *      https://tronche.com/gui/x/xlib-tutorial/2nd-program-anatomy.html
	 *      https://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html
	 */
	XSelectInput(x_dpy, win,
			KeyPressMask | KeyReleaseMask | /* Key */
			ButtonPressMask | ButtonReleaseMask | PointerMotionMask | /* Mouse */
			ExposureMask | StructureNotifyMask); /* Redraw and resize */

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
	retval = event_loop(drawUpdate, reshape, idle, keyevent, x_dpy, win, egl_dpy, egl_surf);

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
/* TODO: In case of GLUT, hax this: http://www.dei.isep.ipp.pt/~matos/cg/docs/manual/glutSetKeyRepeat.3GLUT.html*/
/* We need both keyboardFunc(..) and keyboardUpFunc(..) alongside glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); */
/* See: https://stackoverflow.com/questions/39561997/glut-holding-a-key-down */

static int (*drawUpdateFun)(int);
static int (*keyeventFun)(int, int);

static void glut_draw() {
	/* Call the users drawUpdate function */
	int redraw = drawUpdateFun(true);

	/* Render the scene if asked to */
	if(redraw) {
		glutSwapBuffers();
	}
}

/* SPECIAL KEY PRESSES */
/* ................... */

/* Rem.: we do not need to change or map the code as it will be properly #defined compile time! */
static void glut_special(int code, int mouse_x_unused, int mouse_y_unused) {
	int fields = KEYEVENT_IS_SPECIAL | KEYEVENT_ONPRESS;
	/* Call user code twice*/
	int ret = keyeventFun(code, fields);
	/* Handling for app exiting */
	if(ret) {
		exit(ret - 1);
	}
}
/* Rem.: we do not need to change or map the code as it will be properly #defined compile time! */
static void glut_special_up(int code, int mouse_x_unused, int mouse_y_unused) {
	int fields = KEYEVENT_IS_SPECIAL | KEYEVENT_ONRELEASE;
	/* Call user code twice*/
	int ret = keyeventFun(code, fields);
	/* Handling for app exiting */
	if(ret) {
		exit(ret - 1);
	}
}

/* NORMAL KEY PRESSES */
/* .................. */

static void glut_key(unsigned char code, int mouse_x_unused, int mouse_y_unused) {
	int fields = KEYEVENT_ONPRESS;
	/* Call user code */
	int ret = keyeventFun(code, fields);
	/* Handling for app exiting */
	if(ret) {
		exit(ret - 1);
	}
}

static void glut_keyup(unsigned char code, int mouse_x_unused, int mouse_y_unused) {
	int fields = KEYEVENT_ONRELEASE;
	/* Call user code */
	int ret = keyeventFun(code, fields);
	/* Handling for app exiting */
	if(ret) {
		exit(ret - 1);
	}
}

/** ...Hackz you do not wanna know... */
#define ARGV_HACKZ_LEN (10+1)
static char ARGV_HACKZ[ARGV_HACKZ_LEN];

/**
 * Initializes and runs an OpenGL (ES2) window with the given parameters.
 * This creates the event loops calling your specified callbacks.
 *
 * Remark(keyevent): The fields parameter tells you if the key is special or ascii,
 *                   also tells if this is keypress or keyrelease. Multiple keydowns
 *                   might be sent before a keyrelease, but always in order and with 
 *                   only a single keyrelease. You can create your own logic using 
 *                   this both for games and for normal applications!
 *
 * @param init User-specified init function. Will be called before any drawUpdate() calls! Cannot be NULL!
 * @param drawUpdate User-specified drawUpdate function, its parameter is true when redraw_hint is hinted!
 * @param reshape User-specified reshape-resize(width, height) function. Cannot be NULL!
 * @param idle User-specified idle function. Can be NULL!
 * @param keyevent User-specified key handler(code, fields) function. See KEYEVENT_* #defines for bit fields. Can be NULL!
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
		int (*drawUpdate)(int), /* User-specified drawUpdate function */
		void (*reshape)(int, int), /* The reshape-resize function */
		void (*idle)(), /* User-specified idle function */
		int (*keyevent)(int, int), /* User-specified key handler(code, fields) */
		const char *title, int width, int height, GLboolean printInfo, char *dpyName) {

	strncpy(ARGV_HACKZ, "GLUT HACKZ", ARGV_HACKZ_LEN);
	char* hackz = ARGV_HACKZ;
	int hackzc = 1;
	glutInit(&hackzc, &hackz); /* Hope for this to work... argc = 0; argv = NULL */
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow(title);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); /* Keys are much better off this way */

	/* Set up glut callback functions */
	glutIdleFunc(idle); /* This is the same as ours! */
	glutReshapeFunc(reshape); /* This is the same as ours! */
	drawUpdateFun = drawUpdate; /* Must save this to be visible from callbacks */
	glutDisplayFunc(glut_draw); /* CUSTOM */
	keyeventFun = keyevent; /* Must save this to be visible from callbacks */
	glutSpecialFunc(glut_special); /* CUSTOM */
	glutSpecialUpFunc(glut_special_up); /* CUSTOM */
	glutKeyboardFunc(glut_key); /* CUSTOM */
	glutKeyboardUpFunc(glut_keyup); /* CUSTOM */

	/* Let glut start its own main loop */
	init();
	glutMainLoop();
}
#endif /* GLES2_HELPER_USE_GLUT */

#endif /* __GLES_2_HELPER_H */
/* vim: set ts=4 sw=4 tw=0 noet : */
