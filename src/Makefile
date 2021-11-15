# Compiler
CC = gcc

# Includes
INC = ../include

# Flags:
CFLAGS = -I$(INC)
CFLAGS += -Wall

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all: engines_testclient engines_server

engines_server: $(SRCS)
	gcc -o ../bin/$@ $^ $(CFLAGS) $(LIBS)

engines_testclient: $(SRCS)
	gcc -o ../bin/$@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f ../bin/enco_to_net
	rmdir ../bin
