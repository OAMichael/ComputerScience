CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 03-CopyEntries

03-CopyEntries:
	$(CC) $(CFLAGS) -c 03.1-Copy.c -o 03.1-Copy.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 03.1-Copy.o $(LIB_OBJ) -o 03.1-Copy.exe

	$(CC) $(CFLAGS) -c 03.2-Copy.c -o 03.2-Copy.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 03.2-Copy.o $(LIB_OBJ) -o 03.2-Copy.exe

	$(CC) $(CFLAGS) -c 03.STAR-Copy.c -o 03.STAR-Copy.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 03.STAR-Copy.o $(LIB_OBJ) -o 03.STAR-Copy.exe
