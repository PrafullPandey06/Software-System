/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 17 database for que 17
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
       int fd;
      
       struct {
             int ticket_no;
       } db;
       db.ticket_no=10;
       fd=open("db", O_CREAT|O_RDWR, 0744);
       write(fd,&db,sizeof(db)); 
       close(fd); 
       fd=open("db",O_RDONLY); 
       read(fd,&db,sizeof(db)); 
       printf("Ticket No : %d\n", db.ticket_no);
       close(fd);

}
