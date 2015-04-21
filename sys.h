#ifndef __sys__
#define __sys__

/* SIDHU-HW2.c -- This program implements the system function:

   int system(const char*comm)
   Name: Randeep Sidhu
   Date: 3/26/2015
   Class: ISA563

*/

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>


#define MAX_LEN 256 //defining the max length of the arguments

int tokenize(char** pcomm, char* args[], char** outfile,
             char** infile, int* flag);

int system(const char *comm);

#endif
