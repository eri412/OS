CC=g++
CFLAGS=-std=c++17 -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
LFLAGS=-lzmq -lpthread
EXECUTABLE1=server
EXECUTABLE2=client

SRC1=server_main.cpp server.cpp
SRC2=client_main.cpp client.cpp
SRC=zmq_wrapper.cpp socket.cpp ntree.cpp

OBJ1=$(SRC1:.cpp=.o)
OBJ2=$(SRC2:.cpp=.o)
OBJ=$(SRC:.cpp=.o)

all: $(EXECUTABLE1) $(EXECUTABLE2)

$(EXECUTABLE1): $(OBJ1) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ1) $(OBJ) -o $@ $(LFLAGS) -Iinclude

$(EXECUTABLE2): $(OBJ2) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ2) $(OBJ) -o $@ $(LFLAGS) -Iinclude

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS) -Iinclude

clean:
	rm -f *.o $(EXECUTABLE1) $(EXECUTABLE2)