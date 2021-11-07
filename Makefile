CC = gcc

CFLAGS = -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

LIB_PATH = ./Library/util.c

########################################################
SOURCE_PATH1 = ./01-Stat/01-Stat.c
OBJECT1 = ./01-Stat/01-Stat.o
EXECUTABLE1 = ./01-Stat/01-Stat.exe
########################################################
SOURCE_PATH2 = ./02-Write/02.1-Write.c
OBJECT2 = ./02-Write/02.1-Write.o
EXECUTABLE2 = ./02-Write/02.1-Write.exe
########################################################
SOURCE_PATH3 = ./02-Write/02.2-Write.c
OBJECT3 = ./02-Write/02.2-Write.o
EXECUTABLE3 = ./02-Write/02.2-Write.exe
########################################################
SOURCE_PATH4 = ./03-CopyEntries/03.1-Copy.c
OBJECT4 = ./03-CopyEntries/03.1-Copy.o
EXECUTABLE4 = ./03-CopyEntries/03.1-Copy.exe
########################################################
SOURCE_PATH5 = ./03-CopyEntries/03.2-Copy.c
OBJECT5 = ./03-CopyEntries/03.2-Copy.o
EXECUTABLE5 = ./03-CopyEntries/03.2-Copy.exe
########################################################
SOURCE_PATH6 = ./03-CopyEntries/03.STAR-Copy.c
OBJECT6 = ./03-CopyEntries/03.STAR-Copy.o
EXECUTABLE6 = ./03-CopyEntries/03.STAR-Copy.exe
########################################################
SOURCE_PATH7 = ./04-CopyPerm/04-Copy.c
OBJECT7 = ./04-CopyPerm/04-Copy.o
EXECUTABLE7 = ./04-CopyPerm/04-Copy.exe
########################################################
SOURCE_PATH8 = ./05-CopyOwn/05-Fchown.c
OBJECT8 = ./05-CopyOwn/05-Fchown.o
EXECUTABLE8 = ./05-CopyOwn/05-Fchown.exe
########################################################
SOURCE_PATH9 = ./06-Readdir/06.1-Readdir.c
OBJECT9 = ./06-Readdir/06.1-Readdir.o
EXECUTABLE9 = ./06-Readdir/06.1-Readdir.exe
########################################################
SOURCE_PATH10 = ./06-Readdir/06.2-Readdir.c
OBJECT10 = ./06-Readdir/06.2-Readdir.o
EXECUTABLE10 = ./06-Readdir/06.2-Readdir.exe
########################################################
SOURCE_PATH11 = ./06-Readdir/06.3-Getdents.c
OBJECT11 = ./06-Readdir/06.3-Getdents.o
EXECUTABLE11 = ./06-Readdir/06.3-Getdents.exe
########################################################
SOURCE_PATH12 = ./06-Readdir/06.4-Recursive6.2.c
OBJECT12 = ./06-Readdir/06.4-Recursive6.2.o
EXECUTABLE12 = ./06-Readdir/06.4-Recursive6.2.exe
########################################################
SOURCE_PATH13 = ./07-CopyDir/07.2-CopyDir.c
OBJECT13 = ./07-CopyDir/07.2-CopyDir.o ./Library/util.o
EXECUTABLE13 = ./07-CopyDir/07.2-CopyDir.exe

########################################################
SOURCE_PATH17 = ./12-ProcessInfo/12-ProcInfo.c
OBJECT17 = ./12-ProcessInfo/12-ProcInfo.o
EXECUTABLE17 = ./12-ProcessInfo/12-ProcInfo.exe


all: 01-Stat.o 01-Stat.exe 02.1-Write.o 02.1-Write.exe 02.2-Write.o 02.2-Write.exe 03.1-Copy.o 03.1-Copy.exe
all: 03.2-Copy.o 03.2-Copy.exe 03.STAR-Copy.o 03.STAR-Copy.exe 04-Copy.o 04-Copy.exe 05-Fchown.o 05-Fchown.exe
all: 06.1-Readdir.o 06.1-Readdir.exe 06.2-Readdir.o 06.2-Readdir.exe 06.3-Getdents.o 06.3-Getdents.exe
all: 06.4-Recursive6.2.o 06.4-Recursive6.2.exe 07.2-CopyDir.o util.o 07.2-CopyDir.exe 12-ProcInfo.o 12-ProcInfo.exe

