.PHONY: 01-Stat	02-Write 03-CopyEntries 04-CopyPerm 05-CopyOwn 06-Readdir 07-CopyDir 09-Statvfs 10-Inotify 11-Flock 12-ProcessInfo 13-Wait 14-Integrate 15-Signals 16-MsgQueue 17-Queue 18-dlopen 19-Threads 20-Mmap

all: 01-Stat 02-Write 03-CopyEntries 04-CopyPerm 05-CopyOwn 06-Readdir 07-CopyDir 09-Statvfs 10-Inotify 11-Flock 12-ProcessInfo 13-Wait 14-Integrate 15-Signals 16-MsgQueue 17-Queue 18-dlopen 19-Threads 20-Mmap

01-Stat:
	make -C 01-Stat
02-Write:
	make -C 02-Write
03-CopyEntries:
	make -C 03-CopyEntries
04-CopyPerm:
	make -C 04-CopyPerm
05-CopyOwn:
	make -C 05-CopyOwn
06-Readdir:
	make -C 06-Readdir
07-CopyDir:
	make -C 07-CopyDir
09-Statvfs: 
	make -C 09-Statvfs
10-Inotify:
	make -C 10-Inotify
11-Flock:
	make -C 11-Flock
12-ProcessInfo:
	make -C 12-ProcessInfo
13-Wait:
	make -C 13-Wait
14-Integrate:
	make -C 14-Integrate
15-Signals:
	make -C 15-Signals
16-MsgQueue:
	make -C 16-MsgQueue
17-Queue:
	make -C 17-Queue
18-dlopen:
	make -C 18-dlopen
19-Threads:
	make -C 19-Threads
20-Mmap:
	make -C 20-Mmap

.SILENT clean:
	rm -rf */*.exe
	rm -rf */*.o