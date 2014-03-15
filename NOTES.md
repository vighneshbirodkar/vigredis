To check for memory leaks use

valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./vigredis.out

