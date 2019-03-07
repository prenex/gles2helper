/**************************************************************************
 * EGL based example code
 *
 * Draw a triangle with X/EGL and OpenGL ES 2.x
 *
 * Build:
 * g++ --std=c++11 egl_example.cpp -o egl_example -lGLESv2 -lEGL -lX11
 **************************************************************************/
/*
#define USE_FULL_GL 1
#define GLES2_HELPER_USE_GLUT // Use glut (USE_FULL_GL is neede for this)
*/

#include "gles2helper.h"
/* Already provided via gles2helper.h (or gl.h or the apple one etc. etc.)
#include <GLES2/gl2.h>
#include <EGL/egl.h>
*/

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define FLOAT_TO_FIXED(X)	((X) * 65535.0)

class GameTime {
private:
	static volatile unsigned long long start_ms;
	unsigned long long last_ms;
	unsigned long long now_ms;
public:
	/** Construct a GameTime object with the difference values */
	GameTime(unsigned long long _last_ms, unsigned long long _now_ms) {
		last_ms = _last_ms;
		now_ms = _now_ms;
	}

	/** Gets milliseconds relative to the last frame. */
	const inline unsigned long long getFrameDiffMs() const {
		return now_ms - last_ms;
	}

	/** Gets milliseconds since the game has started. ONLY WORKS AFTER setStartMs is properly called at app start! */
	const inline unsigned long long getMs() const {
		return now_ms - start_ms;
	}

	/** Saves the start time - needed for any later getMs() calls */
	static inline void setStartMs(unsigned long long ms) {
		start_ms = ms;
	}

	static inline unsigned long long getStartMs() {
		return start_ms;
	}
};
volatile unsigned long long GameTime::start_ms; // Need to be defined!

static GLfloat view_rotx = 0.0, view_roty = 0.0;
static GLint u_matrix = -1;
static GLint attr_pos = 0, attr_color = 1;
static bool left, right, up, down; /* Key status */

static void make_z_rot_matrix(GLfloat angle, GLfloat *m) {
	float c = cos(angle * M_PI / 180.0);
	float s = sin(angle * M_PI / 180.0);
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = m[5] = m[10] = m[15] = 1.0;

	m[0] = c;
	m[1] = s;
	m[4] = -s;
	m[5] = c;
}

static void make_scale_matrix(GLfloat xs, GLfloat ys, GLfloat zs, GLfloat *m) {
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = xs;
	m[5] = ys;
	m[10] = zs;
	m[15] = 1.0;
}


static void mul_matrix(GLfloat *prod, const GLfloat *a, const GLfloat *b) {
#define A(row,col)	a[(col<<2)+row]
#define B(row,col)	b[(col<<2)+row]
#define PROD(row,col)  p[(col<<2)+row]
	GLfloat p[16];
	GLint i;
	for (i = 0; i < 4; i++) {
		const GLfloat ai0=A(i,0),  ai1=A(i,1),	ai2=A(i,2),  ai3=A(i,3);
		PROD(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		PROD(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		PROD(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		PROD(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
	memcpy(prod, p, sizeof(p));
#undef A
#undef B
#undef PROD
}

static void update(GameTime gametime) {
	// Turn the triangle according to the currently pressed keys
	// TODO: Use gametime to limit the speed!
	
	if (left) {
		printf("LEFT\n");
		view_roty += 5.0;
	}
	if (right) {
		printf("RIGHT\n");
		view_roty -= 5.0;
	}
	if (up) {
		printf("UP\n");
		view_rotx += 5.0;
	}
	if (down) {
		printf("DOWN\n");
		view_rotx -= 5.0;
	}
}

static void draw(GameTime gametime) {
	static const GLfloat verts[3][2] = {
		{ -1, -1 },
		{  1, -1 },
		{  0,	1 }
	};
	static const GLfloat colors[3][3] = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 }
	};
	GLfloat mat[16], rot[16], scale[16];

	/* Set modelview/projection matrix */
	make_z_rot_matrix(view_rotx, rot);
	make_scale_matrix(0.5, 0.5, 0.5, scale);
	mul_matrix(mat, rot, scale);
	glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
		glEnableVertexAttribArray(attr_pos);
		glEnableVertexAttribArray(attr_color);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(attr_pos);
		glDisableVertexAttribArray(attr_color);
	}
}

/** Simple single-threaded main loop */
static int drawUpdate(int redraw_hint) {
	unsigned long long now_ms = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()).count();
	static unsigned long long last_ms = -1;
	if(last_ms == (unsigned long long) -1) {
		last_ms = GameTime::getStartMs();
	}

	GameTime gt(last_ms, now_ms);

	update(gt);
	//if(redraw_hint) {
		draw(gt);
		return true;
	//} else return false;
}

/* new window size or exposure */
static void reshape(int width, int height) {
	glViewport(0, 0, (GLint) width, (GLint) height);
}

static void create_shaders(void) {
	static const char *fragShaderText =
		"#ifdef GL_ES\n"
		"precision mediump float;\n"
		"#endif\n"
		"varying vec4 v_color;\n"
		"void main() {\n"
		"	gl_FragColor = v_color;\n"
		"}\n";
	static const char *vertShaderText =
		"uniform mat4 modelviewProjection;\n"
		"attribute vec4 pos;\n"
		"attribute vec4 color;\n"
		"varying vec4 v_color;\n"
		"void main() {\n"
		"	gl_Position = modelviewProjection * pos;\n"
		"	v_color = color;\n"
		"}\n";

	char msg[512];
	GLuint fragShader, vertShader, program;
	GLint stat;

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		fprintf(stderr, "Error: fragment shader did not compile!\n");
		glGetShaderInfoLog(fragShader, sizeof msg, NULL, msg);
		fprintf(stderr, "fragment shader info: %s\n", msg);
		exit(1);
	}

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		fprintf(stderr, "Error: vertex shader did not compile!\n");
		glGetShaderInfoLog(vertShader, sizeof msg, NULL, msg);
		fprintf(stderr, "fragment shader info: %s\n", msg);
		exit(1);
	}

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glAttachShader(program, vertShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &stat);
	if (!stat) {
		char log[1000];
		GLsizei len;
		glGetProgramInfoLog(program, 1000, &len, log);
		fprintf(stderr, "Error: linking:\n%s\n", log);
		exit(1);
	}
	

	glUseProgram(program);

	if (1) {
		/* test setting attrib locations */
		glBindAttribLocation(program, attr_pos, "pos");
		glBindAttribLocation(program, attr_color, "color");
		glLinkProgram(program);	/* needed to put attribs into effect */
	}
	else {
		/* test automatic attrib locations */
		attr_pos = glGetAttribLocation(program, "pos");
		attr_color = glGetAttribLocation(program, "color");
	}

	u_matrix = glGetUniformLocation(program, "modelviewProjection");
	printf("Uniform modelviewProjection at %d\n", u_matrix);
	printf("Attrib pos at %d\n", attr_pos);
	printf("Attrib color at %d\n", attr_color);
}


