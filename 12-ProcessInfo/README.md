# Process Information

### Before building you have to install these files: `<capability.h>`:
```console
$ sudo apt-get install libcap-dev
```

### and `<readproc.h>`:
```console
$ sudo apt install libprocps-dev
```

### These programs show a lot of different information about the process.
#### The main difference between them is that first one shows information about itself while the second can show same information about process with PID user enetered.

#### The second difference is that 12-ProcInfoSelf uses `getrusage()` and `getrlimit()` functions as it was supposed for the task. 12-ProcInfo uses some extra information obtained by `readproc()` function instead of two functions above.


### 12-ProcInfoSelf 

#### This one shows information about itself.  

##### By default it just displays information about PID, GID, etc. But there are some optional keys to make program show more.

Show information about scheduler:
```console
$ ./12-ProcInfoSelf.exe s
```

Show information about resource usage:
```console
$ ./12-ProcInfoSelf.exe u
```

Show information about resource limits:
```console
$ ./12-ProcInfoSelf.exe l
```

Show information about namespaces:
```console
$ ./12-ProcInfoSelf.exe n
```

Show information about capabilities:
```console
$ ./12-ProcInfoSelf.exe c
```

Show extra information (nice level, stack size, TTY etc.)
```console
$ ./12-ProcInfoSelf.exe x
```

Show information about environment
```console
$ ./12-ProcInfoSelf.exe e
```

You can combine them the way you like:
```console
$ ./12-ProcInfoSelf.exe n s u
```
```console
$ ./12-ProcInfoSelf.exe lcn
```
```console
$ ./12-ProcInfoSelf.exe sulncxe
```


### 12-ProcInfo

#### This one shows information about process with PID user entered.

##### The program is almost the same as 12-ProcInfoSelf, but there is a note:
  You can enter PID number like that:

```console
$ ./12-ProcInfo.exe [PID] [flags]
```

  It is optionally. By default program shows information about itself as 12-ProcInfoSelf does.
