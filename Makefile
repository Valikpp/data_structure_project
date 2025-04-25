CFLAGS = -g -Wno-unused-parameter
CC = gcc

PROGRAMS = main1 main2 main3 main4 main5 main6 maincpu executor

.PHONY:	all clean

all: $(PROGRAMS)

main1: exo1.o main1.o 
	$(CC) -o $@ $(CFLAGS) $^

main1.o: main1.c
	gcc $(CFLAGS) -c main1.c

exo1.o: exo1.c
	gcc $(CFLAGS) -c exo1.c

main2: main2.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

main2.o: main2.c
	gcc $(CFLAGS) -c main2.c

exo2.o: exo2.c
	gcc $(CFLAGS) -c exo2.c

main3: main3.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

main3.o: main3.c
	gcc $(CFLAGS) -c main3.c

exo3.o: exo3.c
	gcc $(CFLAGS) -c exo3.c

main4: main4.o exo6.o exo4.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

main4.o: main4.c
	gcc $(CFLAGS) -c main4.c

exo4.o: exo4.c
	gcc $(CFLAGS) -c exo4.c

main5: main5.o exo6.o exo4.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

main5.o: main5.c
	gcc $(CFLAGS) -c main5.c

exo6.o: exo6.c
	gcc $(CFLAGS) -c exo6.c

main6: main6.o exo6.o exo4.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

main6.o: main6.c
	gcc $(CFLAGS) -c main6.c

maincpu: maincpu.o exo6.o exo4.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^

maincpu.o: maincpu.c
	gcc $(CFLAGS) -c maincpu.c

executor: executor.o exo6.o exo4.o exo3.o exo2.o exo1.o
	$(CC) -o $@ $(CFLAGS) $^
executor.o: executor.c
	gcc $(CFLAGS) -c executor.c

clean:
	rm -f *.o *~ $(PROGRAMS)