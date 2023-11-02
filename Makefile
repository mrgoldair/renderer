build:
	gcc -Wall -std=c99 ./src/*.c -o ./out/renderer -I /opt/homebrew/Cellar/sdl2/2.28.4/include -l SDL2 -lm -L /opt/homebrew/Cellar/sdl2/2.28.4/lib/

run:
	./out/renderer

clean:
	rm -r out/*
