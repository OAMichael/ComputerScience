CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 13-Wait

13-Wait:
	$(CC) $(CFLAGS) 13.1-Wait.c -o 13.1-Wait.exe
	$(CC) $(CFLAGS) 13.2-pidfd_open.c -o 13.2-pidfd_open.exe
	$(CC) $(CFLAGS) 13.2-ptrace.c -o 13.2-ptrace.exe