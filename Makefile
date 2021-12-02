CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ./Library/util.c
LIB_OBJ  = ./Library/util.o

.PHONY: 01-Stat	02-Write 03-CopyEntries 04-CopyPerm 05-CopyOwn 06-Readdir 07-CopyDir 09-Statvfs 10-Inotify 11-Flock 12-ProcessInfo 13-Wait 14-Integrate 15-Signals 16-MsgQueue 17-Queue 20-Mmap

all: 01-Stat 02-Write 03-CopyEntries 04-CopyPerm 05-CopyOwn 06-Readdir 07-CopyDir 09-Statvfs 10-Inotify 11-Flock 12-ProcessInfo 13-Wait 14-Integrate 15-Signals 16-MsgQueue 17-Queue 20-Mmap

01-Stat:
	$(CC) $(CFLAGS) $@/01-Stat.c -o $@/01-Stat.exe

02-Write:
	$(CC) $(CFLAGS) -c $@/02.1-Write.c -o $@/02.1-Write.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/02.1-Write.o $(LIB_OBJ) -o $@/02.1-Write.exe

	$(CC) $(CFLAGS) $@/02.2-Write.c -o $@/02.2-Write.exe

03-CopyEntries:
	$(CC) $(CFLAGS) $@/03.1-Copy.c -o $@/03.1-Copy.exe
	$(CC) $(CFLAGS) $@/03.2-Copy.c -o $@/03.2-Copy.exe
	$(CC) $(CFLAGS) $@/03.STAR-Copy.c -o $@/03.STAR-Copy.exe

04-CopyPerm:
	$(CC) $(CFLAGS) $@/04-Copy.c -o $@/04-Copy.o

05-CopyOwn:
	$(CC) $(CFLAGS) -c $@/05-Fchown.c -o $@/05-Fchown.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/05-Fchown.o $(LIB_OBJ) -o $@/05-Fchown.exe

06-Readdir:
	$(CC) $(CFLAGS) -c $@/06.1-Readdir.c -o $@/06.1-Readdir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/06.1-Readdir.o $(LIB_OBJ) -o $@/06.1-Readdir.exe

	$(CC) $(CFLAGS) -c $@/06.2-Readdir.c -o $@/06.2-Readdir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/06.2-Readdir.o $(LIB_OBJ) -o $@/06.2-Readdir.exe

	$(CC) $(CFLAGS) -c $@/06.3-Getdents.c -o $@/06.3-Getdents.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/06.3-Getdents.o $(LIB_OBJ) -o $@/06.3-Getdents.exe

	$(CC) $(CFLAGS) -c $@/06.4-Recursive6.2.c -o $@/06.4-Recursive6.2.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/06.4-Recursive6.2.o $(LIB_OBJ) -o $@/06.4-Recursive6.2.exe

07-CopyDir:
	$(CC) $(CFLAGS) -c $@/07.2-CopyDir.c -o $@/07.2-CopyDir.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/07.2-CopyDir.o $(LIB_OBJ) -o $@/07.2-CopyDir.exe

09-Statvfs: 
	$(CC) $(CFLAGS) $@/09-Statvfs.c -o $@/09-Statvfs.exe

10-Inotify:
	$(CC) $(CFLAGS) $@/10-Inotify.c -o $@/10-Inotify.exe
	$(CC) $(CFLAGS) $@/10-Inotify_CRT_DEL.c -o $@/10-Inotify_CRT_DEL.exe

11-Flock:
	$(CC) $(CFLAGS) $@/11-Flock.c -o $@/11-Flock.exe

12-ProcessInfo:
	$(CC) $(CFLAGS) $@/12-ProcInfoSelf.c -lcap -lprocps -o $@/12-ProcInfoSelf.exe
	$(CC) $(CFLAGS) $@/12-ProcInfo.c -lcap -lprocps -o $@/12-ProcInfo.exe

13-Wait:
	$(CC) $(CFLAGS) $@/13.1-Wait.c -o $@/13.1-Wait.exe
	$(CC) $(CFLAGS) $@/13.2-pidfd_open.c -o $@/13.2-pidfd_open.exe
	$(CC) $(CFLAGS) $@/13.2-ptrace.c -o $@/13.2-ptrace.exe

14-Integrate:
	$(CC) $(CFLAGS) $@/14-Integrate.c -lm -ldl -o $@/14-Integrate.exe
	$(CC) $(CFLAGS) $@/14-IntegrateThreads.c -lm -ldl -pthread -o $@/14-IntegrateThreads.exe	

15-Signals:
	$(CC) $(CFLAGS) -c $@/15.1-Sigaction.c -o $@/15.1-Sigaction.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/15.1-Sigaction.o $(LIB_OBJ) -o $@/15.1-Sigaction.exe

	$(CC) $(CFLAGS) -c $@/15.2-Termination10.c -o $@/15.2-Termination10.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/15.2-Termination10.o $(LIB_OBJ) -o $@/15.2-Termination10.exe

	$(CC) $(CFLAGS) -c $@/15.2-Termination10_CRT_DEL.c -o $@/15.2-Termination10_CRT_DEL.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/15.2-Termination10_CRT_DEL.o $(LIB_OBJ) -o $@/15.2-Termination10_CRT_DEL.exe

	$(CC) $(CFLAGS) -c $@/15.STAR-SigReader.c -o $@/15.STAR-SigReader.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/15.STAR-SigReader.o $(LIB_OBJ) -o $@/15.STAR-SigReader.exe

	$(CC) $(CFLAGS) $@/15.STAR-SigSender.c -o $@/15.STAR-SigSender.exe

16-MsgQueue:
	$(CC) $(CFLAGS) $@/16-MsgQueue.c -lrt -o $@/16-MsgQueue.exe

17-Queue:
	$(CC) $(CFLAGS) -c $@/17-QueueReader.c -o $@/17-QueueReader.o
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o $(LIB_OBJ)
	$(CC) $@/17-QueueReader.o $(LIB_OBJ) -lrt -o $@/17-QueueReader.exe

	$(CC) $(CFLAGS) $@/17-QueueSender.c -lrt -o $@/17-QueueSender.exe

20-Mmap:
	$(CC) $(CFLAGS) $@/20.0.c -lrt -o $@/20.0.exe

	$(CC) $(CFLAGS) $@/20.2-Encryption.c -o $@/20.2-Encryption.exe


.SILENT clean:
	rm -rf */*.exe
	rm -rf */*.o