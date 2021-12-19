CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 17-Queue

17-Queue:
	$(CC) $(CFLAGS) -c 17-QueueReader.c -o 17-QueueReader.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 17-QueueReader.o $(LIB_OBJ) -lrt -o 17-QueueReader.exe

	$(CC) $(CFLAGS) 17-QueueSender.c -lrt -o 17-QueueSender.exe