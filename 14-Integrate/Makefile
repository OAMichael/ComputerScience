CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 14-Integrate

14-Integrate:
	$(CC) $(CFLAGS) 14-Integrate.c -lm -ldl -o 14-Integrate.exe
	$(CC) $(CFLAGS) 14-IntegrateThreads.c -lm -ldl -pthread -o 14-IntegrateThreads.exe