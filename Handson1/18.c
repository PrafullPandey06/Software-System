/*
   Name : Prafull Pandey
   Roll no  : MT2022151
   
   this is database file for Que 18
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    struct {
            int train_num;
	    int ticket_count;
          } db[3]; 
   for(int i=0; i<3; i++) {
      db[i].train_num = i+1;
      db[i].ticket_count = 0;
   }
  
  fd=open("database", O_CREAT|O_RDWR , 0744);
  write(fd, db, sizeof(db)); 
  close(fd);
}
