CC = g++
HEADER = include/main.h
CFLAGS = -Wall


all: events.o main.o
	$(CC) $(CFLAGS) $^ -o $@

events.o : src/events.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@

main.o : src/main.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@
	
clean:
	rm -rf *o all
