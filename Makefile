all: exec

exec: main.c lib
	gcc -o main main.c -g -Wall libfractal/libfractal.a -Ilibfractal/ -pthread -lSDL

lib:
	cd libfractal && $(MAKE)
