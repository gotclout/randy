#include "r3.h"

/**
 * Executes the grep command
 *
 * @param char* pcmd[]
 * @param int psz
 */
static void myGrep(char* pcmd[], int psz)
{
  char cmd[MAX_LEN];
  char line[MAX_LEN];
  int i=0;

  memset(cmd, 0, MAX_LEN);
  for(;i<psz;i++)
  {
    strcat(cmd,pcmd[i]);
    if(i+1!=psz)
    {
      strcat(cmd," ");
    }
  }

  FILE* f = (FILE*)popen(cmd, "r");
  if (f==NULL)
  {
    fprintf(stderr,"fopen %s Returned NULL\n", cmd);
  }
  else
  {
    memset(line,0,MAX_LEN);
    while(fgets(line,sizeof(line),f))
    {

      printf("%s", line);
      memset(line,0,MAX_LEN);
    }
    pclose(f);
  }
}

/**
 * Executes ps
 *
 * @param char* pcmd
 * @param psz
 */
static void myPS(char* pcmd[], int psz)
{
  char cmd[MAX_LEN];
  char line[MAX_LEN];
  int i=0;

  memset(cmd, 0, MAX_LEN);
  for(;i<psz;i++)
  {
    strcat(cmd,pcmd[i]);
    if(i+1!=psz)
    {
      strcat(cmd," ");
    }
  }

  FILE* f = (FILE*)popen(cmd, "r");
  if (f==NULL)
  {
    fprintf(stderr,"fopen %s Returned NULL\n", cmd);
  }
  else
  {
    memset(line,0,MAX_LEN);
    while(fgets(line,sizeof(line),f))
    {

      printf("%s", line);
      memset(line,0,MAX_LEN);
    }
    pclose(f);
  }
}

/**
 * Implements ls
 *
 * @param char* pcmd[]
 * @param int psz
 */
static void myLS(char* pcmd[], int psz)
{
  char cmd [MAX_LEN];
  char line[MAX_LEN];
  int i=0;

  memset(cmd, 0, MAX_LEN);
  for(;i<psz;i++)
  {
    strcat(cmd, pcmd[i]);
    if(i+1!=psz)
    {
      strcat(cmd," ");
    }

  }

  FILE* f = (FILE*)popen(cmd, "r");
  if (f==NULL)
  {
    fprintf(stderr,"fopen %s Returned NULL\n", cmd);
  }
  else
  {
    memset(line,0,MAX_LEN);
    while(fgets(line,sizeof(line),f))
    {

      printf("%s", line);
      memset(line,0,MAX_LEN);
    }
    pclose(f);
  }
}

/**
 * Executes diff
 *
 * @param char* pcmd
 * int psz
 */
static void myDiff(char* pcmd[], int psz)
{
  char cmd [MAX_LEN];
  char line[MAX_LEN];
  int i=0;

  memset(cmd, 0, MAX_LEN);
  for(;i<psz;i++)
  {
    strcat(cmd, pcmd[i]);
    if(i+1!=psz)
    {
      strcat(cmd," ");
    }

  }

  FILE* f = (FILE*)popen(cmd, "r");
  if (f==NULL)
  {
    fprintf(stderr,"fopen %s Returned NULL\n", cmd);
  }
  else
  {
    memset(line,0,MAX_LEN);
    while(fgets(line,sizeof(line), f))
    {

      printf("%s", line);
      memset(line,0,MAX_LEN);
    }
    pclose(f);
  }

}

/**
 * Retrieves command line arguments
 *
 * @param CmdData* d
 * @return bool
 */
bool get_args(CmdData* d)
{
  printf("\n>> "); //prompt
  char* buf = 0;
  size_t ARG_MAX = MAX_ARG;
  size_t ap, len, pos = getline(&buf, &ARG_MAX, stdin), cpos = pos;//read

  d->strcmd = (char*) malloc(pos);
  memset(d->strcmd, 0, pos);
  memcpy(d->strcmd, buf, pos);
  if(!(d->argc = 0) && pos > 0)
  {
    while(pos-- && !(ap = 0))
    {
      while(buf && !isspace(*buf++)){ --pos; ++ap; }
      d->args[d->argc] = (char*)malloc((len = 1 + sizeof(char) * ap++));
      strncpy((char*)memset(d->args[d->argc++], 0, len), buf - ap, ap - 1);
    }
    free((buf - cpos));
  }
  return d->argc > 0;
}

