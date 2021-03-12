CC = g++
HEADER = include/main.h
CFLAGS = -Wall


all: events.o main.o
	$(CC) $(CFLAGS) $^ -o $@

events.o : src/events.cpp $(HEADER)
	$(CC) $(CFLAGS) $< -o $@

main.o : src/main.cpp $(HEADER)
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *o all
