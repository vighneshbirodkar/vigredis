
import socket

host = ''
port = 40000
backlog = 5
size = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host,port))
s.listen(backlog)
while 1:
    client, address = s.accept()
    data = client.recv(size)
    print ".%s." % data
    for x in data:
        print ord(x)
    if data:
        client.send(data)
    client.close() 
