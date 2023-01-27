/*
    Name : Prafull Pandey
    Roll no: MT2022151
    
    Que : 9. Write a program to print the following information about a given file.
 a. inode
 b. number of hard links
 c. uid
 d. gid
 e. size
 f. block size
 g. number of blocks
 h. time of last access
 i. time of last modification
 j. time of last change
*/


#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>

void main() {
   
  struct stat info;
  if(stat("db", &info) != 0)
     perror("stat()error");
  else {
    printf("stat() returned following info about file: \n");
    printf("inode : %d\n", (int) info.st_ino);
    printf("no of hard links : %d\n", (int)info.st_nlink);
    printf("uid : %d\n", (int) info.st_uid);
    printf("gid : %d\n", (int) info.st_gid);
    printf("size : %d\n", (int) info.st_size);
    printf("block size : %d\n", (int) info.st_blksize);
    printf("no of blocks : %d\n", (int) info.st_blocks);
    printf("time of last access : %d\n", (int) info.st_atime);
    printf("time of latest modification : %d\n", (int) info.st_mtime);
    printf("time of last change : %d\n", (int) info.st_ctime);
    

  }
}
