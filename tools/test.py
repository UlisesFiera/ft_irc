import socket
import time

s = socket.create_connection(("localhost", 6667))

s.sendall(b"NI")
time.sleep(1)
s.sendall(b"CK ")
time.sleep(1)
s.sendall(b"ali")
time.sleep(1)
s.sendall(b"ce")
time.sleep(1)
s.sendall(b"\r")
time.sleep(1)
s.sendall(b"\n")