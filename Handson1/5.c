/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 5. Write a program to create five new files with infinite loop. Execute the program in the background and check the file descriptor table at /proc/pid/fd.
*/

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
	int i0=open("./newfile0",O_CREAT, 0744);
	int i1=open("./newfile1",O_CREAT, 0744);
	int i2=open("./newfile2",O_CREAT, 0744);
	int i3=open("./newfile3",O_CREAT, 0744);
	int i4=open("./newfile4",O_CREAT, 0744);
	while(1);
}
