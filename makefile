all:
	g++ --std=c++11 egl_example.cpp -o egl_example -lGLESv2 -lEGL -lX11 -lGLEW -lglut
clean:
	rm -f egl_example
