/*
    Name : Prafull Pandey
    Roll no : MT2022151
    
    16. Write a program to perform mandatory locking.
     b. Implement read lock

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
       struct flock lock;
       int fd;
       fd=open("database", O_RDWR);
       lock.l_type=F_RDLCK;
       lock.l_whence=SEEK_SET;
       lock.l_start=0;
       lock.l_len=0;
       lock.l_pid=getpid();
       printf("Before entering into critical section\n");
       fcntl(fd,F_SETLKW,&lock); // lock
       printf("Inside the critical section....\n");
       printf("Enter to unlock...\n");
       getchar(); 
       printf("Unlocked\n");
       lock.l_type=F_UNLCK; // now unlock 
       fcntl(fd,F_SETLK,&lock);
       printf("finish\n");

}
