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

def test_zadd():
    
    server.send("zadd seta -3.5 a" + nl)
    reply = server.recv(100).strip()
    
    if reply == ':1':
        pass
    else :
        assert False
        
    server.send("zadd seta -4.5 a" + nl)
    reply = server.recv(100).strip()

    if reply == ':0':
        pass
    else :
        assert False

def test_zcard():

    server.send("zadd setb -3.5 a" + nl)
    reply = server.recv(100)
    server.send("zadd setb 14 b" + nl)
    reply = server.recv(100)
    server.send("zadd setb 2.8 c" + nl)
    reply = server.recv(100)
    server.send("zadd setb 25 d" + nl)
    reply = server.recv(100)
    
    server.send("zadd setc -3.5 a" + nl)
    reply = server.recv(100)
    server.send("zadd setc 53.5 b" + nl)
    reply = server.recv(100)
    server.send("zadd setc 13.5 c" + nl)
    reply = server.recv(100)
    server.send("zadd setc 100 a" + nl)
    reply = server.recv(100)
    
    server.send('zcard setb' + nl)
    reply = server.recv(100).strip()
    
    if reply == ':4':
        pass
    else :
        assert False
        
    server.send('zcard setc' + nl)
    reply = server.recv(100).strip()
    if reply == ':3':
        pass
    else :
        assert False
        
def test_zrange():
    server.send("zadd setd 1 a" + nl)
    reply = server.recv(100)
    server.send("zadd setd 2 b" + nl)
    reply = server.recv(100)
    server.send("zadd setd 3 c" + nl)
    reply = server.recv(100)
    server.send("zadd setd 4 d" + nl)
    reply = server.recv(100)
    server.send("zadd setd 5 e" + nl)
    reply = server.recv(100)    
    
    server.send('zrange setd 1 -2 withscores' + nl)
    #time.sleep(.01)
    reply = server.recv(1000)
    parts = reply.split()
    
    #print reply
    #for r in reply:
    #    print r,ord(r)
    assert parts[0] == '*6'
    assert parts[2] == 'b'
    assert parts[4] == '2'
    assert parts[6] == 'c'
    assert parts[8] == '3'
    assert parts[10] == 'd'
    assert parts[12] == '4'
    
def test_zcount():
    server.send("zadd sete 1 aewf" + nl)
    reply = server.recv(100)
    server.send("zadd sete 2 bwfe" + nl)
    reply = server.recv(100)
    server.send("zadd sete 2 wcwe" + nl)
    reply = server.recv(100)
    server.send("zadd sete 4 d2wef" + nl)
    reply = server.recv(100)
    server.send("zadd sete 100 qwfe" + nl)
    reply = server.recv(100)    
    server.send("zadd sete 4 wewe" + nl)
    reply = server.recv(100)  
    server.send("zadd sete 7 w552e" + nl)
    reply = server.recv(100)      
    
    server.send('zcount sete 2 7' + nl)
    #time.sleep(.01)
    reply = server.recv(1000)
    reply = reply.split()[0]

    print reply
    assert reply == ':5'
        
