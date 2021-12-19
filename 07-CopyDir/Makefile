CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 07-CopyDir

07-CopyDir:
	$(CC) $(CFLAGS) -c 07.2-CopyDir.c -o 07.2-CopyDir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 07.2-CopyDir.o $(LIB_OBJ) -o 07.2-CopyDir.exe