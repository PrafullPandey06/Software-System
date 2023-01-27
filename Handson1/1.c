/*
    Name : Prafull Pandey
    Roll No. MT2022151
    
    Question : 1. Create the following types of a files using (i) shell command (ii) system call
    a. soft link (symlink system call)
    b. hard link (link system call)
    c. FIFO (mkfifo Library Function or mknod system call
*/


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main() {
  symlink("targetfile", "symlink");
  perror("link soft created");
  link("db", "Hardlink");
  perror("hardlink  created");
  mkfifo("sysfifo");
}
