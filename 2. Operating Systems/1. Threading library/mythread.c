/*********************************************************************
**	File Name		: MyThread.c
**	Description	: Library to implement Thread routines
**	Created by		: Pallavi Deo on 09/16/2014
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "mythread.h"

/*ASSUMPTION: FIRST THREAD IN READY QUEUE IS THE RUNNING THREAD*/

typedef struct Thr{	
	int JoinCount;
	int JoinFlag; 
	ucontext_t con;
	struct Thr *Parent; // Stores immediate parent
	struct Thr *Child; // Stores the child thread it is waiting for
}Thr;

struct Thr *MainT;

typedef struct Semaphore{
	int s;
}Semaphore;

typedef struct Queue{ 
	struct Thr *Thread;
	struct Queue *Next;
}Queue;
struct Queue *readyQ ,*blockedQ ,*waitQ;

/*__________________ FUNCTIONS TO IMPLEMENT QUEUE __________________*/
void enqueue(Queue *qid, Thr *thread)
{
	//printf("Inside enqueue\n");
	Queue *NewQ = (Queue*) malloc(sizeof(Queue));
	Queue *temp;

	NewQ->Thread = thread;
	NewQ->Next = NULL;

	if (qid == NULL)
	{
		printf("Queue Empty!!!\n");
		qid = NewQ;
		return;
	}
	//read next element till null is found
	temp = qid;
	while(temp->Next != NULL)
		temp = temp->Next;

	temp->Next = NewQ;
	//printf("enqueue successful\n");
}
/*__________________________________________________________________*/
Thr* dequeue(Queue *qid)
{
	//printf("Inside dequeue\n");
	Thr *temp = (Thr*) malloc(sizeof(Thr));
	if (qid == NULL)
	{
		printf("ERROR: DEQUEUE REQUESTED ON EMPTY QUEUE!\n");
		return temp;
	}
	temp = qid->Thread;
	qid = qid->Next;
	return temp;
}
/*__________________________________________________________________*/
/*	Unblock routine- to remove requested node from queue
**	Not standard queue routine, but required for this assignment
**	When performing Exit, this routine is called to unblock
**	any thread that is waiting for exiting thread to finish
**	and enqueue it to readyQ */

void unblock (Thr *unblk)
{
	//printf("Inside unblock\n");
	// No matter where it is present, must be added to readyQ
	enqueue (readyQ, unblk);
	// Now only need to remove its ref from blockedQ
	// If it is the first thread on blockedQ
	if (blockedQ->Thread == unblk)
	{
		blockedQ = blockedQ->Next;
		return;
	}
	// If it is not the first thread on blockedQ
	Queue *temp = (Queue*) malloc(sizeof(Queue));
	temp = blockedQ;

	while (temp->Next != NULL)
	{
		if (temp->Next->Thread == unblk)
			temp->Next = (temp->Next)->Next;
		temp = temp->Next;
	}
	return;
}

/*ннн__________________ UNIX ROUTINE, CALLED BY MAIN __________________*/

