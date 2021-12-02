import socket

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
    s.bind(('', 7500))
    clients = set()
    while True:
        try:
            # Wait for incoming packet
            data, addr_info = s.recvfrom(4096)
            # Update list of clients
            if addr_info not in clients:
                clients.add(addr_info)
                print('New client %s:%d' % addr_info)
            # Show incoming packet
            try:
                print('RX from %s (len=%d): "%r"' % (addr_info, len(data), data))
            except UnicodeDecodeError:
                print('Bad RX from %s (len=%d): "%r"' % (addr_info, len(data), data))
            # Replay this packet to all known clients
            for client_addr in clients:
                s.sendto(data, client_addr)
        except KeyboardInterrupt:
            s.close()


if __name__ == '__main__':
    main()