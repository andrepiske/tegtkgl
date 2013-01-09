CC=gcc
CFLAGS=-O2 `pkg-config --cflags --libs gtk+-3.0`
OBJS=tegtkgl.o main.o

all: main

main: main.o tegtkgl.o
	$(CC) -o main $(CFLAGS) $(OBJS) -lGL -lX11 -lm
	
main.o: main.c tegtkgl.h
	$(CC) -c $(CFLAGS) main.c

tegtkgl.o: tegtkgl.c tegtkgl.h
	$(CC) -c $(CFLAGS) tegtkgl.c
	
clean:
	rm -f $(OBJS) main