void MyThreadInit (void(*start_funct)(void *), void *args)
{
	//printf("mythreadinit\n");
	Thr *FirstT = (Thr*) malloc(sizeof(Thr));
	MainT = (Thr*) malloc(sizeof(Thr));

	FirstT->JoinCount = 0;
	FirstT->JoinFlag = 0;
	FirstT->Parent = NULL;
	FirstT->Child = NULL;
	
	// Get context of the current thread of execution & initialize
	getcontext(&(FirstT->con));
	//printf("mythreadinit- get context done\n");
	FirstT->con.uc_link = NULL;
	FirstT->con.uc_stack.ss_sp = malloc(8*1024*2);
	FirstT->con.uc_stack.ss_size = 8*1024*2;
	// Assigning double of required minimum (8KB)
 
	// Initializing readyQ
	readyQ = (Queue*) malloc(sizeof(Queue));
	readyQ = NULL;
	//printf("\nReady queue created\n");

	// Save this context in FirstT & add FirstT to readyQ
	// As mentioned before, for implementation simplicity,
	// it is assumed that first thread of readyQ is running.

	makecontext(&(FirstT->con),(void(*) (void))start_funct,1,args );
	enqueue(readyQ, FirstT);
	//printf("Enqueue done \n\n");
	swapcontext(&MainT->con, &FirstT->con);
	//printf("Last statement in mythreadinit\n");
}
/*__________________________________________________________________*/
MyThread MyThreadCreate (void(*start_funct)(void*), void *args)
{
	//printf ("mythreadcreate\n%d\n\n",readyQ);

	Thr *UsrThr = (Thr*) malloc(sizeof(Thr));
	UsrThr->JoinCount = 0;
	UsrThr->JoinFlag = 0;
	if (readyQ != NULL)
		UsrThr->Parent = readyQ->Thread;
	else 
		UsrThr->Parent = MainT;

	//running thread is the parent
	UsrThr->Child = NULL;

	/* get context of the current thread of execution*/
	getcontext(&(UsrThr->con));
	//printf("mythreadcreate- get context done\n");
	UsrThr->con.uc_link = NULL;
	UsrThr->con.uc_stack.ss_sp = malloc(8*1024*2);
	UsrThr->con.uc_stack.ss_size = 8*1024*2;

	// create a new thread & adding it to readyQ

	//printf(" mythreadcreate, making new context\n");
	makecontext(&(UsrThr->con),(void(*) (void))start_funct,1,args );
	//printf("\n\n%d\n\n",readyQ);
	enqueue(readyQ,UsrThr);
	return (MyThread) UsrThr;
}
/*__________________________________________________________________*/
void MyThreadYield(void)
{
	Thr *CurThr = (Thr*) malloc(sizeof(Thr));
	Thr *RdyThr = (Thr*) malloc(sizeof(Thr));
	//printf("mythreadyield\n");

	/* 	dequeue will take currently running thread
		& set start of readyQ to the next thread
		Then, swap context among CurThr & RdyThr */
	if (readyQ == NULL)
	{
		printf("no thread to yield to, returning\n");
		return;
	}
	
	// Store current execution context in running thread's ucp
	getcontext(&(readyQ->Thread->con));
	CurThr = dequeue(readyQ);
	//printf("Dequeue successful\n");
	enqueue(readyQ,CurThr);
	RdyThr = readyQ->Thread;
	swapcontext(&CurThr->con,&RdyThr->con);
	//printf ("last statement of mythreadyield\n");
}
/*__________________ JOIN OPERATION ON ONE THREAD __________________*/

int MyThreadJoin(MyThread thread)
{
	//printf("mythreadjoin\n");
	// Check if the thread is immediate child
	struct Thr *temp = (Thr*) malloc(sizeof(Thr));
	temp=thread;
	if (readyQ->Thread != temp->Parent)
	{
		printf("ERROR: THIS IS NOT IMMEDIATE CHILD");
		return -1;
	}
	// Thread is the immediate child. Continue-
	struct Thr *Par = (Thr*) malloc(sizeof(Thr));
	Par = dequeue(readyQ);

	Thr *Rdy = (Thr*) malloc(sizeof(Thr));
	Rdy = readyQ->Thread;

	Par->JoinCount = 1;
	Par->Child = thread;
	temp->JoinFlag = 1;

	enqueue(blockedQ,Par);
	swapcontext(&Par->con,&Rdy->con);
	return 1;
}
/*__________________ JOIN ALL IMMEDIATE CHILDREN ___________________*/

