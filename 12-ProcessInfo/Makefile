CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 12-ProcessInfo

12-ProcessInfo:
	$(CC) $(CFLAGS) 12-ProcInfoSelf.c -lcap -lprocps -o 12-ProcInfoSelf.exe
	$(CC) $(CFLAGS) 12-ProcInfo.c -lcap -lprocps -o 12-ProcInfo.exe