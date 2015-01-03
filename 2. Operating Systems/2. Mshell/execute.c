/******************************************************************************
 *
 *  File Name........: execute.c
 *
 *  Description......: Executes each command in pipe received
 *
 *  Author...........: Pallavi S Deo (psdeo@ncsu.edu)
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "parse.h"
#include "execute.h"

char *home , *path;
extern char **environ;
int prompt_flag, exit_flag;
Pipe pipe_line;
int statrc, oldrc , rcflag; // Variables for ushrc reading
char prompt [512];
static void cdExec(Cmd c);
static void echoExec(Cmd c);
static void niceExec(Cmd c);
static void pwdExec(Cmd c);
static void setenvExec(Cmd c);
static void unsetenvExec(Cmd c);
static void whereExec(Cmd c);

/*-----------------------------------------------------------------------------
 *
 * Name...........: ioRedirect
 *
 * Description....: Setup input & output based on values in cmd
 *
 * Input Param(s).: Cmd
 *
 *----------------------------------------------------------------------------*/
static void ioRedirect(Cmd c)
{
	int res=0;
	if(c->in==Tin)
		if((res = open(c->infile,O_RDONLY)) != -1)
		{
			if(dup2(res,fileno(stdin)) == -1)
			{
				fprintf(stderr,"Input redirect failed\n");
				exit(-1);
			}
			close(res);
		}
	if ( c->out != Tnil )
	switch ( c->out )
	{
		case Tapp:
			if((res = open(c->outfile,O_CREAT|O_WRONLY|O_APPEND)) != -1)
			{
				printf("All further outputs are redirected, unless otherwise specified.\nInput is still from prompt\n");
				dup2(res,fileno(stdout));
				//printf("Dup2 done\n");
				res = close(res);
				if (res != 0)
				printf("Error closing the file\n");
			}
			break;
		case TappErr:
			if((res = open(c->outfile,O_CREAT|O_WRONLY|O_APPEND)) != -1)
			{
				dup2(res,fileno(stderr));
				close(res);
			}
		case ToutErr:
			if((res = open(c->outfile,O_CREAT|O_WRONLY|O_TRUNC)) != -1)
			{
				dup2(res,fileno(stderr));
				close(res);
			}	
		case Tout:
			//printf("Inside tout");
			if((res = open(c->outfile,O_CREAT|O_WRONLY|O_TRUNC)) != -1)
			{
				dup2(res,fileno(stdout));
				close(res);
			}	
			break;
		case Tpipe:
			//printf ("tpipe\n");
			break;
		case TpipeErr:
			//printf("Tpipe err\n");
			break;
		default:
			break;
	}                         
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: exCmd
 *
 * Description....: Parses the pipe received & executes each command in it.
 *
 * Input Param(s).: Pipe
 *
 *----------------------------------------------------------------------------*/

static void exCmd(Cmd c)
{
  int status=0, pid =0;
  if ( c ) {
	//redirecting i/o
	ioRedirect(c);
	// For builtin commands
	if(!strcmp(c->args[0],"cd")) {cdExec(c); return;}
	else if(!strcmp(c->args[0],"echo")) {echoExec(c); return;}
	else if(!strcmp(c->args[0],"pwd")) {printf ("pwd\n"); pwdExec(c); return;}
	else if(!strcmp(c->args[0],"setenv")) {setenvExec(c); return;}
	else if(!strcmp(c->args[0],"unsetenv")) {unsetenvExec(c); return;}
	else if(!strcmp(c->args[0],"where")) {whereExec(c); return;}
	else if(!strcmp(c->args[0],"nice")) {niceExec(c); return;}
	
	// Fork, as there is a new command to be executed
		pid = fork();
		if(!pid)
        {
			//printf("Executing %s...\n",c->args[0]);
			execvp(c->args[0],c->args);
			//exec functions only return in case of an error
 			printf("command not found\n");
 			exit(0);
		}
		else
		{
 			//printf("Child id: %d\n",pid);
 			if(pid)
				waitpid(pid,&status,0);
		}
		return;
  }
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: exPipe
 *
 * Description....: Parses the list of pipes received & executes each command in it.
 *
 * Input Parameter.: Pipe
 *
 *-----------------------------------------------------------------------------*/
void exPipe(Pipe p)
{
  Cmd c;
  c = malloc(sizeof(struct cmd_t));
  if ( p == NULL )
    return;
  for ( c = p->head; c != NULL; c = c->next ) 
  {
	if(!strcmp(c->args[0],"logout"))	exit(0);
   exCmd(c);
   }
  if (p->next == NULL) 
	{
		free(c);
		return;
	}
  exPipe(p->next);
}

static void sigflush()
{
	if(exit_flag)
		exit(0);
	printf("\n");
	prompt_flag = 0;
	pipe_line = NULL;
	if(prompt_flag == 0)
	{
		printf("%s",prompt);
		fflush(stdout);
		prompt_flag = 1;
	}
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: getShellData
 *
 * Description....: Initialization of signals & getting environment
 *
 * Input Parameter.: None
 *
 *-----------------------------------------------------------------------------*/
void getShellData()
{
	char temppath[512];
	home = getenv("HOME");
	path = getenv("PATH");
	// Setting the signals. Not sure if correct. Need to check
	signal(SIGTSTP,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,sigflush);
	// Moving on to ushrc reading
	strcpy(temppath,home);
	strcat(temppath,"/.ushrc");
	if((statrc = open(temppath,O_RDONLY)) != -1)
	{
		oldrc = dup(fileno(stdin));
		if(oldrc == -1)
		{
			fprintf(stderr,"dup rcfile failed");
			exit(-1);
		}
		if(dup2(statrc,fileno(stdin))==-1)
		{
			fprintf(stderr,"dup rcfile failed");
			exit(-1);
		}
		close(statrc);
		rcflag = 1;	
	}
	else
	{
		char tempname[512];
		int len;
		rcflag = 0;
		if(isatty(fileno(stdout)))
		{		
			gethostname(tempname,len);
			if(len)
				strcpy(prompt,tempname);
			strcat(prompt,"% ");
		}
	}
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: sisdir
 *
 * Description....: An attempt to write S_ISDIR function. Somehow not able to use inbuilt one.
 *
 * Input Parameter.: Path
 *
 *-----------------------------------------------------------------------------*/
static int sisdir(char *temppath)
{
	int r;
	struct stat *buf;
	buf = malloc(sizeof(struct stat));
	stat(temppath,buf);
	r = buf->st_mode & S_IFDIR;
	free(buf);
	return r;
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: cdExec
 *
 * Description....: Execution code for inbuilt command cd
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void cdExec(Cmd c) // Need to implement error handling
{
	char *cmd_path;
	if(c->args[1] == NULL) // The variable HOME is the default dir.
	{
		c->args[1] = malloc(strlen(home));
		strcpy(c->args[1],home);
	}
	if(c->args[1][0]=='~')
	{
		char* temppath;
		int tlen = 0;
		temppath = strtok(c->args[1],"~"); // Split the string delimited by ~
		if(temppath) // if anything is present after ~
			tlen = strlen(temppath);
		c->args[1] = malloc(strlen(home)+tlen); // changing path length to accomodate extended path
		strcpy(c->args[1],home);
		if(temppath)
			strcat(c->args[1],temppath); // Now args holds entire path we wish to go to
	}
	// Now we need to validate if the path specified exists or not
	if (sisdir(c->args[1])) // Not able to understand inbuilt S_ISDIR function..need to write own
	{
		if(c->args[1][0] == '/')
		{
			chdir("/");
		#ifdef DEBUG	
			printf("/");
		#endif	
		}
		cmd_path=strtok(c->args[1],"/");
		while(cmd_path != NULL) // Recursively change to inside directory.
		{
			if(cmd_path[0]!='~')
			{
				chdir(cmd_path);
			#ifdef DEBUG
				printf("%s/",cmd_path);
			#endif	
			}
			cmd_path = strtok(NULL,"/");
		}
	}	
	else
	fprintf(stderr, "No such file or directory\n"); // Atleast this is working!
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: echoExec
 *
 * Description....: Execution code for inbuilt command echo
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void echoExec(Cmd c)
{
	int i=0;
	for(i=0;c->args[i]!=NULL;i++)
		printf("%s ",c->args[i]);
	if(i!=1)
		printf("\n");
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: niceExec
 *
 * Description....: Execution code for inbuilt command nice
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void niceExec(Cmd c) 
{
	// did not really understand how to implement this
	if (c->args[1] == NULL)
	printf("0\n");
	else
	return;
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: pwdExec
 *
 * Description....: Execution code for inbuilt command pwd
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void pwdExec(Cmd c) 
{
	char *dir;
	dir = (char*)get_current_dir_name();
	printf("%s\n",dir);
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: setenvExec
 *
 * Description....: Execution code for inbuilt command setenv
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void setenvExec(Cmd c)
{
	int i;
	if(c->args[1] == NULL)
	{
		char **e = environ;
		while(*e)
		{
			printf("%s\n",*e);
			*e++;
		}
		return;
	}
	else if(c->args[2] == NULL)
		setenv(c->args[1],"",1);
	else
		setenv(c->args[1],c->args[2],1);
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: unsetenvExec
 *
 * Description....: Execution code for inbuilt command unsetenv
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void unsetenvExec(Cmd c) 
{
	if(c->args[1] == NULL)
	{
		fprintf(stderr,"Arguments missing\n");
		return;
	}
	else
		unsetenv(c->args[1]);
}

/*-----------------------------------------------------------------------------
 *
 * Name...........: whereExec
 *
 * Description....: Execution code for inbuilt command where
 *
 * Input Parameter.: Cmd
 *
 *-----------------------------------------------------------------------------*/

static void whereExec(Cmd c) 
{
	char *pointer;
	char temppath[512] , cmd_path[512]="";
	if(!strcmp(c->args[1],"cd")) printf ("cd\n");
	else if(!strcmp(c->args[1],"echo")) printf ("echo\n");
	else if(!strcmp(c->args[1],"logout")) printf ("logout\n");
	else if(!strcmp(c->args[1],"nice")) printf ("nice\n");
	else if(!strcmp(c->args[1],"setenv")) printf ("setenv\n");
	else if(!strcmp(c->args[1],"unsetenv")) printf ("unsetenv\n");
	else if(!strcmp(c->args[1],"pwd")) printf ("pwd\n");
	else if(!strcmp(c->args[1],"where")) printf ("where\n");
	
	strcpy(temppath,path);
	pointer = strtok(temppath,":"); // delimiting the string by colon
	while(pointer != NULL)
	{
		strcpy(cmd_path,pointer);
		strcat(cmd_path,"/");
		strcat(cmd_path,c->args[1]);
		if( (access(cmd_path,F_OK) == 0) && ( ( !sisdir(cmd_path)) && (access(cmd_path,R_OK|X_OK) == 0)) )
			printf("%s\n",cmd_path);
		pointer = strtok(NULL,":"); 
	}

}