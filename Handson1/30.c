/*
  Name : Prafull Pandey
  Roll no : MT2022151
  
  Que : 30 write a program to run a script at a specific time using daemon process
*/

#include <time.h>   
#include <stdio.h>     
#include <stdlib.h>    
#include <sys/types.h> 
#include <unistd.h>    
#include <sys/stat.h> 

// Argumnet to be passed as hour minute second
// hour is mandatory

void main(int argc, char *argv[])
{

    time_t currentEpoch, deadlineEpoch;  
    struct tm *deadline;            

    pid_t child;

    if (argc < 2)
        printf("Pass one argument atleast\n");
    else
    {
        time(&currentEpoch); // Get current time
        deadline = localtime(&currentEpoch);

        deadline->tm_hour = atoi(argv[1]);
        deadline->tm_min = argv[2] == NULL ? 0 : atoi(argv[2]);
        deadline->tm_sec = argv[3] == NULL ? 0 : atoi(argv[3]);

        deadlineEpoch = mktime(deadline); // convert daeadline to epoch

        if ((child = fork()) == 0)
        {
        
            setsid();
            chdir("/");
            umask(0);
            do
            {
                time(&currentEpoch);
            } while (difftime(deadlineEpoch, currentEpoch) > 0);
            printf("success!\n");
            exit(0);
        }
        exit(0);
    }
}

