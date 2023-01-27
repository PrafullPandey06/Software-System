/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 4. Write a program to open an existing file with read write mode. Try O_EXCL flag also.
*/


#include<stdio.h>
#include<fcntl.h>

void main() {
 int fd = open("exp",O_RDWR | O_EXCL, 7421);
 printf("%d\n", fd);
}
