/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que : 22. Write a program, open a file, call fork, and then write to the file by both the child as well as the
parent processes. Check output of the file.
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
     int fd = open("test", O_CREAT|O_RDWR, 0666);
     int pid = fork();

     if(pid == 0) {  // child process
       write(fd,"mai hu child process\n",20);
     } 
     else if(pid > 0) {
       write(fd,"mai hu parent process\n",21);
    }
    else {
      printf("error occured\n");
    }
}
