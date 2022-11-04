#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "./comm.h"

// Function Prototypes***********************************

bool admin_operation_handler(int fd1);
bool add_acc(int fd1);
int add_cust(int fd1, bool isPrimary, int newAccNumber);
bool delete_acc(int fd1);
bool modify_cust_info(int fd1);


bool admin_operation_handler(int fd1)
{

    if (login_administrator(1, fd1, NULL))
    {
        ssize_t W_bytes, R_bytes;            // Number of bytes read from / written to the client
        char R_buff[500];
        char W_buff[500]; // A buffer used for reading & writing to the client
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ADMIN_LOGIN_SUCCESS);
        while (1)
        {
            strcat(W_buff, "\n");
            strcat(W_buff, ADMIN_MENU);
            W_bytes = write(fd1, W_buff, strlen(W_buff));
            if (W_bytes == -1)
            {
                printf("Error while writing ADMIN_MENU to client!");
                return 0;
            }
            bzero(W_buff, sizeof(W_buff));

            R_bytes = read(fd1, R_buff, sizeof(R_buff));
            if (R_bytes == -1)
            {
                printf("Error while reading client's choice for ADMIN_MENU");
                return 0;
            }

            int choice = atoi(R_buff);
            switch (choice)
            {
            case 1:
                get_cust_details(fd1, -1);
                break;
            case 2:
                get_acc_details(fd1, NULL);
                break;
            case 3: 
                get_transaction_details(fd1, -1);
                break;
            case 4:
                add_acc(fd1);
                break;
            case 5:
                delete_acc(fd1);
                break;
            case 6:
                modify_cust_info(fd1);
                break;
            default:
                W_bytes = write(fd1, ADMIN_LOGOUT, strlen(ADMIN_LOGOUT));
                return 0;
            }
        }
    }
    else
    {
        // ADMIN LOGIN FAILED
        return 0;
    }
    return 1;
}

bool add_acc(int fd1)
{
    ssize_t R_bytes, W_bytes;
    char R_buff[500], W_buff[500];

    struct Acc newAcc, prevAcc;

    int accFileDescriptor = open(ACCOUNT_FILE, O_RDONLY);
    if (accFileDescriptor == -1 && errno == ENOENT)
    {
        // Acc file was never created
        newAcc.accNumber = 0;
    }
    else if (accFileDescriptor == -1)
    {
        printf("Error while opening acc file");
        return 0;
    }
    else
    {
        int offset = lseek(accFileDescriptor, -sizeof(struct Acc), SEEK_END);
        if (offset == -1)
        {
            printf("Error seeking to last Acc record!");
            return 0;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Acc), getpid()};
        int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            printf("Error obtaining read lock on Acc record!");
            return 0;
        }

        R_bytes = read(accFileDescriptor, &prevAcc, sizeof(struct Acc));
        if (R_bytes == -1)
        {
            printf("Error while reading Acc record from file!");
            return 0;
        }

        lock.l_type = F_UNLCK;
        fcntl(accFileDescriptor, F_SETLK, &lock);

        close(accFileDescriptor);

        newAcc.accNumber = prevAcc.accNumber + 1;
    }
    W_bytes = write(fd1, ADMIN_ADD_ACCOUNT_TYPE, strlen(ADMIN_ADD_ACCOUNT_TYPE));
    if (W_bytes == -1)
    {
        printf("Error writing ADMIN_ADD_ACCOUNT_TYPE message to client!");
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, &R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading acc type response from client!");
        return 0;
    }

    newAcc.acc_isRegularAcc = atoi(R_buff) == 1 ? 1 : 0;

    newAcc.acc_own[0] = add_cust(fd1, 1, newAcc.accNumber);

    if (newAcc.acc_isRegularAcc)
        newAcc.acc_own[1] = -1;
    else
        newAcc.acc_own[1] = add_cust(fd1, 0, newAcc.accNumber);

    newAcc.active = 1;
    newAcc.acc_bal = 0;

    memset(newAcc.transactions, -1, M_TXN * sizeof(int)); //memset() is used to fill a block of memory with a particular value.

    accFileDescriptor = open(ACCOUNT_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (accFileDescriptor == -1)
    {
        printf("Error while creating / opening acc file!");
        return 0;
    }

    W_bytes = write(accFileDescriptor, &newAcc, sizeof(struct Acc));
    if (W_bytes == -1)
    {
        printf("Error while writing Acc record to file!");
        return 0;
    }

    close(accFileDescriptor);

    bzero(W_buff, sizeof(W_buff));
    sprintf(W_buff, "%s%d", ADMIN_ADD_ACCOUNT_NUMBER, newAcc.accNumber);
    strcat(W_buff, "\nRedirecting you to the main menu ...^");
    W_bytes = write(fd1, W_buff, sizeof(W_buff));
    R_bytes = read(fd1, R_buff, sizeof(read)); // Dummy read
    return 1;
}

