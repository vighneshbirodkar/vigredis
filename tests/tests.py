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
        
def test_setbit():

    server.send("setbit key_of_a 1 1"+ nl)
    reply = server.recv(100).strip()
    
    if reply == ':0':
        pass
    else:
        assert False
        
    server.send("setbit key_of_a 2 1"+ nl)
    reply = server.recv(100).strip()
    
    if reply == ':0':
        pass
    else:
        assert False
        
    server.send("setbit key_of_a 7 1"+ nl)
    reply = server.recv(100).strip()
    
    if reply == ':0':
        pass
    else:
        assert False
        
    server.send("get key_of_a"+ nl)
    reply = server.recv(100)
    l,s = reply.split()
    l = l.strip()
    s = s.strip()
    if l == '$1':
        pass
    else:
        assert False
        
    if s == 'a':
        pass
    else:
        assert False
        
def test_getbit():
    server.send("set hello3 world3" + nl)
    reply = server.recv(100).strip()

    
    if reply == '+OK':
        pass
    else :
        assert False
    
    binString = ''.join(format(ord(x), 'b').zfill(8) for x in 'world3')
    for i in range(len(binString)):
        command = 'getbit hello3 %d' % i
        server.send(command + nl)
        reply = server.recv(100).strip()
        ans = ':%s' % binString[i]
        
        if reply == ans :
            pass
        else:
            assert False
