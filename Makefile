CC = g++
HEADER = include/main.h
CFLAGS = -Wall


all: events.o main.o initializers.o configHandlers.o
	$(CC) $(CFLAGS) $^ -o $@

events.o : src/events.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@

main.o : src/main.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@
	
initializers.o : src/initializers.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@

configHandlers.o : src/configHandlers.cpp $(HEADER)
	$(CC) -c -Wall $< -o $@
clean:
	rm -rf *o all
