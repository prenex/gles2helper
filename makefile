all:
	g++ egl_example.cpp -o egl_example -lGLESv2 -lEGL -lX11
clean:
	rm -f egl_example
