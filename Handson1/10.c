/*
    Name : Prafull Pandey
    Roll No : MT2022151
    
    Que : 10. Write a program to open a file with read write mode, write 10 bytes, move the file pointer by 10 bytes (use lseek) and write again 10 bytes.
a. check the return value of lseek
b. open the file with od and check the empty spaces in between the data. (od -c seek)

*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void main() {
     int fd;
     fd = open("seek", O_CREAT|O_RDWR,0744);
     write(fd, "writing before seek\n",19);
     lseek(fd,5L, SEEK_CUR);
     write(fd,"now after seek I'm writing\n",26);
     close(fd);
}
