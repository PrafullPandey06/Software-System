/*
   Name : Prafull Pandey
   Roll no: MT2022151
   
   Question:-Write a program to execute an executable program.
   a. use some executable program

*/
#include <unistd.h>
#include<stdio.h>
void main(int argc , char * argv[])
{
	char * args[]={"./26a",argv[1],NULL};
	execve(args[0],args,0);
	
	printf("After execve\n");
	
}

