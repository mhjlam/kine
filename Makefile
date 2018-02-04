all:
	g++ src/*.cpp -o bin/spline -lfreeglut -lglu32 -lopengl32 -mwindows