int add_cust(int fd1, bool isPrimary, int newAccNumber)
{
    ssize_t R_bytes, W_bytes;
    char R_buff[500], W_buff[500];

    struct Cust newCust, previousCust;

    int custFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (custFileDescriptor == -1 && errno == ENOENT) // no such file or directory
    {
        // Cust file was never created
        newCust.id = 0;
    }
    else if (custFileDescriptor == -1)
    {
        printf("Error while opening cust file");
        return -1;
    }
    else
    {
        int offset = lseek(custFileDescriptor, -sizeof(struct Cust), SEEK_END);
        if (offset == -1)
        {
            printf("Error seeking to last Cust record!");
            return 0;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Cust), getpid()};
        int lockingStatus = fcntl(custFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            printf("Error obtaining read lock on Cust record!");
            return 0;
        }

        R_bytes = read(custFileDescriptor, &previousCust, sizeof(struct Cust));
        if (R_bytes == -1)
        {
            printf("Error while reading Cust record from file!");
            return 0;
        }

        lock.l_type = F_UNLCK;
        fcntl(custFileDescriptor, F_SETLK, &lock);

        close(custFileDescriptor);

        newCust.id = previousCust.id + 1;
    }

    if (isPrimary)
        sprintf(W_buff, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    else
        sprintf(W_buff, "%s%s", ADMIN_ADD_CUSTOMER_SECONDARY, ADMIN_ADD_CUSTOMER_NAME);

    W_bytes = write(fd1, W_buff, sizeof(W_buff));
    if (W_bytes == -1)
    {
        printf("Error writing ADMIN_ADD_CUSTOMER_NAME message to client!");
        return 0;
    }

    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading cust name response from client!");
        ;
        return 0;
    }

    strcpy(newCust.name, R_buff);

    W_bytes = write(fd1, ADMIN_ADD_CUSTOMER_GENDER, strlen(ADMIN_ADD_CUSTOMER_GENDER));
    if (W_bytes == -1)
    {
        printf("Error writing ADMIN_ADD_CUSTOMER_GENDER message to client!");
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading cust gender response from client!");
        return 0;
    }

    if (R_buff[0] == 'M' || R_buff[0] == 'F' || R_buff[0] == 'O')
        newCust.gender = R_buff[0];
    else
    {
        W_bytes = write(fd1, ADMIN_ADD_CUSTOMER_WRONG_GENDER, strlen(ADMIN_ADD_CUSTOMER_WRONG_GENDER));
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }

    bzero(W_buff, sizeof(W_buff));
    strcpy(W_buff, ADMIN_ADD_CUSTOMER_AGE);
    W_bytes = write(fd1, W_buff, strlen(W_buff));
    if (W_bytes == -1)
    {
        printf("Error writing ADMIN_ADD_CUSTOMER_AGE message to client!");
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading cust age response from client!");
        return 0;
    }

    int custAge = atoi(R_buff);
    if (custAge == 0)
    {
        // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ERRON_INPUT_FOR_NUMBER);
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    newCust.age = custAge;

    newCust.acc = newAccNumber;

    strcpy(newCust.login, newCust.name);
    strcat(newCust.login, "-");
    sprintf(W_buff, "%d", newCust.id);
    strcat(newCust.login, W_buff);

    char hashedPassword[500];
    strcpy(hashedPassword, crypt(AUTOGEN_PASSWORD, SALT_BAE));
    strcpy(newCust.password, hashedPassword);

    custFileDescriptor = open(CUSTOMER_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (custFileDescriptor == -1)
    {
        printf("Error while creating / opening cust file!");
        return 0;
    }
    W_bytes = write(custFileDescriptor, &newCust, sizeof(newCust));
    if (W_bytes == -1)
    {
        printf("Error while writing Cust record to file!");
        return 0;
    }

    close(custFileDescriptor);

    bzero(W_buff, sizeof(W_buff));
    sprintf(W_buff, "%s%s-%d\n%s%s", ADMIN_ADD_CUSTOMER_AUTOGEN_LOGIN, newCust.name, newCust.id, ADMIN_ADD_CUSTOMER_AUTOGEN_PASSWORD, AUTOGEN_PASSWORD);
    strcat(W_buff, "^");
    W_bytes = write(fd1, W_buff, strlen(W_buff));
    if (W_bytes == -1)
    {
        printf("Error sending cust loginID and password to the client!");
        return 0;
    }

    R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read

    return newCust.id;
}

bool delete_acc(int fd1)
{
    ssize_t R_bytes, W_bytes;
    char R_buff[500], W_buff[500];

    struct Acc acc;

    W_bytes = write(fd1, ADMIN_DEL_ACCOUNT_NO, strlen(ADMIN_DEL_ACCOUNT_NO));
    if (W_bytes == -1)
    {
        printf("Error writing ADMIN_DEL_ACCOUNT_NO to client!");
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading acc number response from the client!");
        return 0;
    }

    int accNumber = atoi(R_buff);

    int accFileDescriptor = open(ACCOUNT_FILE, O_RDONLY);
    if (accFileDescriptor == -1)
    {
        // Acc record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ACCOUNT_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }


    int offset = lseek(accFileDescriptor, accNumber * sizeof(struct Acc), SEEK_SET);
    if (errno == EINVAL)
    {
        // Cust record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ACCOUNT_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    else if (offset == -1)
    {
        printf("Error while seeking to required acc record!");
        return 0;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Acc), getpid()};
    int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        printf("Error obtaining read lock on Acc record!");
        return 0;
    }

    R_bytes = read(accFileDescriptor, &acc, sizeof(struct Acc));
    if (R_bytes == -1)
    {
        printf("Error while reading Acc record from file!");
        return 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(accFileDescriptor, F_SETLK, &lock);

    close(accFileDescriptor);

    bzero(W_buff, sizeof(W_buff));
    if (acc.acc_bal == 0)
    {
        // No money, hence can close acc
        acc.active = 0;
        accFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
        if (accFileDescriptor == -1)
        {
            printf("Error opening Acc file in write mode!");
            return 0;
        }

        offset = lseek(accFileDescriptor, accNumber * sizeof(struct Acc), SEEK_SET);
        if (offset == -1)
        {
            printf("Error seeking to the Acc!");
            return 0;
        }

        lock.l_type = F_WRLCK;
        lock.l_start = offset;

        int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            printf("Error obtaining write lock on the Acc file!");
            return 0;
        }

        W_bytes = write(accFileDescriptor, &acc, sizeof(struct Acc));
        if (W_bytes == -1)
        {
            printf("Error deleting acc record!");
            return 0;
        }

        lock.l_type = F_UNLCK;
        fcntl(accFileDescriptor, F_SETLK, &lock);

        strcpy(W_buff, ADMIN_DEL_ACCOUNT_SUCCESS);
    }
    else
        // Acc has some money ask cust to withdraw it
        strcpy(W_buff, ADMIN_DEL_ACCOUNT_FAILURE);
    W_bytes = write(fd1, W_buff, strlen(W_buff));
    if (W_bytes == -1)
    {
        printf("Error while writing final DEL message to client!");
        return 0;
    }
    R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read

    return 1;
}

bool modify_cust_info(int fd1)
{
    ssize_t R_bytes, W_bytes;
    char R_buff[500], W_buff[500];

    struct Cust cust;

    int custID;

    off_t offset;
    int lockingStatus;

    W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_ID, strlen(ADMIN_MOD_CUSTOMER_ID));
    if (W_bytes == -1)
    {
        printf("Error while writing ADMIN_MOD_CUSTOMER_ID message to client!");
        return 0;
    }
    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error while reading cust ID from client!");
        return 0;
    }

    custID = atoi(R_buff);

    int custFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (custFileDescriptor == -1)
    {
        // Cust File doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, CUSTOMER_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    
    offset = lseek(custFileDescriptor, custID * sizeof(struct Cust), SEEK_SET);
    if (errno == EINVAL)
    {
        // Cust record doesn't exist
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, CUSTOMER_ID_DOESNT_EXIT);
        strcat(W_buff, "^");
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }
    else if (offset == -1)
    {
        printf("Error while seeking to required cust record!");
        return 0;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Cust), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(custFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        printf("Couldn't obtain lock on cust record!");
        return 0;
    }

    R_bytes = read(custFileDescriptor, &cust, sizeof(struct Cust));
    if (R_bytes == -1)
    {
        printf("Error while reading cust record from the file!");
        return 0;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(custFileDescriptor, F_SETLK, &lock);

    close(custFileDescriptor);

    W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_MENU, strlen(ADMIN_MOD_CUSTOMER_MENU));
    if (W_bytes == -1)
    {
        printf("Error while writing ADMIN_MOD_CUSTOMER_MENU message to client!");
        return 0;
    }
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error while getting cust modification menu choice from client!");
        return 0;
    }

    int choice = atoi(R_buff);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, ERRON_INPUT_FOR_NUMBER);
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    switch (choice)
    {
    case 1:
        W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_NEW_NAME, strlen(ADMIN_MOD_CUSTOMER_NEW_NAME));
        if (W_bytes == -1)
        {
            printf("Error while writing ADMIN_MOD_CUSTOMER_NEW_NAME message to client!");
            return 0;
        }
        R_bytes = read(fd1, &R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            printf("Error while getting response for cust's new name from client!");
            return 0;
        }
        strcpy(cust.name, R_buff);
        break;
    case 2:
        W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
        if (W_bytes == -1)
        {
            printf("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
            return 0;
        }
        R_bytes = read(fd1, &R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            printf("Error while getting response for cust's new age from client!");
            return 0;
        }
        int updatedAge = atoi(R_buff);
        if (updatedAge == 0)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(W_buff, sizeof(W_buff));
            strcpy(W_buff, ERRON_INPUT_FOR_NUMBER);
            W_bytes = write(fd1, W_buff, strlen(W_buff));
            if (W_bytes == -1)
            {
                printf("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return 0;
            }
            R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
            return 0;
        }
        cust.age = updatedAge;
        break;
    case 3:
        W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
        if (W_bytes == -1)
        {
            printf("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
            return 0;
        }
        R_bytes = read(fd1, &R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            printf("Error while getting response for cust's new gender from client!");
            return 0;
        }
        cust.gender = R_buff[0];
        break;
    default:
        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, INVALID_MENU_CHOICE);
        W_bytes = write(fd1, W_buff, strlen(W_buff));
        if (W_bytes == -1)
        {
            printf("Error while writing INVALID_MENU_CHOICE message to client!");
            return 0;
        }
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        return 0;
    }

    custFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
    if (custFileDescriptor == -1)
    {
        printf("Error while opening cust file");
        return 0;
    }
    offset = lseek(custFileDescriptor, custID * sizeof(struct Cust), SEEK_SET);
    if (offset == -1)
    {
        printf("Error while seeking to required cust record!");
        return 0;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(custFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        printf("Error while obtaining write lock on cust record!");
        return 0;
    }

    W_bytes = write(custFileDescriptor, &cust, sizeof(struct Cust));
    if (W_bytes == -1)
    {
        printf("Error while writing update cust info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(custFileDescriptor, F_SETLKW, &lock);

    close(custFileDescriptor);

    W_bytes = write(fd1, ADMIN_MOD_CUSTOMER_SUCCESS, strlen(ADMIN_MOD_CUSTOMER_SUCCESS));
    if (W_bytes == -1)
    {
        printf("Error while writing ADMIN_MOD_CUSTOMER_SUCCESS message to client!");
        return 0;
    }
    R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read

    return 1;
}

#endif
