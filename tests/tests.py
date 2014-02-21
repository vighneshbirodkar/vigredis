import socket
import time
import conf
import time

NUM = 10000
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
nl = '\r\n'
server.connect(('localhost', conf.PORT))

def test_set_get():
    server.send("set hello1 world1" + nl)
    reply = server.recv(100).strip()

    if reply == '+OK':
        pass
    else :
        assert False
        
    server.send("get hello1" + nl)
    reply = server.recv(100)
    print reply
    l,s = reply.split()
    l = l.strip()
    s = s.strip()
    if l == '$6':
        pass
    else:
        assert False
        
    if s == 'world1':
        pass
    else:
        assert False

def test_set_get_expiry():
    server.send("set hello2 world2 ex 2" + nl)
    reply = server.recv(100).strip()

    if reply == '+OK':
        pass
    else :
        assert False
        
    server.send("get hello2" + nl)
    reply = server.recv(100)
    l,s = reply.split()
    l = l.strip()
    s = s.strip()
    if l == '$6':
        pass
    else:
        assert False
        
    if s == 'world2':
        pass
    else:
        assert False
        
    time.sleep(2)
    
    server.send("get hello2" + nl)
    reply = server.recv(100).strip()

    if reply == '$-1':
        pass
    else:
        assert False
        

