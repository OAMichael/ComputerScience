import select
import socket
import sys

SRV_ADDR = ('81.5.88.117', 7500)

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    s.connect(SRV_ADDR)
    sources = select.poll()
    sources.register(sys.stdin, select.POLLIN)
    sources.register(s, select.POLLIN)
    try:
        while True:
            for src, event in sources.poll():
                print('New event:', src, event)
                if src == sys.stdin.fileno():
                    line = sys.stdin.readline().strip()
                    s.send(line.encode('utf-8'))
                else:
                    packet = s.recv(4096)
                    try:
                        print('Msg from server, len=%d: "%s"' % (len(packet), packet.decode('utf-8').strip()))
                    except UnicodeDecodeError:
                        print('Non-utf-8 msg from server, len=%d: "%r"' % (len(packet), packet))
    except KeyboardInterrupt:
        s.close()

if __name__ == '__main__':
    main()