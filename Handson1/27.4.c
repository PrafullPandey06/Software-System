/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que: 27. Write a program to execute ls -Rl by the following system calls
       d.execv

*/

#include<stdio.h>
#include<unistd.h>

void main()
{
	char * argv[]={"ls","-Rl",NULL};
	execv("/bin/ls",argv);
}
