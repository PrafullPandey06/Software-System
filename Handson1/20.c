/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 20. Find out the priority of your running program. Modify the priority with nice command.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>

void main() {
    printf("priority of current process is :%d\n", getpriority(PRIO_PROCESS,0));
    nice(-2);
    printf("modified priority of current process is :%d\n", getpriority(PRIO_PROCESS,0));
    
    setpriority(PRIO_PROCESS,0,2);
    printf("modified priority of current process is :%d\n", getpriority(PRIO_PROCESS,0));
}
