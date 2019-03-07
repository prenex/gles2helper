egl:
	g++ --std=c++11 egl_example.cpp -o egl_example -lGLESv2 -lEGL -lX11
glut:
	g++ --std=c++11 egl_example.cpp -o egl_example -lGLESv2 -lX11 -lGLEW -lglut -DUSE_FULL_GL=1 -DGLES2_HELPER_USE_GLUT
clean:
	rm -f egl_example
