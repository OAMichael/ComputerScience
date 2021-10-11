cmake_minimum_required(VERSION 2.8)

add_compile_options(
    -Wall 
    -Werror 
    -Wextra 
    -Wnarrowing 
    -Wconversion 
    -Wwrite-strings 
    -Wcast-qual 
    -Wundef 
    -Wstrict-prototypes 
    -Wbad-function-cast 
    -Wlogical-op 
    -Wreturn-type 
    -g
)

add_executable(./01-Stat/01-Stat.exe ./01-Stat/01-Stat.c)
add_executable(./02-Write/02.1-Write.exe ./02-Write/02.1-Write.c)
add_executable(./02-Write/02.2-Write.exe ./02-Write/02.2-Write.c)
add_executable(./03-CopyEntries/03.1-Copy.exe ./03-CopyEntries/03.1-Copy.c)
add_executable(./03-CopyEntries/03.2-Copy.exe ./03-CopyEntries/03.2-Copy.c)
add_executable(./03-CopyEntries/03.STAR-Copy.exe ./03-CopyEntries/03.STAR-Copy.c)
add_executable(./04-CopyPerm/04-Copy.exe ./04-CopyPerm/04-Copy.c)
add_executable(./05-CopyOwn/05-Fchown.exe ./05-CopyOwn/05-Fchown.c)
add_executable(./06-Readdir/06.1-Readdir.exe ./06-Readdir/06.1-Readdir.c)
add_executable(./06-Readdir/06.2-Readdir.exe ./06-Readdir/06.2-Readdir.c)
add_executable(./06-Readdir/06.3-Getdents.exe ./06-Readdir/06.3-Getdents.c)
add_executable(./06-Readdir/06.4-Recursive6.2.exe ./06-Readdir/06.4-Recursive6.2.c)