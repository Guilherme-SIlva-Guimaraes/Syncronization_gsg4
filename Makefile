CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -g -pthread

all: sync_car

sync_car: sync_car.o sync_car-runner.o
	$(CC) $(CFLAGS) -o $@ sync_car.o sync_car-runner.o

sync_car.o: sync_car.c sync_car.h
	$(CC) $(CFLAGS) -c sync_car.c

sync_car-runner.o: sync_car-runner.c sync_car.h
	$(CC) $(CFLAGS) -Wno-unused-parameter -c sync_car-runner.c

clean:
	rm -f sync_car sync_car.o sync_car-runner.o

.PHONY: all clean
