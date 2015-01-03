/******************************************************************************
 *
 *  File Name........: speak.c
 *
 *  Description......:
 *	Create a process that talks to the listen.c program.  After 
 *  connecting, each line of input is sent to listen.
 *
 *  This program takes two arguments.  The first is the host name on which
 *  the listen process is running. (Note: listen must be started first.)
 *  The second is the port number on which listen is accepting connections.
 *
 *  Revision History.:
 *
 *  When	Who         What
 *  09/02/96    vin         Created
 *
 *****************************************************************************/

/*........................ Include Files ....................................*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define LEN	64
  char buf[512];
main (int argc, char *argv[])
{
  int s, rc, len, port, pl, pr, len1, len2, len3, left;
  char host[LEN], str[LEN];
  struct hostent *hp , *hpr;
  struct sockaddr_in sin, sinp, sinr, sinl;
  struct playerInfo {
    char name[64];
    int port;
  }playerInfo;

  /* read host and port number from command line */
  if ( argc != 3 ) {
    fprintf(stderr, "Usage: %s <host-name> <port-number>\n", argv[0]);
    exit(1);
  }
  
  /* fill in hostent struct */
  hp = gethostbyname(argv[1]); 
  if ( hp == NULL ) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }
  port = atoi(argv[2]);

  /* create and connect to a socket */

  /* use address family INET and STREAMing sockets (TCP) */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if ( s < 0 ) {
    perror("socket:");
    exit(s);
  }

  /* set up the address and port of master*/
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

  /* connect to own socket at above addr and master's port */
  rc = connect(s, (struct sockaddr *)&sin, sizeof(sin));
  if ( rc < 0 ) {
    perror("connect:");
    exit(rc);	
  }
  int id=0;
  len = recv(s, (int *)&id, sizeof(id), 0);
  printf("Connected as player %d\n", id);
  int players = 0 , hops=0;
  len = recv(s, (int *)&players, sizeof(players), 0);
  len = recv(s, (int *)&hops, sizeof(hops), 0);
  int *mypotato = NULL;
  mypotato = (int*) malloc (sizeof (int) * (hops + 1)); //hops rem, trace 1, trace 2.......... trace n
  mypotato[0] = hops;
// At this point, this player can speak to master using socketfd s

  /* fill in hostent struct for self */
  gethostname(host, sizeof host);
  hp = gethostbyname(host);
  if ( hp == NULL ) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

/* Create new sockets for left and right connections*/
  pl = socket(AF_INET, SOCK_STREAM, 0);
  if ( pl < 0 ) {
    perror("socket:");
    exit(s);
  }
  pr = socket(AF_INET, SOCK_STREAM, 0);
  if ( pr < 0 ) {
    perror("socket:");
    exit(s);
  }

  sinp.sin_family = AF_INET;
  memcpy(&sinp.sin_addr, hp->h_addr_list[0], hp->h_length);

/* bind to a random port */
  srand(5);
int tempport;
  do{
   tempport = rand() % 60000; 
  if (tempport< 1024) tempport += 1024;
  sinp.sin_port = htons(tempport);
  rc = bind(pl, (struct sockaddr *)&sinp, sizeof(sinp));
  } while(rc < 0);
  //printf("%d\n",sinp.sin_port);
/* Listen on this socker for left player to connect*/
  rc = listen(pl, 5);
  if ( rc < 0 ) {
    perror("listen:");
    exit(rc);
  }

/* Send this port & host name to master */
  struct playerInfo current,right;
  gethostname(current.name, sizeof current.name);
  current.port  = tempport; //sinp.sin_port;

  len = send (s, (struct playerInfo *)&current, sizeof(struct playerInfo),0);

/* Receive right from master */
  len = recv(s, (struct playerInfo *)&right, sizeof(struct playerInfo), 0);
  if ( len < 0 ) {
  printf("%d\n" , errno);
  perror("recv ");
  exit(1);
  }
  //printf( "%s,   %d\n",right.name,right.port);
  hp = gethostbyname(right.name);
  if ( hp == NULL ) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

  sinr.sin_family = AF_INET;
  sinr.sin_port = htons(right.port);
  memcpy(&sinr.sin_addr, hp->h_addr_list[0], hp->h_length);

  if (id > 0){ //connect first, then accept
	//printf("connect first\n");
	  rc = connect(pr, (struct sockaddr *)&sinr, sizeof(sinr));
	  if ( rc < 0 ) {
	    printf("%d\n",errno);
	    perror("connect: in right");
	  } 
	  {
	  left = accept(pl, (struct sockaddr *)&sinl, &len);
	  if ( left< 0 ) {
	    perror("Connect: in left");
	  } }
  }
  else { //accept first then connect
	  {
	  left = accept(pl, (struct sockaddr *)&sinl, &len);
	  if ( rc< 0 ) {
	    perror("Connect: in left");
	    exit(rc);
	  }}
	  rc = connect(pr, (struct sockaddr *)&sinr, sizeof(sinr));
	  if ( rc < 0 ) {
	    printf("%d\n",errno);
	    perror("connect: in right");
	  }
  }

//printf ("connected\n");
  
  /* Make all sockets non blocking */
  int status;
  status = fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) perror("calling fcntl");
  status = fcntl(pl, F_SETFL, fcntl(pl, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) perror("calling fcntl");
  status = fcntl(pr, F_SETFL, fcntl(pr, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) perror("calling fcntl");
  status = fcntl(left, F_SETFL, fcntl(pr, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) perror("calling fcntl");

    /* Receive potato from master or any player */
while(1)
{
    len1 = len2 = len3 =0;
    len1 = recv(s, (int *)mypotato, sizeof (int) * (hops + 1),0);
    //if (len1< 0) perror("Potato received from master");
    len2 = recv(left, (int *)mypotato, sizeof (int) * (hops + 1),0);
    //if (len2< 0) perror("Potato received from left");
    len3 = recv(pr, (int *)mypotato, sizeof (int) * (hops + 1),0);
    //if (len3< 0) perror("Potato received from right");

    /* Pass potato to next player if hops is > 0 */
    if(len1>0 || len2>0 || len3>0)
    {
 	int index = hops - mypotato[0] +1;
	mypotato[index] = id;
	mypotato[0]-=1;
        if (mypotato[0]< 0) break;
      /* Pass potato to master if hops = 0 */
      if (mypotato[0] ==0){
		mypotato[0]=-1;
		printf("I'm it\n");
		len = send (s, (int *)mypotato, sizeof (int) * (hops + 1),0);
		if ( len < 0 ) 
		  perror("error in sending to master");
      }
      else {
	      int temp = rand();
	      if (temp % 2 ==0){ // pass to left
		if (id == players-1)       printf ("Sending potato to 0\n");
		else printf ("Sending potato to %d\n", (id+1));
		len = send (left, (int *)mypotato, sizeof (int) * (hops + 1),0);
		if ( len < 0 ) 
		perror("sending to left");
	      }
	      else{ // pass to right
		if (id == 0) printf ("Sending potato to %d\n", (players-1));
		else printf ("Sending potato to %d\n", (id-1));
		len = send (pr, (int *)mypotato, sizeof (int) * (hops + 1),0);
		if ( len < 0 ) 
		  perror("sending to right");
	      }
      }
    }
}
  
/* Logic to close connections */
close (s); close(left); close(pr); close(pl);
}

/*........................ end of player.c ...................................*/
