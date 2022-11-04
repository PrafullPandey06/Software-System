
                              /*************************************************server.c file ***********************************************/

#include <stdio.h> 
#include <errno.h> 
#include <fcntl.h>     

#include <unistd.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/ip.h> 

#include <string.h>  
#include <stdbool.h> 
#include <stdlib.h> 

#include "./stables.h"
#include "./admin.h"
#include "./cust.h"

void connection_handler(int fd1); // Handles the communication with the client

void main()
{
    int socketFd, socketBindStatus, socketListenStatus, connFD;
    struct sockaddr_in serverAddress, clientAddress;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        printf("Error while creating server socket!");
        _exit(0);
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(8081);              // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

    socketBindStatus = bind(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (socketBindStatus == -1)
    {
        printf("Error while binding to server socket!");
        _exit(0);
    }

    socketListenStatus = listen(socketFd, 10);
    if (socketListenStatus == -1)
    {
        printf("Error while listening for connections on the server socket!");
        close(socketFd);
        _exit(0);
    }

    int clientSize;
    while (1)
    {
        clientSize = (int)sizeof(clientAddress);
        connFD = accept(socketFd, (struct sockaddr *)&clientAddress, &clientSize);
        if (connFD == -1)
        {
            printf("Error while connecting to client!");
            close(socketFd);
        }
        else
        {
            if (!fork())
            {
                // Child will enter this branch
                connection_handler(connFD);
                close(connFD);
                _exit(0);
            }
        }
    }

    close(socketFd);
}

void connection_handler(int connFD)
{
    printf("Client has connected to the server!\n");

    char readBuffer[1000], writeBuffer[1000];
    ssize_t readBytes, writeBytes;
    int userChoice;

    writeBytes = write(connFD, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
    if (writeBytes == -1)
        printf("Error while sending first prompt to the user!");
    else
    {
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
            printf("Error while reading from client");
        else if (readBytes == 0)
            printf("No data was sent by the client");
        else
        {
            userChoice = atoi(readBuffer);
            switch (userChoice)
            {
            case 1:
                // Admin
                admin_operation_handler(connFD);
                break;
            case 2:
                // Cust
                cust_operation_handler(connFD);
                break;
            default:
                // Exit
                break;
            }
        }
    }
    printf("Terminating connection to client!\n");
}
