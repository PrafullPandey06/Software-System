/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 19. Write a program to find out time taken to execute getpid system call. Use time stamp counter.
*/


#include <stdio.h>
#include <sys/time.h>

unsigned long long  rdtsc() {
  
  unsigned long long dst;
  __asm__ __volatile__("rdtsc":"=A"(dst));
  return dst;
 
 }
 
 int main() {

     long long int start,end;
     start = rdtsc();
     int pid = getpid();
     printf("%d\n",pid);
     end = rdtsc();
     
     printf("Total time taken is : %llu\n", (end-start)/2.3);
}
