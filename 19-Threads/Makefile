CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

all: 19-Threads

19-Threads:
	$(CC) $(CFLAGS) 19.1-Mutex.c -lm -pthread -o 19.1-Mutex.exe

	$(CC) $(CFLAGS) 19.2-semaphores_1.c -lm -lpthread -o 19.2-semaphores_1.exe

	$(CC) $(CFLAGS) 19.3-semaphores_2.c -lm -lpthread -o 19.3-semaphores_2.exe

	$(CC) $(CFLAGS) 19.4-atomic.c -lpthread -o 19.4-atomic.exe

	$(CC) $(CFLAGS) 19.6-OpenMP.c -fopenmp -lm -o 19.6-OpenMP.exe
