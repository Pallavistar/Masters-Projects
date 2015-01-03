/******************************************************************************
 *
 *  File Name........: master.c
 *
 *  Description......: Acts as the ringmaster of Hot potato game
 *                            Accepts port number, no of player & no of hops
 *                            Waits for all players to join
 *                            Forms a ring by giving left & right neighbor info to each player
 *                            Creates a POTATO & passes it to a random player
 *                            Upon receiving the POTATO back, prints it & terminate the game.
 *
 *  Revision History.:
 *
 *  Date			| ID			| Description
 *  09/02/96    	| vin		| Created listen.c
 *  10/30/14		| psdeo		| Added upon listen.c to create master.c
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
#include <fcntl.h>
#include <sys/select.h>

  struct playerInfo {
    char name[64];
    int port;
  }playerInfo;

main (int argc, char *argv[])
{
  if(argc<4)
  {
  	printf("Usage: %s <port-number> <no-of-players> <no-of-hopes>\n", argv[0]);
	exit(1);
  }
  char buf[512];
  char host[64];
  int  fp, rc, len, port, players, hops;
  struct hostent *hp;
  struct sockaddr_in sin;
	
  port = atoi(argv[1]);		//  Master is listening to this port
  players = atoi(argv[2]);	//  No of players
  if (players <2) {printf ("At least 2 players required\n"); exit(1);}
  int s, p[players];
  struct hostent *ihp[players];
  struct sockaddr_in incoming[players];
  struct playerInfo current[players];
  int randmax = players - 1;
  hops = atoi(argv[3]);		//  No of hops
  int potlen = hops;			// Initial length of potato is 2 i.e. only header.

  /* fill in hostent struct for self */
  gethostname(host, sizeof host);
  hp = gethostbyname(host);
  if ( hp == NULL ) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

/* Print required statements*/
printf("Potato master on: %s\n" , hp->h_name);
printf("Players: %d\n", players);
printf("Hops: %d\n", hops);


 /* Game initialization
   *	1. create socket
   *	2. bind it to an address/port
   *	3. listen 
   *	4. accept a connection from all players
   *   5. form the ring
   */

  /* use address family INET and STREAMing sockets (TCP) */
  s= socket(AF_INET, SOCK_STREAM, 0);
  if ( s < 0 ) {
    perror("socket:");
    exit(s);
  }
  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

  /* bind socket s to address sin */
  rc = bind(s, (struct sockaddr *)&sin, sizeof(sin));
  if ( rc < 0 ) {
    perror("bind:");
    exit(rc);
  }
  /* Listen on the socket */
  rc = listen(s, 5);
  if ( rc < 0 ) {
    perror("listen:");
    exit(rc);
  }

  /* accept connections from all players*/
  int pcount = players, i=0;
  while (pcount){
	   len = sizeof(sin);
	   p[i] = accept(s, (struct sockaddr *)&incoming[i], &len);
	   if ( p [i]< 0 ) {
	     perror("bind:");
	     exit(rc);
	   }	
	   ihp[i] = gethostbyaddr((char *)&incoming[i].sin_addr, 
			sizeof(struct in_addr), AF_INET);
	   printf(">> player %d is on %s\n", i, ihp[i]->h_name);
	   len = send (p[i], (int *)&i, sizeof(i), 0);
	   len = send (p[i], (int *)&players, sizeof(players), 0);
	   len = send (p[i], (int *)&hops, sizeof(hops), 0);
	   /* Receive new port number from each player */
           len = recv(p[i], (struct payerInfo *)&current[i], sizeof(struct playerInfo), 0);
           //printf( "%s,   %d\n",current[i].name,current[i].port);
           if ( len < 0 ) {
           perror("recv");
           exit(1);
           }
	  i++;
	  pcount--;
  }

  /* Pass the new port number of each player to its left player. Player will then form the ring */
  for (i=0; i<players;i++)
  {
	//send sin of right
	if (i == 0) 	len = send (p[i], (struct playerInfo *)&current[players-1], sizeof(current[players-1]),0);
	else			len = send (p[i], (struct playerInfo *)&current[i-1], sizeof(current[i-1]),0);
  }

if (hops <= 0)
{printf("Hops should be greater than 0\n"); exit(1);}

int *mypotato = NULL;
mypotato = (int*) malloc (sizeof (int) * (hops + 1)); //hops rem, trace 1, trace 2.......... trace n
mypotato[0] = hops;
//printf("%d\n",mypotato[0]);
  /* Make all sockets non blocking */
  int status;
  status = fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) perror("calling fcntl");
  for(i=0; i<players;i++)
  {
	  status = fcntl(p[i], F_SETFL, fcntl(p[i], F_GETFL, 0) | O_NONBLOCK);
	  if (status == -1) perror("calling fcntl");
  }
/* Select a random process & pass the potato to it */
  srand(players);
  int first = rand() % randmax;
  printf ("All players present, sending potato to player %d\n", first);
  len = send (p[first], (int*)mypotato, sizeof (int) * (hops + 1),0);
  if ( len < 0 ) {
    perror("send potato");
    exit(1);
  }
  while (1)
  {
	  len =0;
	  for (i=0; i<players;i++)
	  {
	    len = recv(p[i], (int *)mypotato, sizeof (int) * (hops + 1),0);
	  
	  if (len>0) 
	  {
	  printf("Trace of potato:\n");
	  for (i=1; i<=(hops-1) ;i++)
            printf("%d,",mypotato[i]);
            printf("%d\n",mypotato[i]);
	  for (i=0; i<players;i++)
          {
   	    len = send (p[i], (int*)mypotato, sizeof (int) * (hops + 1),0);
	    close(p[i]);
	   }
	  //printf("\n>> Connection closed\n\n");
	  close(s);
	  exit(0);
	  }
	  }
  }
}

/*........................ end of master.c ..................................*/
