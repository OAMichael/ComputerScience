import select
import socket

def main():
    srv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    srv_sock.bind(('', 7500))
    srv_sock.listen(10)
    # Create poll object for handling multiple sockets (srv_sock + client connections):
    poll_obj = select.poll()
    poll_obj.register(srv_sock, select.POLLIN)
    # Create dict for clients, fd -> (addr, socket, fobj)
    clients = {}
    try:
        while True:
            for fd, event in poll_obj.poll():
                if fd == srv_sock.fileno():
                    # Get one more connection from the connection queue:
                    client_sock, client_addr = srv_sock.accept()
                    print('Connected from:', client_addr)
                    # Wrap it into a file-like object for convenience, use line buffering
                    client_fobj = client_sock.makefile('rw', buffering=1, encoding='utf-8')
                    # Add client to dict and poll_obj for further handling:
                    clients[client_sock.fileno()] = (client_addr, client_sock, client_fobj)
                    poll_obj.register(client_sock, select.POLLIN)
                    continue
                # Client socket event
                print('Clint event:', fd, event)
                (client_addr, client_sock, client_fobj) = clients[fd]
                if event == select.POLLIN:
                    # New data from client, read it and send to all clients
                    line = client_fobj.readline()
                    if len(line) > 0:
                        print('Client data: %r' % line)
                        for other_client_fd in clients:
                            if fd == other_client_fd:
                                continue
                            (other_client_addr, _, other_client_fobj) = clients[other_client_fd]
                            print('Forwarding to', other_client_addr)
                            other_client_fobj.write(str(client_addr) + ' ' + line)
                            other_client_fobj.flush()
                        # This client isn't disconnected yet, handle next event(s)
                        continue
                # POLLHUP or POLLIN with 0-lenght data:
                print('Disconnecting', fd, client_addr)
                poll_obj.unregister(fd)
                client_fobj.close()
                try:
                    client_sock.shutdown(socket.SHUT_RDWD)
                except OSError as exc:
                    print('Client has closed its connection too early, was it a bot?', client_addr, exc)
                client_sock.close()
                del clients[fd]
    except KeyboardInterrupt:
        srv_sock.close()    

if __name__ == '__main__':
    main()