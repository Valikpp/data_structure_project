CFLAGS = -g -Wno-unused-parameter
CC = gcc

PROGRAMS = e1main e2main e3main 

.PHONY:	all clean

all: $(PROGRAMS)

e1main: exo1.o e1main.o 
	$(CC) -o $@ $(CFLAGS) $^

e1main.o: e1main.c
	gcc $(CFLAGS) -c e1main.c

exo1.o: exo1.c
	gcc $(CFLAGS) -c exo1.c

e2main: e2main.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

e2main.o: e2main.c
	gcc $(CFLAGS) -c e2main.c

exo2.o: exo2.c
	gcc $(CFLAGS) -c exo2.c

e3main: e3main.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

e3main.o: e3main.c
	gcc $(CFLAGS) -c e3main.c

exo3.o: exo3.c
	gcc $(CFLAGS) -c exo3.c

clean:
	rm -f *.o *~ $(PROGRAMS)