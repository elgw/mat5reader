
cflags=-g -Wall
ldflags=-lm `pkg-config zlib --libs` -lreadline


matbrowser:  matreader
	gcc $(cflags) src/matbrowser.c matreader.o $(ldflags) -o matbrowser

matreader: 
	gcc -c $(cflags) src/matreader.c $(ldflags)


