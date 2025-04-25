CFLAGS = -Wno-unused-parameter
CC = gcc

PROGRAMS = main1 main2 main3 main4 main5 main6 maincpu executor

.PHONY:	all clean

all: $(PROGRAMS)

main1: hashmap.o main1.o 
	$(CC) -o $@ $(CFLAGS) $^

main1.o: main1.c
	gcc $(CFLAGS) -c main1.c

hashmap.o: hashmap.c
	gcc $(CFLAGS) -c hashmap.c

main2: main2.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

main2.o: main2.c
	gcc $(CFLAGS) -c main2.c

memoryhandler.o: memoryhandler.c
	gcc $(CFLAGS) -c memoryhandler.c

main3: main3.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

main3.o: main3.c
	gcc $(CFLAGS) -c main3.c

parser.o: parser.c
	gcc $(CFLAGS) -c parser.c

main4: main4.o program.o cpu.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

main4.o: main4.c
	gcc $(CFLAGS) -c main4.c

cpu.o: cpu.c
	gcc $(CFLAGS) -c cpu.c

main5: main5.o program.o cpu.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

main5.o: main5.c
	gcc $(CFLAGS) -c main5.c

program.o: program.c
	gcc $(CFLAGS) -c program.c

main6: main6.o program.o cpu.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

main6.o: main6.c
	gcc $(CFLAGS) -c main6.c

maincpu: maincpu.o program.o cpu.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^

maincpu.o: maincpu.c
	gcc $(CFLAGS) -c maincpu.c

executor: executor.o program.o cpu.o parser.o memoryhandler.o hashmap.o
	$(CC) -o $@ $(CFLAGS) $^
executor.o: executor.c
	gcc $(CFLAGS) -c executor.c

clean:
	rm -f *.o *~ $(PROGRAMS)