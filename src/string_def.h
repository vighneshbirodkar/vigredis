/*
 * Format strings for reply
 */
#ifndef __STRING_DEF_H__
#define __STRING_DEF_H__

//Command isn't known
#define VR_REPLY_UNKNOWN_COMMAND "-ERR unknown command '%s'\r\n"
//Wrong number of arguments
#define VR_REPLY_WRONG_ARG_SET "-ERR wrong number of arguments for '%s' command\r\n"
//Some sort of error parsing
#define VR_REPLY_SYNTAX_ERROR "-Err syntax error\r\n"
//Aal eez well
#define VR_REPLY_OK "+OK\r\n"
//Mostly returned when key noy found
#define VR_REPLY_NOT_OK "$-1\n"
//reply for a string
#define VR_REPLY_STRING "$%d\r\n%.*s\r\n"
//not found
#define VR_REPLY_NOT_FOUND "$-1\r\n"
//bit reply
#define VR_REPLY_BIT ":%d\r\n"
//bit offset wrong
#define VR_REPLY_WRONG_OFFSET "-ERR bit offset is not an integer or out of range\r\n"
//bit is bound of range
#define VR_REPLY_WRONG_BIT "-ERR bit is not an integer or out of range\r\n"
//value not a float
#define VR_REPLY_NOT_FLOAT "-ERR value is not a float\r\n"
//value not an int
#define VR_REPLY_NOT_INT "-ERR value is not an int\r\n"
//array size
#define VR_REPLY_ARRAY_SIZE "*%d\r\n"


#endif
