/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 17. Write a program to simulate online ticket reservation. Implement write lock
Write a program to open a file, store a ticket number and exit. Write a separate program, to
open the file, implement write lock, read the ticket number, increment the number and print
the new ticket number then close the file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
            struct {
                   int ticket_no;
                   } db;
           struct flock lock;
	   int fd;
	   fd=open("db",O_RDWR); 
	   read(fd,&db,sizeof(db));
	   lock.l_type=F_WRLCK;  
	   lock.l_whence=SEEK_SET;
	   lock.l_start=0;  
	   lock.l_len=0;
	   lock.l_pid=getpid();
	   printf("Before entering into critical section");
	   fcntl(fd,F_SETLKW,&lock);  // set the lock
	   printf("current ticket no :%d\n", db.ticket_no); 
	   db.ticket_no++; 
	   lseek(fd,0,SEEK_SET); 
	   printf("Inside the critical section....\n");
           printf("Enter to unlock...\n");
	   write(fd,&db,sizeof(db)); 
	   getchar();
           lock.l_type=F_UNLCK; 
	   printf("unlocked\n");
           fcntl(fd,F_SETLK,&lock); 
           printf("finish\n");
 
 
}
