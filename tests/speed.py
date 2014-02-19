import socket
import time

PORT = 41000
NUM = 50000
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect(('localhost', PORT))

i = 0
start = time.time()
while i < NUM:
    key = "key%d" % i
    value = "value%d" % i
    command = 'set %s %s' % (key,value)
    command = command + chr(13) + chr(10)

    client_socket.send(command)
    reply = client_socket.recv(100)
    reply = reply.strip()
    i += 1
    
stop = time.time()
total = stop - start
print "%d SET operations in %f seconds" %(NUM,total)

i = 0
start = time.time()
while i < NUM:
    key = "key%d" % i
    value = "value%d" % i
    command = 'get %s' % (key) + chr(13) + chr(10)

    client_socket.send(command)
    reply = client_socket.recv(100)
    recv_value = reply.split()[1]
    reply = reply.strip()

    if(recv_value != value):
        raise AssertionError
    i += 1
    
stop = time.time()
total = stop - start
print "%d GET operations in %f seconds" %(NUM,total)
