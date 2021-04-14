all:
	gcc tetris.c -o tetris -Wall -Wextra -O2 -std=c89 -lSDL2

clean:
	rm -f tetris
