CC=emcc
CFLAGS=-Os -Wall

all: emsnake.html

emsnake.html: emsnake.o display_score.o
	emcc emsnake.o display_score.o -o $@ -s USE_SDL=2 -s SAFE_HEAP=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL_TTF=2 --preload-file assets

clean:
	rm emsnake.js emsnake.html emsnake.wasm    \
		emsnake.data emsnake.o display_score.o
