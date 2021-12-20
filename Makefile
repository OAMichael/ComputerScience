all: Stat Write CopyEntries CopyPerm CopyOwn Readdir CopyDir Filefrag Statvfs Inotify Flock ProcessInfo Wait Integrate Signals MsgQueue Queue dlopen Threads Mmap

Stat:
	make -C 01-Stat
Write:
	make -C 02-Write
CopyEntries:
	make -C 03-CopyEntries
CopyPerm:
	make -C 04-CopyPerm
CopyOwn:
	make -C 05-CopyOwn
Readdir:
	make -C 06-Readdir
CopyDir:
	make -C 07-CopyDir
Filefrag:
	make -C 08-Filefrag
Statvfs: 
	make -C 09-Statvfs
Inotify:
	make -C 10-Inotify
Flock:
	make -C 11-Flock
ProcessInfo:
	make -C 12-ProcessInfo
Wait:
	make -C 13-Wait
Integrate:
	make -C 14-Integrate
Signals:
	make -C 15-Signals
MsgQueue:
	make -C 16-MsgQueue
Queue:
	make -C 17-Queue
dlopen:
	make -C 18-dlopen
Threads:
	make -C 19-Threads
Mmap:
	make -C 20-Mmap

.SILENT clean:
	rm -rf */*.exe
	rm -rf */*.o