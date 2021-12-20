CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 20-Mmap

20-Mmap:
	$(CC) $(CFLAGS) 20.0.c -lrt -o 20.0.exe

	$(CC) $(CFLAGS) 20.1-Server.c -lpthread -lrt -o 20.1-Server.exe

	$(CC) $(CFLAGS) 20.1-Client.c -lpthread -lrt -o 20.1-Client.exe

	$(CC) $(CFLAGS) 20.2-Encryption.c -o 20.2-Encryption.exe
