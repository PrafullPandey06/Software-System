/*
   
   Name : Prafull Pandey
   Roll no : MT2022151
   
    Que : 12. Write a program to find out the opening mode of a file. Use fcntl.


*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void main() {
    int fd, x, accessmode;
    fd = open("file1", O_RDONLY);
    x = fcntl(fd, F_GETFL);
    accessmode = x & O_ACCMODE;
    
    if(accessmode == O_RDONLY) {
       printf("read only");
    } 
    if(accessmode == O_RDWR) {
       printf("read write only");
    } 
    if(accessmode == O_WRONLY) {
       printf("read only");
    } 
      
}
