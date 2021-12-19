CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 15-Signals

15-Signals:
	$(CC) $(CFLAGS) 15.1-Sigaction.c -o 15.1-Sigaction.exe

	$(CC) $(CFLAGS) 15.2-Termination10.c -o 15.2-Termination10.exe

	$(CC) $(CFLAGS) 15.2-Termination10_CRT_DEL.c -o 15.2-Termination10_CRT_DEL.exe

	$(CC) $(CFLAGS) 15.STAR-SigReader.c -o 15.STAR-SigReader.exe

	$(CC) $(CFLAGS) 15.STAR-SigSender.c -o 15.STAR-SigSender.exe
