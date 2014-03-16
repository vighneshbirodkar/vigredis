vigredis
========

A sub set of redis implemented for http://exotel.in/about/exotel-tech-challenge-1/
A short article about **VigRedis** ca be found here - http://wp.me/p2D1Yw-5e.

Dependencies
------------
On a Linux System, none whatsoever.

Compiling
---------
```shell
cd src
make
./vigredis.out
```

Usage
-----
```shell
./vigredis.out -f filename -p port
```
`filename` - The file to use for loading and storing database.

`port` - The port to listen on for incoming connections.

Defualt port is `15000` and defualt dump file is `~/vr.dump`.

Unit Tests
----------
Running unit tests requires `python-nose`

Ensure that the file used by `vigredis` is either empty or doesn't exist before running the tests.


