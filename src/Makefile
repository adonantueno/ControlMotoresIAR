# Compiler
CC = gcc

# Includes
INC = ../include

# Flags:
CFLAGS = -I$(INC)
CFLAGS += -Wall

BINS=engines_server engines_testclient

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all: $(OBJS) $(BINS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

engines_server: engines_server.o
	$(CC) $(LDFLAGS) $^ -o ../bin/$@ $(LDLIBS)

engines_testclient: engines_testclient.o
	$(CC) $(LDFLAGS) $^ -o ../bin/$@ $(LDLIBS)

clean:
	rm -f *.o
	rm -f ../bin/*
	rmdir ../bin
