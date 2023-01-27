/*
    Name : Prafull Pandey
    Roll no : MT2022151
    
    16. Write a program to perform mandatory locking.
     a. Implement write lock

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
       struct flock lock;
       int fd;
       fd=open("database", O_CREAT | O_RDWR, 0600);
       lock.l_type=F_WRLCK;
       lock.l_whence=SEEK_SET;
       lock.l_start=0;
       lock.l_len=0;
       lock.l_pid=getpid();
       printf("Before entering into critical section\n");
       fcntl(fd,F_SETLKW,&lock); // set locking
       printf("Inside the critical section.....\n");
       printf("Enter to unlock\n");
       getchar(); // enter character to get out of critical section
       printf("unlocked\n");
       lock.l_type=F_UNLCK; // release the lock
       fcntl(fd,F_SETLK,&lock); // F_SETLK = acquire unlock and put it there
       printf("finish\n");

}
