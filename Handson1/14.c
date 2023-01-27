/*
  Name : Prafull Pandey 
  Roll no : MT2022151

  Que: 14 Write a program to find the type of a file.
a. Input should be taken from command line.
b. program should be able to identify any type of a file.
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<time.h>
int main(int argc, char *argv[])
{
struct stat x;
if (argc != 2)
{
printf("Usage: %s <pathname>\n", argv[0]);
exit(1);
}
if (stat(argv[1], &x) == -1)
{
perror("stat");
exit(2);
}
printf("File type:");
switch (x.st_mode& S_IFMT)
{
      case S_IFBLK:
      printf("block device file\n");
      break;

      case S_IFCHR:
      printf("character device file\n");
      break;

      case S_IFDIR:
      printf("directory\n");
      break;

      case S_IFIFO:
      printf("FIFO/pipe\n");
      break;

      case S_IFLNK:
      printf("symlink\n");
      break;

      case S_IFREG:
      printf("regular file\n");
      break;

      case S_IFSOCK:
      printf("socket\n");
      break;

      default:
      printf("unknown?\n");
      break;
}
}
