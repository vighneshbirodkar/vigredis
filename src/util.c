#include "util.h"
#include<stdlib.h>
#include <ctype.h>
#include<string.h>
#include<stdio.h>
#include <sys/time.h>


/*
 *Get time in ms
 */
 
double get_time_ms()
{
    struct timeval now;
    double d;

    gettimeofday(&now, NULL);
    d = now.tv_sec + ((double)now.tv_usec)/1000/1000/1000;
    return d;
}
/* 
 * DEBAGGING
 * Generates a random string in 's'
*/
void gen_random(char *s,const int len) 
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    int i;
    for ( i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

/*
 *changes chars in s to lower case
 */
void str_lower( char* s )
{
    int i=0;
    if (s == NULL)
        return;
    while(s[i])
    {
        s[i] = tolower(s[i]);
        //printf("i = %d\n",i);
        i++;
    }
}

/*
 * Strips whitespaces at the end of string
 */
void rstrip(char* s)
{
    int i = strlen(s)-1;
    if (s == NULL)
        return;
    while(i > 0)
    {
        if(isspace(s[i]))
            s[i] = 0;
        else
            break;
        i--;
    }
}

/*
 * Checks if string is an int ( only has digits)
 */
int isint(char *s)
{
    int i=0;
    while(s[i])
    {
        if(!isdigit(s[i]))
            return 0;
        i++;
    }
    return 1;
}


/*
 * checks if string is positive or negative int
 */
int isint_neg(char *s)
{
    int i=0;
    while(s[i])
    {
        if(!(isdigit(s[i]) || s[i] == '-'))
            return 0;
        i++;
    }
    return 1;
}

int isdouble(char *s)
{
    int i=0;
    while(s[i])
    {
        if(! (isdigit(s[i]) || (s[i] == '.') || (s[i] == '-') ) )
            return 0;
        i++;
    }
    return 1;
}
