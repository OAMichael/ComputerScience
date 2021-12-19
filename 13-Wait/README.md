# Wait

#### `13.1-Wait.c` 
#### Parent process will wait its child.
##### In this particular program child just sleeps 20 seconds - time interval when you can send him signals.



#### For `13.2-pidfd_open.c` and `13.2-ptrace.c` situation is reversed - child wait for parent's state change.
#### For `13.2-ptrace.c` you need to activate ptrace like that:

```console
$ sudo bash -c "echo 0 > /proc/sys/kernel/yama/ptrace_scope"
``` 
##### Parent sleeps 20 seconds - it is the time to send him signals.
