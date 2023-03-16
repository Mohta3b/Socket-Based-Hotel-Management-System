CC = g++ -g -std=c++11

all: server.out client.out

client.out: client.o
	$(CC) $^ -o $@

server.out: server.o Logs.o
	$(CC) $^ -lboost_filesystem -o $@

client.o: Client/client.cpp Client/client.hpp Server/utility.hpp Server/Errors.hpp
	$(CC) -c $< -o $@

server.o: Server/server.cpp Server/server.hpp Server/Logs.hpp Server/Errors.hpp\
 Server/utility.hpp ./Client/admin.hpp ./Client/user.hpp ./Client/room.hpp
	$(CC) -c $< -o $@

Logs.o: Server/Logs.cpp Server/Logs.hpp
	$(CC) -c $< -o $@

run: 
	g++ $(FILENAME).cpp -o $(FILENAME).o && ./$(FILENAME).o

.PHONY: clean
clean: 
	rm *.o *.out