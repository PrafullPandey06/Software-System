/*
    Name : Prafull Pandey
    Roll no: MT2022151
    
    Que : 28. Write a program to get maximum and minimum real time priority.
*/

#include <stdio.h>
#include <sched.h>
#include <unistd.h>

int main() {
  

  int rr_max = sched_get_priority_max(SCHED_RR);
  int rr_min = sched_get_priority_min(SCHED_RR);
  printf("minimum priority of Round Robin is:%d\n",rr_min);
  printf("maximum priority of Round Robin is:%d\n",rr_max);

  int fifo_max = sched_get_priority_max(SCHED_FIFO);
  int fifo_min = sched_get_priority_min(SCHED_FIFO);
  printf("minimum priority of FIFO is:%d\n",fifo_min);
  printf("maximum priority of FIFO is :%d\n", fifo_max);
  
  int other_max = sched_get_priority_max(SCHED_OTHER);
  int other_min = sched_get_priority_min(SCHED_OTHER);
  printf("minimum priority of OTHER is:%d\n",other_min);
  printf("maximum priority of OTHER is :%d\n", other_max);
}
