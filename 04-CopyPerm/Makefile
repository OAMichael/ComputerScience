CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 04-CopyPerm

04-CopyPerm:
	$(CC) $(CFLAGS) -c 04-Copy.c -o 04-Copy.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 04-Copy.o $(LIB_OBJ) -o 04-Copy.exe
