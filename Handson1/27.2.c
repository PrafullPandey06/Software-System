/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que: 27. Write a program to execute ls -Rl by the following system calls
       b. execlp

*/
#include<stdio.h>
#include<unistd.h>

void main()
{
	execlp("ls","ls","-Rl",(char *)0);
}	

