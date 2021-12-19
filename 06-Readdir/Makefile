CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ../Library/util.c
LIB_OBJ  = ../Library/util.o

all: 06-Readdir

06-Readdir:
	$(CC) $(CFLAGS) -c 06.1-Readdir.c -o 06.1-Readdir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 06.1-Readdir.o $(LIB_OBJ) -o 06.1-Readdir.exe

	$(CC) $(CFLAGS) -c 06.2-Readdir.c -o 06.2-Readdir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 06.2-Readdir.o $(LIB_OBJ) -o 06.2-Readdir.exe

	$(CC) $(CFLAGS) -c 06.3-Getdents.c -o 06.3-Getdents.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 06.3-Getdents.o $(LIB_OBJ) -o 06.3-Getdents.exe

	$(CC) $(CFLAGS) -c 06.4-Recursive6.2.c -o 06.4-Recursive6.2.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) 06.4-Recursive6.2.o $(LIB_OBJ) -o 06.4-Recursive6.2.exe