#################################################################
01-Stat.o: $(SOURCE_PATH1)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH1) -o $(OBJECT1)

01-Stat.exe: $(OBJECT1)
	$(CC) $(OBJECT1) -o $(EXECUTABLE1)
#################################################################
02.1-Write.o: $(SOURCE_PATH2)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH2) -o $(OBJECT2)

02.1-Write.exe: $(OBJECT2)
	$(CC) $(OBJECT2) -o $(EXECUTABLE2)
#################################################################
02.2-Write.o: $(SOURCE_PATH3)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH3) -o $(OBJECT3)

02.2-Write.exe: $(OBJECT3)
	$(CC) $(OBJECT3) -o $(EXECUTABLE3)
#################################################################
03.1-Copy.o: $(SOURCE_PATH4)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH4) -o $(OBJECT4)

03.1-Copy.exe: $(OBJECT4)
	$(CC) $(OBJECT4) -o $(EXECUTABLE4)
#################################################################
03.2-Copy.o: $(SOURCE_PATH5)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH5) -o $(OBJECT5)

03.2-Copy.exe: $(OBJECT5)
	$(CC) $(OBJECT5) -o $(EXECUTABLE5)
#################################################################
03.STAR-Copy.o: $(SOURCE_PATH6)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH6) -o $(OBJECT6)

03.STAR-Copy.exe: $(OBJECT6)
	$(CC) $(OBJECT6) -o $(EXECUTABLE6)
#################################################################
04-Copy.o: $(SOURCE_PATH7)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH7) -o $(OBJECT7)

04-Copy.exe: $(OBJECT7)
	$(CC) $(OBJECT7) -o $(EXECUTABLE7)
#################################################################
05-Fchown.o: $(SOURCE_PATH8)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH8) -o $(OBJECT8)

05-Fchown.exe: $(OBJECT8)
	$(CC) $(OBJECT8) -o $(EXECUTABLE8)
#################################################################
06.1-Readdir.o: $(SOURCE_PATH9)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH9) -o $(OBJECT9)

06.1-Readdir.exe: $(OBJECT9)
	$(CC) $(OBJECT9) -o $(EXECUTABLE9)
#################################################################
06.2-Readdir.o: $(SOURCE_PATH10)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH10) -o $(OBJECT10)

06.2-Readdir.exe: $(OBJECT10)
	$(CC) $(OBJECT10) -o $(EXECUTABLE10)
#################################################################
06.3-Getdents.o: $(SOURCE_PATH11)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH11) -o $(OBJECT11)

06.3-Getdents.exe: $(OBJECT11)
	$(CC) $(OBJECT11) -o $(EXECUTABLE11)
#################################################################
06.4-Recursive6.2.o: $(SOURCE_PATH12)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH12) -o $(OBJECT12)

06.4-Recursive6.2.exe: $(OBJECT12)
	$(CC) $(OBJECT12) -o $(EXECUTABLE12)
#################################################################
07.2-CopyDir.o: $(SOURCE_PATH13)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH13) -o ./07-CopyDir/07.2-CopyDir.o

util.o: $(LIB_PATH)
	$(CC) $(CFLAGS) -c $(LIB_PATH) -o ./Library/util.o

07.2-CopyDir.exe: ./07-CopyDir/07.2-CopyDir.o ./Library/util.o
	$(CC) $(OBJECT13) -o $(EXECUTABLE13) 
#################################################################
12-ProcInfo.o: $(SOURCE_PATH17)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH17) -o $(OBJECT17)

12-ProcInfo.exe: $(OBJECT17)
	$(CC) $(OBJECT17) -lcap -lprocps -o $(EXECUTABLE17)
#################################################################

.SILENT clean:
	rm -rf */*.exe
	rm -rf */*.o