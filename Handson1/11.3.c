/*
   
   Name : Prafull Pandey
   Roll no : MT2022151
   
   11. Write a program to open a file, duplicate the file descriptor and append the file with both the descriptors and check whether the file is updated properly or not.

 c. use fcntl


*/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
int fd1,fd2;
fd1=open("unknownfile1",O_CREAT|O_RDWR|O_APPEND,0744);
printf("%d",fd1);
fd2=fcntl(fd1,F_DUPFD);
printf("%d",fd2);
write(fd1,"kya hal chal",11);
write(fd2,"sab thik",8);
close(fd1);
close(fd2);

return 0;

}

