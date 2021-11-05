# Process Information

### Before building you have to install these files: `<capability.h>`:
```console
$ sudo apt-get install libcap-dev
```

### and `<readproc.h>`:
```console
$ sudo apt install libprocps-dev
```

#### This program shows a lot of different information about the process (itself). 

##### By default it just displays information about PID, GID, etc. But there are some optional keys to make program show more.

Show information about scheduler:
```console
$ ./12-Id.exe s
```

Show information about resource usage:
```console
$ ./12-Id.exe u
```

Show information about resource limits:
```console
$ ./12-Id.exe l
```

Show information about namespaces:
```console
$ ./12-Id.exe n
```

Show information about capabilities:
```console
$ ./12-Id.exe c
```

Show extra information (nice level, stack size, TTY etc.)
```console
$ ./12-Id.exe x
```

Show information about environment
```console
$ ./12-Id.exe e
```

You can combine them the way you like:
```console
$ ./12-Id.exe n s u
```
```console
$ ./12-Id.exe lcn
```
```console
$ ./12-Id.exe sulncxe
```


