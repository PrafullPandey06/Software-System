/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 23. Write a program to create a Zombie state of the running program.
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
   int pid = fork();

   if(pid == 0) { 
        exit(0); 
   } else { 
          sleep(100);
   }
  return 0;
}
