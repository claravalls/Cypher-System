CC = gcc
CFLAGS  = -Wall -Wextra

p.exe:  main.o manager.o logica.o 
	$(CC) -o p.exe main.o manager.o logica.o $(CFLAGS)

main.o:  main.c
	$(CC) -c main.c $(CFLAGS)

manager.o:  manager.c manager.h 
	$(CC) -c manager.c $(CFLAGS)

logica.o:  logica.c logica.h 
	$(CC) -c logica.c $(CFLAGS)

clean: 
	$(RM) count *.o *~