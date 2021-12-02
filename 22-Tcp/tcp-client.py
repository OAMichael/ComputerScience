import select
import socket
import sys

SRV_ADDR = ('81.5.88.117', 7500)

def main():
    # Create a TCP socket
    conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    # Establish connection, TODO: configurate timeout before possible hang in 'connect'
    conn.connect(SRV_ADDR)
    conn_fobj = conn.makefile('rw', encoding='utf-8')
    # Create poll object for handling stdin and socket
    stdin_fd = sys.stdin.fileno()
    poll_array = select.poll()
    poll_array.register(stdin_fd, select.POLLIN)
    poll_array.register(conn, select.POLLIN)
    # Handle input and incoming data
    runnung = True
    try:
        while runnung:
            for fd, event in poll_array.poll():
                if fd == stdin_fd:
                    line = sys.stdin.readline()
                    conn_fobj.write(line)
                    conn_fobj.flush()
                else:
                    if event == select.POLLIN:
                        line = conn_fobj.readline()
                        sys.stdout.write(line)
                    else:
                        print('Disconnected!')
                        runnung = False
                        break
    except KeyboardInterrupt:
        pass
    # We're done, close connection
    conn.shutdown(socket.SHUT_RDWR)

if __name__ == '__main__':
    main()