/**
 * Retrieves the argument at the specified position
 *
 * @param CmdData* cmd
 * @param int i arg index
 * @return char* the arg
 */
char* CmdData_at(CmdData* cmd, int i)
{
  return *cmd->args && i < cmd->argc ? cmd->args[i] : 0;
}

/**
 * Retrieves the jth character or the ith argument in the cmd
 *
 * @param CmdData* pCmd
 * @param int i arg index
 * @param int j arg char index
 * @return char the char at arg[i][j]
 */
char CmdData_argat(CmdData* cmd, int i, int j)
{
  char* arg = CmdData_at(cmd, i);
  return arg ? *(arg + j) : '\0';
}

/**
 * Deallocates memory for CmdData
 *
 * @param CmdData** pCmd
 */
void CmdData_Free(CmdData** pCmd)
{
  CmdData* cmd = *pCmd;
  int i        = 0;

  if(cmd->pth) { free(cmd->pth); cmd->pth = 0; }
  for( ; i < cmd->argc; ++i) if(CmdData_at(cmd, i)) free(CmdData_at(cmd, i));
}

/**
 * Initializes CmdData
 *
 * @param CmdData** pCmd
 */
void CmdData_Init(CmdData** pCmd)
{
  CmdData* cmd = *pCmd;

  if(cmd->pth) { CmdData_Free(&cmd); }
  memset((cmd->pth = (char*)malloc(cmd->plen)), 0, cmd->plen);
  memset(cmd->args, 0, sizeof(char*)*MAX_LEN);
  cmd->inf  = cmd->outf = 0;
  cmd->argc = 0;
}

/**
 * Creates and allocates memory for a CmdData
 */
CmdData* CmdData_Create()
{
  CmdData* cmd;
  cmd = (CmdData*) malloc(sizeof(CmdData));
  cmd->plen = 2*(MAX_LEN)*sizeof(char);
  cmd->pth  = 0;
  CmdData_Init(&cmd);

  return cmd;
};

/**
 *
 */
int main(int argc, char* argv[])
{
  //create a new command structure to imitate argc/argv
  CmdData* cmd = CmdData_Create();
  int        i = 1;

  cmd->argc   = argc - 1;                 //ignore program name
  cmd->strcmd = (char*) malloc(MAX_ARG); //allocate a string for using system
  memset(cmd->strcmd, 0, MAX_ARG);
  for( ; i < argc; ++i)              //copy argv
  {
    cmd->args[i - 1] = (char*) malloc(strlen(argv[i]));
    strcpy(cmd->args[i - 1], argv[i]);
    strcat(cmd->strcmd, argv[i]);
    if(i + 1 < argc) strcat(cmd->strcmd, " ");
  }

  //Example Could Have Used My system Implementation
  //Rather than popen
  /**
   * Test this after the rest is compiled and tested
   *
   * Be sure to modify your working system by removing main
   * including the sys.h file and removing the headers in the .c file
   *
  if(strstr(cmd->strcmd, "ping") != 0)
  {
    system(cmd->strcmd);
  }
  */


  do  //process accepted commands until user enters q
  {
    if(cmd->argc >= 3 && strcmp(cmd->args[0], "grep") == 0)
    {
      myGrep(cmd->args, cmd->argc);
    }
    else if(cmd->argc >= 1 && strcmp(cmd->args[0],"ps") == 0)
    {
      myPS(cmd->args,cmd->argc);
    }
    else if(cmd->argc >= 1 && strcmp(cmd->args[0], "ls") == 0)
    {
      myLS(cmd->args, cmd->argc);
    }
    else if(cmd->argc >= 3 && strcmp(cmd->args[0], "diff") == 0)
    {
      myDiff(cmd->args, cmd->argc);
    }
    else
    {
      fprintf(stderr, "Accepted Commands are grep, ps, ls, diff\n");
    }

    printf("Please enter a command or press q to exit\n");

    CmdData_Init(&cmd);
  }
  while( get_args(cmd) && strcmp(CmdData_at(cmd, 0),"q"));


  if(cmd) CmdData_Free(&cmd);

  return 0;
}

