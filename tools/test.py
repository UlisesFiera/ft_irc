import socket

s = socket.socket()
s.connect(("127.0.0.1", 6667))

s.sendall(b"PING :hello\r\n")

response = s.recv(4096)
print(response.decode())

s.close()