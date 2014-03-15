#ifndef __VIGREDIS_H__
#define __VIGREDIS_H__

#define VR_PORT 41000
#define VR_LISTEN_LIMIT 64
#define VR_LISTEN_WAIT_TIME 250000
#define VR_FILENAME strcat(getenv("HOME"),"/vr.dump")

#define MAX(a,b) (( a > b) ? a : b)


#define VR_SELECT_SEC 0
#define VR_SELECT_USEC 100

#define VR_END_CHAR2 '\n'
#define VR_END_CHAR1 '\r'


#endif
