/**
 * SIDHU-HW2.c -- This program implements the system function int system(const char*comm)
 */

/*Name: Randeep Sidhu
Date: 3/26/2015
Class: ISA563

*/

#include "sys.h"
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

/**
 * Tokenizes the command to be executed by the system function
 * used for tokenizing ie split a string for the commands the system is executing
 */
int tokenize(char **pcomm, char* args[], char** outfile, char** infile, int* flag)
{
  char* comm = *pcomm;                          //set the pointer comm to a value pcomm for parsed command
  int argc=2;                                   //the system function calls sh -c which is two arguments
  int p;
  size_t len = strlen(comm);                    //set the size of the object to the string length of the command entered
  for(p=0; p < len && comm[p] != ' '; p++)      //for loop for running through the command
  {
    //nothing is needed inside function
  }

  if(p == len)                                  //if there is only one command i.e ls without anything after it
  {
    args[argc]=(char*)malloc(strlen(comm) + 1); //allocating memory for the argument
    memset(args[argc], 0, strlen(comm) + 1);    //setting memory for the argument
    strcpy(args[argc], comm);                   //copying command read in to the array of args
    argc++;                                     //incrementing argc so that it goes to the next place in the array
    *flag=argc;                                 //setting flag value from tokenize function to argc which will be the number
                                                //of arguments returned.
  }
  else
  {
    bool fset=false;                            //bool variable to see if the flag value is set.
                                                //get initial token; while there is a token stll left; get the next token
    char* token = 0;
    for(token = strtok(comm, " "); token !=0 && argc < MAX_LEN; token=strtok(0," ") )
    {
                                                    //copy a token into the first available argument
      args[argc]=(char*)malloc(strlen(token) + 1);  //allocating memory for the token
      memset(args[argc], 0, strlen(token) + 1);     //setting the memory for the token
      strcpy(args[argc],token);                     //copying what is at the array args[argc] into the token

      if(strcmp(args[argc],">")==0)                 //if you encounter the outfile operator
      {
        token=strtok(0," ");                        //set the token to null ended as we don't care what is after the output file operator
        *outfile=(char*)malloc(strlen(token));      //allocate memory for the token
        memset(*outfile, 0, strlen(token) + 1);     //set the memory for the token for the output file
        strcpy(*outfile, token);                    //copy output file into token
        if(!fset)                                   // fset == false
        {
          fset=true;
          *flag=argc;                               //set flag = to number of arguments
        }
      }
      else if(strcmp(args[argc],"<")==0)
      {
        token=strtok(0," ");  //setting token
        *infile=(char*)malloc(strlen(token));      //allocating memory for the input file
        memset(*infile, 0, strlen(token) + 1);     //setting the memory for the input file to the token
        strcpy(*infile, token);                    //copying the input file to the token

        if(!fset)                                  // fset == false
        {
          fset=true;
          *flag=argc;                              //set flag= to number of arguements.
        }
      }
      else if(strcmp(args[argc],"&")==0)           //if ampersand encountered in command you are calling
      {
        if(!fset)                                  // fset == false
        {
          fset=true;
          *flag=argc;                              //set flag to number of arguments
        }
      }
      else
        argc++;                                    //if you don't encounter any operators you keep on incrementing argc
    }
  }

  return(argc);                                    //return the amount of arguments being passed
}

/**
 * implementing the system function
 */
