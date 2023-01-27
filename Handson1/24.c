/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que :24. Write a program to create an orphan process.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
       int pid = fork(); // created a child process
       if(pid == 0) {
             printf("Before orphan parent id is :%d\n", getppid());
	     
	     sleep(3); 
	     
	     printf("After orphan process parnet id is : %d\n", getppid());
       }
       else {
          sleep(1);
	  printf("parent pid is : %d\n", getpid());
       }
}
