all:
	gcc -c -I./vendor/raylib/src src/main.c -o out/main.o
	gcc -o bin/botanic_sweeper out/main.o -L./lib -lraylib -lm -lpthread -ldl -lrt -lX11
