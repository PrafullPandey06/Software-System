/*
   Name : Prafull Pandey
   Roll no : MT2022151
   
   Que : 15. Write a program to display the environmental variable of the user (use environ).
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
extern char **environ; 

void  main() {
          char **env = environ; // create pointer to environ variable
          for(*env; *env != NULL; env++) {
               printf("%s\n", *env);
	  }
         
}
