/*
   
   Name : Prafull Pandey
   Roll no : MT2022151
   
    Que : 13. Write a program to wait for a STDIN for 10 seconds using select. Write a proper print statement to verify whether the data is available within 10 seconds or not (check in $man 2 select).


*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
   fd_set rfds;  
   struct timeval tv;
   int retval;

   FD_ZERO(&rfds);
   FD_SET(0, &rfds);

   tv.tv_sec=10;
   tv.tv_usec=0;
   
   retval=select(1,&rfds,NULL,NULL,&tv);
  
   if(retval)
         printf("Data is available within 10 seconds...\n");
	 else
	 printf("Data is not available within 10 seconds...\n");
  }
