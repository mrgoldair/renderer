build:
	gcc -Wall -std=c99 ./src/*.c -o ./out/renderer -I /usr/local/Cellar/sdl2/2.0.22/include -l SDL2 -lm -L /usr/local/Cellar/sdl2/2.0.22/lib/

run:
	./out/renderer

clean:
	rm -r out