int system(const char *comm)
{
  char* args[MAX_LEN];                       //Defining MAX # of arguments can be
  memset(args, 0, sizeof(char*) * MAX_LEN);  //Setting the memory for these arguments

  int rval = EXIT_SUCCESS;                   //the return value of success if everything completes

  const char* cmd = "sh";                    //first argument system() calls is sh
  args[0]=(char*)malloc(strlen(cmd));        //allocating memory for args[0]
  strcpy(args[0], cmd);                      //copying cmd into args[0]

  const char* arg0="-c";                     //2nd argument system() calls is -c

  args[1]=(char*)malloc(strlen(arg0));       //allocating memory for insertion into array args for -c
  strcpy(args[1], arg0);                     //copying into args[1] the command -c
  char* infile=0,*outfile=0;                 //infile and outfile being used for input and output files
  int p = 0;
  size_t len = strlen(comm);                 //setting the length to the size of the command.
  for( p=0; p < len && comm[p] != ' '; p++)  //for loop to go through the command
  {
    //no arguments needed inside loop
  }

  int flag = 0;

  char* co = (char*) malloc(strlen(comm) + 1);         //since system accepts a constant need to copy comm to another
                                                       // variable called co
  memset(co, 0, strlen(comm)+1);
  strncpy(co, comm, strlen(comm));                     //strncpy copies full string up to the length of the string
  int argc=tokenize(&co,args,&outfile,&infile,&flag);  //calling the tokenize function you pass *co instead of comm.
  bool waitfor=1;       //waiting for a child process or not
  bool redirectin = 0;  //bool for whether you are redirecting input in
  bool redirectout = 0; //bool for whether you are redirecting output out

  pid_t child;
  int cstatus; //exit status of child
  pid_t c;     //pid of child to be returned by the wait

  //int j = 0;
  if(flag > 0)
  {
    args[flag]=0;
  }
  int ifid=0, ofid=0;

  const char* a = *(args+2); //setting char* a to args +2

  if((child= fork())==0)     //if forked
  {
    if(redirectout==1)       //if you are redirecting the command to an output file
    {
      close(1);              //close the file to make sure it wasn't being used
      int ofid = open(outfile, O_RDONLY|O_WRONLY|O_CREAT, 00666); //setting a variable to the open file command
      if(ofid==-1)                                                //if the return value from from ofid is -1
        fprintf(stderr, "Could not open %s\n",outfile);           //return an error that you couldn't open the file
    }
    if(redirectin==1)                                             //if you are redirecting output
    {
      close(0);                                                   //close the file to make sure it wasn't being used
      int ifid = open(infile, O_RDONLY|O_NONBLOCK, 00222);        //set an int for input id to the open file command
      if(ifid==-1)                                                //if an error
        fprintf(stderr, "Could not open %s\n",infile);
    }

    char* cc = *(args+3);  //setting char* c = *(args+3)
    execlp(a,"",cc);       //calling the execlp command and passing the arguments to it.
    if(redirectin)         //if output is being redirected in
    {
      close (0);           //close 0 is being used for a child
      if(ifid>0)           //if ifid returns a positive number
      {
        close(ifid);       //close the ifid
      }
    }

    if(redirectout)
    {
      close (1);          //close 1 is being use for parent.
      if(ofid>0)          //if open successful
      {
        close (ofid);     //close the ofid
      }
    }
    perror(*args);

    pause();  //waiting for a signal
    _exit(0); //exit for child process
  }
  else
  {
    if(child <0)
    {
      fprintf(stderr, "ERROR: could not fork subprocess\n");
      rval = EXIT_FAILURE;  //could not fork so failure
    }
    else if (waitfor==1)    //if you are waiting for child to complete
    {
      do
      {
        c = waitpid(child, &cstatus, 0);  //set variable c to waitpid passing the child and the cstatus
        if (c == -1)                      //if an error
        {
          perror("waitpid");              //error waitpid
          rval=EXIT_FAILURE;              //set the return value to exit with a failure
        }
        if(rval !=EXIT_FAILURE)           //if there is no failure check for signals
        {
          //Signal Handling
          if (WIFEXITED(cstatus))
            printf("exited, status=%d\n", WEXITSTATUS(cstatus));
          else if (WIFSIGNALED(cstatus))
            printf("killed by signal %d\n", WTERMSIG(cstatus));
          else if (WIFSTOPPED(cstatus))
            printf("stopped by signal %d\n", WSTOPSIG(cstatus));
          else if (WIFCONTINUED(cstatus)) printf("continued\n");

        }
      } while ((!WIFEXITED(cstatus) && !WIFSIGNALED(cstatus)) || rval !=EXIT_FAILURE);
      //while there is not a signal or process has not failed continue to run this loop
    }
  }

  return(rval);  //return the return value
}

/**
 * For testing purposes
 */
/*int main(int argc, char *argv[])
{
  system(argv[1]);

  return (0);
}*/
