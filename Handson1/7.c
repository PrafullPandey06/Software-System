/*
  Name : Prafull Pandey
  Roll No : MT2022151
  
  Que : 7. Write a program to copy file1 into file2 ($cp file1 file2).
*/


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void main() {
          int fd,fd1,size,len;
	  char buf[50];
	  len=sizeof(buf);
	  fd=open("file1",O_RDONLY);
	  fd1=open("file2", O_CREAT|O_RDWR|O_APPEND,0644);

	  while(len != 0 && (size=read(fd,buf,len))!=0) {
                     if(size==-1) {
                         perror("read");
		     }
               write(fd1,buf,size);
	  }
          close(fd);
          close(fd1);
}
