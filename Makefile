all: DES
	./DES
DES: src/main.cpp src/events.cpp include/main.h
	g++ src/main.cpp src/events.cpp -o DES 
clean:
	rm DES