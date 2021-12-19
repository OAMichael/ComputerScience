CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 02-Write

02-Write:
	$(CC) $(CFLAGS) -c 02.1-Write.c -o 02.1-Write.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 02.1-Write.o $(LIB_OBJ) -o 02.1-Write.exe

	$(CC) $(CFLAGS) 02.2-Write.c -o 02.2-Write.exe