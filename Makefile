build:
	gcc -Wall -std=c99 ./src/*.c -o renderer -I /opt/homebrew/Cellar/sdl2/2.0.22/include -l SDL2 -L /opt/homebrew/Cellar/sdl2/2.0.22/lib/

run:
	./renderer

clean:
	rm renderer