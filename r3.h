#ifndef __r3__
#define __r3__

#include "sys.h"   //for using system implementation
#include <ctype.h> //implicit declaration of isspace

#define MAX_ARG 1024

/**
 * Contains data associated with a command to be executed
 */
typedef struct _CmdData
{
  char* args[MAX_LEN], *pth, *inf, *outf; //args, path to exec io redirect
  int argc , plen;                        //num args
  char* strcmd;
} CmdData;

#endif
