/*
   Name : Prafull Pandey
   Roll no. : MT2022151
   
   Question 2. Write a simple program to execute in an infinite loop at the background. Go to /proc   directory and identify all the process related information in the corresponding proc directory.

*/

#include <stdio.h>

void main() {

   for(;;) {}
   
   // compile : cc 2.c
   // run : ./a.out & (to run in the background)
   // it will return a inode number let say 'c'
   // cd /proc/c
   // now do ls
}
