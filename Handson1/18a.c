/*
    Name : Prafull Pandey
    Roll no : MT2022151
    
    Que : 18. Write a program to perform Record locking.
    a. Implement write lock
   
    Create three records in a file. Whenever you access a particular record, first lock it then modify/access
    to avoid race condition.
*/


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

struct {
        int train_no;
        int ticket_no;
      } db;
      
void main() {

           int fd, input;
           fd = open("database",O_RDWR);
           printf("Select train number (1,2,3) : \n");
           scanf("%d", &input);
           
           struct flock lock;
           lock.l_type = F_WRLCK;
           lock.l_whence = SEEK_SET;
           lock.l_start = (input-1)*sizeof(db);
           lock.l_len = sizeof(db);
           lock.l_pid = getpid();
           
           lseek(fd, (input-1) * sizeof(db), SEEK_SET);
           read(fd, &db, sizeof(db));
           printf("Before entering into critical section");
           
           fcntl(fd, F_SETLKW, &lock);
           printf("Ticket no is :%d\n", db.ticket_no);
           db.ticket_no++;
           lseek(fd, -1 * sizeof(db), SEEK_CUR);
           write(fd, &db, sizeof(db));
           printf("To book ticket,press enter : \n");
           getchar();
           getchar();
           lock.l_type = F_UNLCK;
           fcntl(fd, F_SETLK, &lock);
           printf("Ticket Booked\n");
}      
