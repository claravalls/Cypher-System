all: p.exe
p.o: practica.c practica.h
	gcc -c practica.c
p.exe: practica.o
	gcc practica.o -o p.exe -Wall -Wextra
clean:
	rm *.o
