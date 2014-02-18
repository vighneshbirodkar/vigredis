/*
 * Format strings for reply
 */
#ifndef __STRING_DEF_H__
#define __STRING_DEF_H__

//Command isn't known
#define VR_REPLY_UNKNOWN_COMMAND "-ERR unknown command '%s'\n"
//Wrong number of arguments
#define VR_REPLY_WRONG_ARG_SET "-ERR wrong number of arguments for 'set' command\n"
//Some sort of error parsing
#define VR_REPLY_SYNTAX_ERROR "-Err syntax error\n"
//Aal eez well
#define VR_REPLY_OK "+OK\n"
//Mostly returned when key noy found
#define VR_REPLY_NOT_OK "$-1\n"
//reply for a string
#define VR_REPLY_STRING "$%d\n%.*s\n"
//not found
#define VR_REPLY_NOT_FOUND "$-1\n"


#endif
