build:
	gcc voxel.c dos/*.c `sdl2-config --libs --cflags` -I"/opt/homebrew/include/" -L"/opt/homebrew/lib/" -lGLEW -framework OpenGL -lpthread -o voxel

run:
	./voxel

clean:
	rm voxel