/*
  Name : Prafull Pandey
  Roll No : MT2022151
  
  Que : 6. Write a program to take input from STDIN and display on STDOUT. Use only read/write system calls
*/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

void main() {
   char buff[1024];

   read(0,buff,10);
   write(1,buff,10);
}
