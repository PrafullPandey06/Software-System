#ifndef COMMON_FUNCTIONS
#define COMMON_FUNCTIONS


/***************************ACCOUNT INFORMATION STRUCTURE***********************************/
#define ACC_Record

#define M_TXN 10

struct Acc
{
    int accNumber;  
    //acc_own will store the information regarding the owner of the account   
    int acc_own[2];   
    //account can either be regular or joint      
    bool acc_isRegularAcc; 
    //account can either be active or deactive
    bool active;    
    //acc_bal of account      
    long int acc_bal;   
    //store information related to transactions   
    int transactions[M_TXN];  
};


/****************************CUSTOMER INFORMATION STRUCTURE***********************************/
#define CUST_Record

struct Cust
{
    int id; 
    char name[25];
    char gender; 
    int age;
    char login[30]; 
    char password[30];
    int acc; 
};


/*****************************TRANSACTION INFORMATION STRUCTURE*******************************/
#define TXN

#include <time.h>

struct Transaction
{
    int transactionID;
    int accNumber;
    bool operation; 
    long int oldacc_bal;
    long int newacc_bal;
    time_t transactionTime;
};



/**************************************ADMIN INFORMATION****************************************/
#define ADMIN_CREDENTIALS

#define ADMIN_LOGIN_ID "prafull"
#define ADMIN_PASSWORD "66WrdC8pl7zYM" // "420boo69"


