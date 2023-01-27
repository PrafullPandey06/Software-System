/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 11. Write a program to open a file, duplicate the file descriptor and append the file with both the descriptors and check whether the file is updated properly or not.
a. use dup

*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void main() {
       int fd1, fd2, fd3;
       char buff[50];
       
       fd1 = open("anyfile", O_RDWR);
       fd2 = dup(fd1);
       write(fd1,"witing on fd1\n",13);
       write(fd2,"writing on fd2\n",14);
       
       close(fd1);
       close(fd2);
       
       fd3 = open("anyfile", O_RDWR);
       read(fd3,buff,sizeof(buff));
       printf("%s\n", buff);
}
