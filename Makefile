CFLAGS = -Wall -g -std=c++11

client: client.cpp helpers.hpp json.hpp
	g++ $(CFLAGS) -o client client.cpp helpers.cpp

run: client
	./client

clean:
	rm -f *.o client
	
