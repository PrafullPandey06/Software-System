/*
    Name : Prafull Pandey
    Roll no: MT2022151
    
    Que : /*
    Name : Prafull Pandey
    Roll no: MT2022151
    
    Que : 29. Write a program to get scheduling policy and modify the scheduling policy (SCHED_FIFO, SCHED_RR).

*/

#include <stdio.h>
#include <unistd.h>
#include <sched.h>

int main() {
    
    int curr_policy = sched_getscheduler(0);
    printf("current scheduling policy of this program is :%d\n", curr_policy);

    
   struct sched_param merastruct;
   merastruct.sched_priority = 10;

    int changed_priority = sched_setscheduler(0,SCHED_FIFO,&merastruct);
    if(!changed_priority) {
         printf("policy is changed\n");
    }
    else {
         printf("some error occured\n");
    }

    
   int new_policy = sched_getscheduler(0);
   printf("new scheduling policy of this program is :%d\n", new_policy);
      
}

// use sudo ./a.out
