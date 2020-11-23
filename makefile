CFLAGS=-Wall
LDFLAGS=-lm `pkg-config zlib --libs` -lreadline

DEBUG?=0
ifeq ($(DEBUG),1)
    CFLAGS += -g3 -DDEBUG
else
    CFLAGS += -DNDEBUG -O3 -flto -ftree-vectorize
endif


matbrowser:  matreader
	gcc $(CFLAGS) src/matbrowser.c matreader.o $(LDFLAGS) -o matbrowser

matreader:
	gcc -c $(CFLAGS) src/matreader.c

install:
	sudo cp matbrowser /usr/local/bin/

uninstall:
	sudo rm /usr/local/bin/matbrowser
