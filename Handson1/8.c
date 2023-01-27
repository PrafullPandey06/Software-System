/*
  Name : Prafull Pandey
  Roll No : MT2022151
  
  Que : 8. Write a program to open a file in read only mode, read line by line and display each line as it is read. Close the file when end of file is reached.
*/


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void main() {
      char c;
      int fd;
      fd = open("file", O_RDONLY);
      
      while(read(fd,&c,1))
      {
          if(c == '\n')
              getchar();
          else
             printf("%c",c);
      }
      close(fd);
}