static void init() {
	/* Init shaders */
	glClearColor(0.4, 0.4, 0.4, 0.0);
	create_shaders();

	/* Save start time and last time so that we provide differentials */
	GameTime::setStartMs(std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()).count());
}

static void usage(void)
{
	printf("Usage:\n");
	printf("	-display <displayname>	set the display to run on\n");
	printf("	-info					 display OpenGL renderer info\n");
}

int keyevent(int code, int fields) {
	bool isPress = (bool)(fields & KEYEVENT_ONPRESS);
	bool isRelease = (bool)(fields & KEYEVENT_ONRELEASE);
	bool isSpecial = (bool)(fields & KEYEVENT_IS_SPECIAL);
	printf("c:%d s:%d p:%d r:%d\n",
		code,
		isSpecial,
		isPress,
		isRelease);
	if(isSpecial) {
		// Use special (named) key codes
		// ARROWS
		if (code == GLES2H_LEFT) {
			if(isPress) left = 1;
			if(isRelease) left = 0;
		}
		else if (code == GLES2H_RIGHT) {
			if(isPress) right = 1;
			if(isRelease) right = 0;
		}
		else if (code == GLES2H_UP) {
			if(isPress) up = 1;
			if(isRelease) up = 0;
		}
		else if (code == GLES2H_DOWN) {
			if(isPress) down = 1;
			if(isRelease) down = 0;
		}
		// Keep the app main loop running!
		return 0;
	} else {
		// Use ascii codes
		// ESC
		if(code == 27) {
			// EXIT on ESC!
			// App will return (1-1=0)
			return 1;
		}
		// WASD
		if (code == 'a') {
			if(isPress) left = 1;
			if(isRelease) left = 0;
		}
		else if (code == 'd') {
			if(isPress) right = 1;
			if(isRelease) right = 0;
		}
		else if (code == 'w') {
			if(isPress) up = 1;
			if(isRelease) up = 0;
		}
		else if (code == 's') {
			if(isPress) down = 1;
			if(isRelease) down = 0;
		}
		return 0;
	}
}

int main(int argc, char *argv[]) {
	char *dpyName = NULL;
	GLboolean printInfo = GL_FALSE;
	int i, retval;
	const char *s;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-display") == 0) {
		 dpyName = argv[i+1];
		 i++;
		}
		else if (strcmp(argv[i], "-info") == 0) {
		 printInfo = GL_TRUE;
		}
		else {
		 usage();
		 return -1;
		}
	}

	/* TODO: handle keyevents or idle */
	retval = gles2run(init, drawUpdate, reshape, NULL, keyevent, "GLES2-helper test code", 300, 300, GL_TRUE, NULL);

	return retval;
}

/* vim: set ts=4 sw=4 tw=0 noet : */
