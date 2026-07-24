import socket
import time

s = socket.socket()
s.connect(("127.0.0.1", 6667))

s.sendall(b"asd")

time.sleep(20)

s.sendall(b"\r\n")

s.close()