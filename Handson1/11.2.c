/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   11. Write a program to open a file, duplicate the file descriptor and append the file with both the descriptors and check whether the file is updated properly or not.

b. use dup2

*/


#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
int fd1,fd2;
fd1=open("file1",O_RDWR|O_APPEND);
printf("%d",fd1);
fd2=dup2(fd1,5);
printf("%d",fd2);
write(fd1,"kya hal chal",11);
write(fd2,"sab thik",8);
close(fd1);
close(fd2);

return 0;

}

