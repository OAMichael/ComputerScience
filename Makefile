CC=gcc

CFLAGS= -O3 -Wall -Werror -Wextra -Wnarrowing -Wconversion -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast -Wlogical-op -Wreturn-type

SOURCE_PATH = ./07-CopyDir/07.2-CopyDir.c
TEST_PATH = ./Library/util.c

OBJECTS = ./07-CopyDir/07.2-CopyDir.o ./Library/util.o
EXECUTABLE = ./07-CopyDir/07.2-CopyDir.exe

.PHONY: all install uninstall clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) 

07.2-CopyDir.o: $(SOURCE_PATH)
	$(CC) $(CFLAGS) -c $(SOURCE_PATH) -o ./07-CopyDir/07.2-CopyDir.o 

util.o: $(TEST_PATH)
	$(CC) $(CFLAGS) -c $(TEST_PATH) -o ./Library/util.o

install:
	install $(EXECUTABLE)
uninstall:
	rm -rf $(EXECUTABLE)
clean:
	rm -rf *.o $(EXECUTABLE)
	rm -rf *.o $(OBJECTS)