/*
    Name : Prafull Pandey
    Roll no: MT2022151
    
    Que : 25. Write a program to create three child processes. The parent should wait for a particular child (use
waitpid system call).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void main() {

  int i,j,k;
  
  i = fork();
  if(i == 0) {
    sleep(1);
    printf("I'm 'i' child");
   
  }
  if(i > 0) {
      j = fork();
      if(j == 0) {
        sleep(4);
        printf("I'm 'j' child");
       
      }
      
      if(j > 0) {
          k = fork();
          if(k == 0) {
             sleep(12);
             printf("I'm 'k' child");
           }
           else {
              waitpid(k,NULL,0);
           }
      }
      
  }
  
  


}
