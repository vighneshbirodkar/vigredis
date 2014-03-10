
#ifndef __UTIL_H__
#define __UTIL_H__
#include<math.h>

#define VR_EPS 0.000000001

#define VR_EQ(a,b) ( abs(a-b) < VR_EPS ) 
#define VR_NEQ(a,b) ( abs(a-b) > VR_EPS ) 

void gen_random(char *s,const int len) ;
void str_lower( char* s );
void rstrip(char* s);
int isint(char* s);
double get_time_ms();
int isdouble(char *s);
int isint_neg(char *s);

#endif
