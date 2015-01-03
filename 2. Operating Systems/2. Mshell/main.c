/******************************************************************************
 *
 *  File Name........: main.c
 *
 *  Description......: Simple driver program for ush's parser
 *
 *  Author...........: Vincent W. Freeh
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "execute.h"

static void prCmd(Cmd c)
{
  int i;

  if ( c ) {
    //printf("%s%s ", c->exec == Tamp ? "BG " : "", c->args[0]);
    if ( c->in == Tin )
      //printf("<(%s) ", c->infile);
    if ( c->out != Tnil )
      switch ( c->out ) {
      case Tout:
	//printf(">(%s) ", c->outfile);
	break;
      case Tapp:
	//printf(">>(%s) ", c->outfile);
	break;
      case ToutErr:
	//printf(">&(%s) ", c->outfile);
	break;
      case TappErr:
	//printf(">>&(%s) ", c->outfile);
	break;
      case Tpipe:
	printf("| ");
	break;
      case TpipeErr:
	printf("|& ");
	break;
      default:
	fprintf(stderr, "Shouldn't get here\n");
	exit(-1);
      }

    if ( c->nargs > 1 ) {
      //printf("[");
      //for ( i = 1; c->args[i] != NULL; i++ )
	//printf("%d:%s,", i, c->args[i]);
      //printf("\b]");
    }
    //putchar('\n');
    // this driver understands one command
    if ( !strcmp(c->args[0], "end") )
      exit(0);
  }
}

static void prPipe(Pipe p)
{
  int i = 0;
  Cmd c;

  if ( p == NULL )
    return;

  //printf("Begin pipe%s\n", p->type == Pout ? "" : " Error");
  for ( c = p->head; c != NULL; c = c->next ) {
    //printf("  Cmd #%d: ", ++i);
    prCmd(c);
  }
  //printf("End pipe\n");
  prPipe(p->next);
}

int main(int argc, char *argv[])
{
  Pipe p;
  char *host = "armadillo";
  //Need to execute all commands from ushrc file first
	getShellData();
	//printf("done with getshelldata\n");
  // Then perform the shell in loop
  while ( 1 ) {
    printf("%s%% ", host);
	p = parse();
    prPipe(p);
	exPipe(p);
	//printf("back to main\n");
    freePipe(p);
	//printf("pipe freed\n");
  }
}

/*........................ end of main.c ....................................*/