void MyThreadJoinAll(void)
{
	// children would be in blocked or ready Q
	// Set JoinCount of parent to zero
	// Parse each Q, & check the value in parent 
	//	If child found, set joinFlag of child
	//	Also increment joinCount in current thread
	// If no child is present, exit
	// Otherwise, dequeue readyQ, enqueue blocked Q & swapcontext

	//printf("mythreadjoinall\n");
	readyQ->Thread->JoinCount =0;
	struct Thr *Parent = (Thr*) malloc(sizeof(Thr));
	Parent = readyQ->Thread;

	Queue *temp = (Queue*) malloc(sizeof(Queue));
	temp = readyQ->Next;
	while(temp != NULL)
	{
		if(temp->Thread->Parent == Parent)
		{
			temp->Thread->JoinFlag = 1;
			readyQ->Thread->JoinCount +=1;
		}
		temp = temp->Next;
	}
	temp = blockedQ;
	while(temp!= NULL)
	{
		if(temp->Thread->Parent == Parent)
		{
			temp->Thread->JoinFlag = 1;
			readyQ->Thread->JoinCount +=1;
		}
		temp = temp->Next;
	}
	if (Parent->JoinCount == 0)
		return;
	temp->Thread = dequeue(readyQ);
	enqueue(blockedQ, temp->Thread);
	swapcontext(&temp->Thread->con,&readyQ->Thread->con);
}
/*____________ EXIT THREAD: TERMINATES INVOKING THREAD _____________*/
void MyThreadExit(void)
{
	// This thread HAS to exit, so dequeue in any case
	// Check if any thread is waiting for this to finish

	//printf("mythreadexit\n");
	Thr *Current = (Thr*) malloc(sizeof(Thr));
	if (readyQ == NULL)
	{
		if(blockedQ == NULL)
		{//printf("blocked q also null\n");
		swapcontext(&Current->con, &MainT->con);}
		return;
	}
	
	Current = dequeue(readyQ);
		
	if (Current->JoinFlag == 1); // there are threads waiting for it.
	{
		Queue *temp;
		Thr *unblk = (Thr*) malloc(sizeof(Thr));

		temp = blockedQ;
		while (temp != NULL)
		{
			if((temp->Thread->Child != NULL && 
			temp->Thread->Child == Current) ||
			(Current->Parent != NULL &&
			Current->Parent == temp->Thread))
			{
				temp->Thread->JoinCount -= 1;
				temp->Thread->Child = NULL;
				if (temp->Thread->JoinCount == 0)
					unblock(temp->Thread);
			}
			temp = temp->Next;
		}
	}
	swapcontext(&Current->con,&readyQ->Thread->con);
}
/*__________________________________________________________________*/
/* new semaphore*/

MySemaphore MySemaphoreInit(int initialValue)
{
	//printf("mysemaphoreinit\n");

	Semaphore *Sem = (Semaphore*) malloc(sizeof(Semaphore));
	Sem->s = initialValue;
	waitQ = (Queue*) malloc(sizeof(Queue));

	return (MySemaphore)Sem;
}
/*__________________________________________________________________*/
/* Signal semaphore sem. The invoking thread is not pre-empted. */

void MySemaphoreSignal(MySemaphore sem)
{
	Semaphore *semlocal = (Semaphore*) malloc(sizeof(Semaphore));
	if (semlocal->s < 0)
	semlocal->s +=1;
	else
printf("Semaphore already signaled! Checking if there are more occurences in waitQ\n");

Queue *temp = (Queue*) malloc(sizeof(Queue));
Thr *thread = (Thr*) malloc(sizeof(Thr));
temp = waitQ;
while(temp != NULL)
{
	if (temp == readyQ)
	thread = dequeue(waitQ);
	temp = temp->Next;
	}
}
/*__________________________________________________________________*/
/* Wait on semaphore sem. */

void MySemaphoreWait(MySemaphore sem)
{
	Semaphore *semlocal = (Semaphore*) malloc(sizeof(Semaphore));
	if (semlocal->s == 0 )
	{
		printf("Available! No need to wait.\n");
		return;
	}
	semlocal->s -= 1;

	// put the current thread in wait Q
	// dequeue the readyQ & swap context

	Queue *temp = (Queue*) malloc(sizeof(Queue));
	temp->Thread = dequeue(readyQ);
	getcontext(&(temp->Thread->con));
	enqueue(waitQ,temp->Thread);

	swapcontext(&temp->Thread->con,&readyQ->Thread->con);
}
/*__________________________________________________________________*/
/* Destroy semaphore sem. 
Do not destroy semaphore if any threads are blocked on the queue.
Return 0 on success, -1 on failure. */

int MySemaphoreDestroy(MySemaphore sem)
{
	free(sem);
	return 0;
}