/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que: 27. Write a program to execute ls -Rl by the following system calls
       c.execle

*/

#include<stdio.h>
#include<unistd.h>

void main()
{
	char * Prafull[]={"PATH=/bin:/usr/bin","TERM=xterm-256color",NULL};
	
	execle("/bin/ls","ls","-Rl",(char *)0,Prafull);
}
