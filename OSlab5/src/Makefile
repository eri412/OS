CC = gcc
CFLAGS = -Wall -Wextra
BINPATH = ./bin
SRCPATH = ./src
INCPATH = ./include

.PHONY: clean all

all: main_runt main_link

main_runt: $(BINPATH)/main_runt.o $(BINPATH)/libpi_1.so $(BINPATH)/libsquare_1.so $(BINPATH)/libpi_2.so $(BINPATH)/libsquare_2.so
	$(CC) $< -o $@ -ldl

main_link: $(BINPATH)/main_link.o $(BINPATH)/libpi_1.so $(BINPATH)/libsquare_1.so
	$(CC) -L$(BINPATH) -Wl,-rpath=$(BINPATH) $^ -o $@ -lpi_1 -lsquare_1

$(BINPATH)/main_runt.o: $(SRCPATH)/main_runt.c $(INCPATH)/pi.h $(INCPATH)/square.h
	$(CC) $(CFLAGS) -c -I$(INCPATH) $< -o $@

$(BINPATH)/main_link.o: $(SRCPATH)/main_link.c $(INCPATH)/pi.h $(INCPATH)/square.h
	$(CC) $(CFLAGS) -c -I$(INCPATH) $< -o $@

$(BINPATH)/pi_1.o: $(SRCPATH)/pi_1.c $(INCPATH)/pi.h
	$(CC) $(CFLAGS) -c -fpic -I$(INCPATH) $< -o $@

$(BINPATH)/pi_2.o: $(SRCPATH)/pi_2.c $(INCPATH)/pi.h
	$(CC) $(CFLAGS) -c -fpic -I$(INCPATH) $< -o $@

$(BINPATH)/square_1.o: $(SRCPATH)/square_1.c $(INCPATH)/square.h
	$(CC) $(CFLAGS) -c -fpic -I$(INCPATH) $< -o $@

$(BINPATH)/square_2.o: $(SRCPATH)/square_2.c $(INCPATH)/square.h
	$(CC) $(CFLAGS) -c -fpic -I$(INCPATH) $< -o $@

$(BINPATH)/libpi_1.so: $(BINPATH)/pi_1.o
	gcc -shared $< -o $@

$(BINPATH)/libpi_2.so: $(BINPATH)/pi_2.o
	gcc -shared $< -o $@

$(BINPATH)/libsquare_1.so: $(BINPATH)/square_1.o
	gcc -shared $< -o $@

$(BINPATH)/libsquare_2.so: $(BINPATH)/square_2.o
	gcc -shared $< -o $@

clean:
	rm $(BINPATH)/*
