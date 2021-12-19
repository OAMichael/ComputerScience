CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 05-CopyOwn

05-CopyOwn:
	$(CC) $(CFLAGS) -c 05-Fchown.c -o 05-Fchown.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 05-Fchown.o $(LIB_OBJ) -o 05-Fchown.exe