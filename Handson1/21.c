/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 21. Write a program, call fork and print the parent and child process id.
*/

#include <stdio.h>
#include <unistd.h>

int main() {
   int pid = fork();
   if(pid == 0)
      printf("child process id is :%d\n", getpid());
   else 
      printf("parent process id is: %d\n", getpid());
}
