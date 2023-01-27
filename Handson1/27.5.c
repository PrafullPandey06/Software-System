/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que: 27. Write a program to execute ls -Rl by the following system calls
       e.execvp

*/
#include<stdio.h>
#include<unistd.h>

void main()
{
	char * argv[]={"ls","-Rl",NULL};
	execvp("ls",argv);
}	

