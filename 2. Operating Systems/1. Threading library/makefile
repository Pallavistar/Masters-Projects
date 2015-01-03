make: mythread.a

mythread.a: queue.o mythread.o
	ar rvs mythread.a  mythread.o

mythread.o: mythread.c
	gcc -c -g mythread.c