#include <stdio.h>     
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>    
#include <stdbool.h>   
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`



#include "./stables.h"

/*************************************Function Prototypes***************************************/

//FOR HANDLINIG THE LOGIN
bool login_administrator(bool isAdmin, int fd1, struct Cust *ptrToCust);

//TO GET ACCOUNT RELATED INFORMATION
bool get_acc_details(int fd1, struct Acc *custAcc);

//TO GET CUSTOMER RELATED INFORMtion
bool get_cust_details(int fd1, int custID);


/*************************************Function Definition***************************************/

bool login_administrator(bool isAdmin, int fd1, struct Cust *ptrToCustID)
{
    ssize_t R_bytes, W_bytes;             // Number of bytes written to / read from the socket
    char R_buff[500]={0};
    char W_buff[500]={0};        // Buffer for reading from / writing to the client
    char tempBuffer[500];
    struct Cust cust;

    int ID;

    //to print the welcome message for the type of user(whether user is admin/customer)
    if (isAdmin)
        strcpy(W_buff, "Welcome admin !Enter your credentials to continue");
    else
        strcpy(W_buff, "Welcome dear customer! Enter your credentials to gain access to your account!");

    // To join the request for LOGIN ID message(concatenates string2 to string1 and ends the resulting string with the null character)
    strcat(W_buff, "\n");
    strcat(W_buff, "Enter your login ID");


    W_bytes = write(fd1, W_buff, strlen(W_buff));
    if (W_bytes == -1)
    {
        perror("Error writing WELCOME & LOGIN_ID message to the client!");
        return 0;
    }

    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        perror("Error reading login ID from client!");
        return 0;
    }

    bool userFound = 0;

    if (isAdmin)
    {
        if (strcmp(R_buff, ADMIN_LOGIN_ID) == 0)
            userFound = 1;
    }
    else
    {
        bzero(tempBuffer, sizeof(tempBuffer));
        strcpy(tempBuffer, R_buff);
        strtok(tempBuffer, "-");
        ID = atoi(strtok(NULL, "-"));

        int custFileFD = open(CUSTOMER_FILE, O_RDONLY);
        if (custFileFD == -1)
        {
            perror("Error opening cust file in read mode!");
            return 0;
        }

        off_t offset = lseek(custFileFD, ID * sizeof(struct Cust), SEEK_SET);
        if (offset >= 0)
        {
            struct flock lock = {F_RDLCK, SEEK_SET, ID * sizeof(struct Cust), sizeof(struct Cust), getpid()};

            int lockingStatus = fcntl(custFileFD, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on cust record!");
                return 0;
            }

            R_bytes = read(custFileFD, &cust, sizeof(struct Cust));
            if (R_bytes == -1)
            {
                ;
                perror("Error reading cust record from file!");
            }

            lock.l_type = F_UNLCK;
            fcntl(custFileFD, F_SETLK, &lock);

            if (strcmp(cust.login, R_buff) == 0)
                userFound = 1;

            close(custFileFD);
        }
        else
        {
            W_bytes = write(fd1, CUSTOMER_LOGIN_ID_DOESNT_EXIT, strlen(CUSTOMER_LOGIN_ID_DOESNT_EXIT));
        }
    }

    if (userFound)
    {
        bzero(W_buff, sizeof(W_buff));
        W_bytes = write(fd1, PASSWORD, strlen(PASSWORD));
        if (W_bytes == -1)
        {
            perror("Error writing PASSWORD message to client!");
            return 0;
        }

        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == 1)
        {
            perror("Error reading password from the client!");
            return 0;
        }

        char hashedPassword[500];
        strcpy(hashedPassword, crypt(R_buff, SALT_BAE));

        if (isAdmin)
        {
            if (strcmp(hashedPassword, ADMIN_PASSWORD) == 0)
                return 1;
        }
        else
        {
            if (strcmp(hashedPassword, cust.password) == 0)
            {
                *ptrToCustID = cust;
                return 1;
            }
        }

        bzero(W_buff, sizeof(W_buff));
        W_bytes = write(fd1, INVALID_PASSWORD, strlen(INVALID_PASSWORD));
    }
    else
    {
        bzero(W_buff, sizeof(W_buff));
        W_bytes = write(fd1, INVALID_LOGIN, strlen(INVALID_LOGIN));
    }

    return 0;
}

bool get_acc_details(int fd1, struct Acc *custAcc)
{
    ssize_t R_bytes, W_bytes;            // Number of bytes read from / written to the socket
    char R_buff[500], W_buff[500]; // A buffer for reading from / writing to the socket
    char tempBuffer[500];

    int accNumber;
    struct Acc acc;
    int accFileDescriptor;

    if (custAcc == NULL)
    {

        W_bytes = write(fd1, GET_ACCOUNT_NUMBER, strlen(GET_ACCOUNT_NUMBER));
        if (W_bytes == -1)
        {
            perror("Error writing GET_ACCOUNT_NUMBER message to client!");
            return 0;
        }

        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            perror("Error reading acc number response from client!");
            return 0;
        }

        accNumber = atoi(R_buff);
    }
    else
        accNumber = custAcc->accNumber;

    accFileDescriptor = open(ACCOUNT_FILE, O_RDONLY);
    if (accFileDescriptor == -1)
    {
        // Acc record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ACCOUNT_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        perror("Error opening acc file in get_acc_details!");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            perror("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }

    int offset = lseek(accFileDescriptor, accNumber * sizeof(struct Acc), SEEK_SET);
    if (offset == -1 && errno == EINVAL) // invalid argument
    {
        // Acc record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ACCOUNT_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        perror("Error seeking to acc record in get_acc_details!");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            perror("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required acc record!");
        return 0;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Acc), getpid()};

    int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error obtaining read lock on acc record!");
        return 0;
    }

    R_bytes = read(accFileDescriptor, &acc, sizeof(struct Acc));
    if (R_bytes == -1)
    {
        perror("Error reading acc record from file!");
        return 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(accFileDescriptor, F_SETLK, &lock);

    if (custAcc != NULL)
    {
        *custAcc = acc;
        return 1;
    }

    bzero(W_buff, sizeof(W_buff));
    sprintf(W_buff, "Acc Details - \n\tAcc Number : %d\n\tAcc Type : %s\n\tAcc Status : %s", acc.accNumber, (acc.acc_isRegularAcc ? "Regular" : "Joint"), (acc.active) ? "Active" : "Deactived");
    if (acc.active)
    {
        sprintf(tempBuffer, "\n\tAcc acc_bal:₹ %ld", acc.acc_bal);
        strcat(W_buff, tempBuffer);
    }

    sprintf(tempBuffer, "\n\tPrimary Owner ID: %d", acc.acc_own[0]);
    strcat(W_buff, tempBuffer);
    if (acc.acc_own[1] != -1)
    {
        sprintf(tempBuffer, "\n\tSecondary Owner ID: %d", acc.acc_own[1]);
        strcat(W_buff, tempBuffer);
    }

    strcat(W_buff, "\n^");

    W_bytes = write(fd1, W_buff, strlen(W_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read

    return 1;
}

bool get_cust_details(int fd1, int custID)
{
    ssize_t R_bytes, W_bytes;             // Number of bytes read from / written to the socket
    char R_buff[500], W_buff[5000]; // A buffer for reading from / writing to the socket
    char tempBuffer[500];

    struct Cust cust;
    int custFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Acc), getpid()};

    if (custID == -1)
    {
        W_bytes = write(fd1, GET_CUSTOMER_ID, strlen(GET_CUSTOMER_ID));
        if (W_bytes == -1)
        {
            perror("Error while writing GET_CUSTOMER_ID message to client!");
            return 0;
        }

        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            perror("Error getting cust ID from client!");
            ;
            return 0;
        }

        custID = atoi(R_buff);
    }

    custFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (custFileDescriptor == -1)
    {
        // Cust File doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, CUSTOMER_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    int offset = lseek(custFileDescriptor, custID * sizeof(struct Cust), SEEK_SET);
    if (errno == EINVAL)
    {
        // Cust record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, CUSTOMER_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required cust record!");
        return 0;
    }
    lock.l_start = offset;

    int lockingStatus = fcntl(custFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Cust file!");
        return 0;
    }

    R_bytes = read(custFileDescriptor, &cust, sizeof(struct Cust));
    if (R_bytes == -1)
    {
        perror("Error reading cust record from file!");
        return 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(custFileDescriptor, F_SETLK, &lock);

    bzero(W_buff, sizeof(W_buff));
    sprintf(W_buff, "Cust Details - \n\tID : %d\n\tName : %s\n\tGender : %c\n\tAge: %d\n\tAcc Number : %d\n\tLoginID : %s", cust.id, cust.name, cust.gender, cust.age, cust.acc, cust.login);

    strcat(W_buff, "\n\nYou'll now be redirected to the main menu...^");

    W_bytes = write(fd1, W_buff, strlen(W_buff));
    if (W_bytes == -1)
    {
        perror("Error writing cust info to client!");
        return 0;
    }

    R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
    return 1;
}

bool get_transaction_details(int fd1, int accNumber)
{

    ssize_t R_bytes, W_bytes;                               // Number of bytes read from / written to the socket
    char R_buff[500], W_buff[5000], tempBuffer[500]; // A buffer for reading from / writing to the socket

    struct Acc acc;

    if (accNumber == -1)
    {
        // Get the accNumber
        W_bytes = write(fd1, GET_ACCOUNT_NUMBER, strlen(GET_ACCOUNT_NUMBER));
        if (W_bytes == -1)
        {
            perror("Error writing GET_ACCOUNT_NUMBER message to client!");
            return 0;
        }

        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            perror("Error reading acc number response from client!");
            return 0;
        }

        acc.accNumber = atoi(R_buff);
    }
    else
        acc.accNumber = accNumber;

    if (get_acc_details(fd1, &acc))
    {
        int iter;

        struct Transaction transaction;
        struct tm transactionTime;

        bzero(W_buff, sizeof(R_buff));

        int transactionFileDescriptor = open(TRANSACTION_FILE, O_RDONLY);
        if (transactionFileDescriptor == -1)
        {
            perror("Error while opening transaction file!");
            write(fd1, TXN_NOT_FOUND, strlen(TXN_NOT_FOUND));
            read(fd1, R_buff, sizeof(R_buff)); // Dummy read
            return 0;
        }

        for (iter = 0; iter < M_TXN && acc.transactions[iter] != -1; iter++)
        {

            int offset = lseek(transactionFileDescriptor, acc.transactions[iter] * sizeof(struct Transaction), SEEK_SET);
            if (offset == -1)
            {
                perror("Error while seeking to required transaction record!");
                return 0;
            }

            struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Transaction), getpid()};

            int lockingStatus = fcntl(transactionFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on transaction record!");
                return 0;
            }

            R_bytes = read(transactionFileDescriptor, &transaction, sizeof(struct Transaction));
            if (R_bytes == -1)
            {
                perror("Error reading transaction record from file!");
                return 0;
            }

            lock.l_type = F_UNLCK;
            fcntl(transactionFileDescriptor, F_SETLK, &lock);

            transactionTime = *localtime(&(transaction.transactionTime));

            bzero(tempBuffer, sizeof(tempBuffer));
            sprintf(tempBuffer, "Details of transaction %d - \n\t Date : %d:%d %d/%d/%d \n\t Operation : %s \n\t acc_bal - \n\t\t Before : %ld \n\t\t After : %ld \n\t\t Difference : %ld\n", (iter + 1), transactionTime.tm_hour, transactionTime.tm_min, transactionTime.tm_mday, transactionTime.tm_mon, transactionTime.tm_year, (transaction.operation ? "Deposit" : "Withdraw"), transaction.oldacc_bal, transaction.newacc_bal, (transaction.newacc_bal - transaction.oldacc_bal));

            if (strlen(W_buff) == 0)
                strcpy(W_buff, tempBuffer);
            else
                strcat(W_buff, tempBuffer);
        }

        close(transactionFileDescriptor);

        if (strlen(W_buff) == 0)
        {
            write(fd1, TXN_NOT_FOUND, strlen(TXN_NOT_FOUND));
            read(fd1, R_buff, sizeof(R_buff)); // Dummy read
            return 0;
        }
        else
        {
            strcat(W_buff, "^");
            W_bytes = write(fd1, W_buff, strlen(W_buff));
            read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        }
    }
}
// =====================================================

#endif
