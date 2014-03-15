#ifndef __VIGREDIS_H__
#define __VIGREDIS_H__

#define VR_PORT 41000
#define VR_LISTEN_LIMIT 64
#define VR_LISTEN_WAIT_TIME 250000
#define VR_FILENAME strcat(getenv("HOME"),"/vr.dump")

#define MAX(a,b) (( a > b) ? a : b)


#define VR_SELECT_SEC 0
#define VR_SELECT_USEC 100

#define VR_GREETING "\n ---------- Greetings ----------\n"
#define VR_DETAILS "VigRedis now Running as PID:%d in PORT:%d\nUsing File : %s\n"
#define VR_VERSION "VigRedis 0.01 ( pre alpha )\n"
#define VR_SOURCE_URL "Browse Source at : https://github.com/vighneshbirodkar/vigredis\n"
#define VR_WARNING "This is not meant to be run on a Production Enviornment \nUnless you hate yourself !\n"
#define VR_BYE "--------- Exiting, Bye ----------\n"
#define VR_END_CHAR2 '\n'
#define VR_END_CHAR1 '\r'


#endif
