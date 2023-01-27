/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que 3. Write a program to create a file and print the file descriptor value. Use creat ( ) system call

*/


#include <stdio.h>
#include <fcntl.h>

void main() {
    int fd;
    fd = creat("file",0744);
    printf("fd :%d\n", fd);

}
