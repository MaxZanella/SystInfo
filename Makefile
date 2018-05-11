all: exec

exec: main.c lib
	gcc -o main main.c -g -Wall libfractal/libfractal.a -Ilibfractal/ -pthread -lSDL -FRAMEWORK

lib:
	cd libfractal && $(MAKE)
clean:
	rm main
	cd libfractal && rm libfractal.a fractal.o tools.o

test: exec
	./main a.txt c.txt -d b
