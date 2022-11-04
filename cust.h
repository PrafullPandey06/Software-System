#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUNCTIONS

// Semaphores are necessary joint acc due the design choice I've made
#include <sys/ipc.h>
#include <sys/sem.h>

struct Cust loggedInCust;
int semIdentifier;

// Function Prototypes =================================

bool cust_operation_handler(int fd1);
bool deposit(int fd1);
bool withdraw(int fd1);
bool get_acc_bal(int fd1);
bool change_password(int fd1);
bool lock_critical_section(struct sembuf *semOp);
bool unlock_critical_section(struct sembuf *sem_op);
void write_transaction_to_array(int *transactionArray, int ID);
int write_transaction_to_file(int accNumber, long int oldacc_bal, long int newacc_bal, bool operation);

// =====================================================

// Function Definition =================================

bool cust_operation_handler(int fd1)
{
    if (login_administrator(0, fd1, &loggedInCust))
    {
        ssize_t W_bytes, R_bytes;            // Number of bytes read from / written to the client
        char R_buff[500];
        char W_buff[500]; // A buffer used for reading & writing to the client

        // Get a semaphore for the user
        key_t semKey = ftok(CUSTOMER_FILE, loggedInCust.acc); // Generate a key based on the acc number hence, different custs will have different semaphores

        union semun
        {
            int val; // Value of the semaphore
        } semSet;

        int semctlStatus;
        semIdentifier = semget(semKey, 1, 0); // Get the semaphore if it exists
        if (semIdentifier == -1)
        {
            semIdentifier = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
            if (semIdentifier == -1)
            {
                printf("Error while creating semaphore!");
                _exit(1);
            }

            semSet.val = 1; // Set a binary semaphore
            semctlStatus = semctl(semIdentifier, 0, SETVAL, semSet);
            if (semctlStatus == -1)
            {
                printf("Error while initializing a binary sempahore!");
                _exit(1);
            }
        }

        bzero(W_buff, sizeof(W_buff));
        strcpy(W_buff, CUSTOMER_LOGIN_SUCCESS);
        while (1)
        {
            strcat(W_buff, "\n");
            strcat(W_buff, CUSTOMER_MENU);
            W_bytes = write(fd1, W_buff, strlen(W_buff));
            if (W_bytes == -1)
            {
                printf("Error while writing CUSTOMER_MENU to client!");
                return 0;
            }
            bzero(W_buff, sizeof(W_buff));

            bzero(R_buff, sizeof(R_buff));
            R_bytes = read(fd1, R_buff, sizeof(R_buff));
            if (R_bytes == -1)
            {
                printf("Error while reading client's choice for CUSTOMER_MENU");
                return 0;
            }
            
            // printf("READ BUFFER : %s\n", R_buff);
            int choice = atoi(R_buff);
            // printf("CHOICE : %d\n", choice);
            switch (choice)
            {
            case 1:
                get_cust_details(fd1, loggedInCust.id);
                break;
            case 2:
                deposit(fd1);
                break;
            case 3:
                withdraw(fd1);
                break;
            case 4:
                get_acc_bal(fd1);
                break;
            case 5:
                get_transaction_details(fd1, loggedInCust.acc);
                break;
            case 6:
                change_password(fd1);
                break;
            default:
                W_bytes = write(fd1, CUSTOMER_LOGOUT, strlen(CUSTOMER_LOGOUT));
                return 0;
            }
        }
    }
    else
    {
        // CUSTOMER LOGIN FAILED
        return 0;
    }
    return 1;
}

bool deposit(int fd1)
{
    char R_buff[500], W_buff[500];
    ssize_t R_bytes, W_bytes;

    struct Acc acc;
    acc.accNumber = loggedInCust.acc;

    long int depositAmount = 0;

    // Lock the critical section
    struct sembuf semOp;
    lock_critical_section(&semOp);

    if (get_acc_details(fd1, &acc))
    {
        
        if (acc.active)
        {

            W_bytes = write(fd1, DEPOSIT_AMOUNT, strlen(DEPOSIT_AMOUNT));
            if (W_bytes == -1)
            {
                printf("Error writing DEPOSIT_AMOUNT to client!");
                unlock_critical_section(&semOp);
                return 0;
            }

            bzero(R_buff, sizeof(R_buff));
            R_bytes = read(fd1, R_buff, sizeof(R_buff));
            if (R_bytes == -1)
            {
                printf("Error reading deposit money from client!");
                unlock_critical_section(&semOp);
                return 0;
            }

            depositAmount = atol(R_buff);
            
            /* #####################################################################################  */
            if (depositAmount < 0) {
               // perror("Enter money greater than zero!");
                 W_bytes = write(fd1, DEPOSIT_AMOUNT_INVALID, strlen(DEPOSIT_AMOUNT_INVALID));
                 unlock_critical_section(&semOp);
                 return false;
            }
            /* ######################################################################################### */
            
            if (depositAmount != 0 && depositAmount > 0)
            {

                int newTransactionID = write_transaction_to_file(acc.accNumber, acc.acc_bal, acc.acc_bal + depositAmount, 1);
                write_transaction_to_array(acc.transactions, newTransactionID);

                acc.acc_bal += depositAmount;

                int accFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
                off_t offset = lseek(accFileDescriptor, acc.accNumber * sizeof(struct Acc), SEEK_SET);

                struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Acc), getpid()};
                int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
                if (lockingStatus == -1)
                {
                    printf("Error obtaining write lock on acc file!");
                    unlock_critical_section(&semOp);
                    return 0;
                }

                W_bytes = write(accFileDescriptor, &acc, sizeof(struct Acc));
                if (W_bytes == -1)
                {
                    printf("Error storing updated deposit money in acc record!");
                    unlock_critical_section(&semOp);
                    return 0;
                }

                lock.l_type = F_UNLCK;
                fcntl(accFileDescriptor, F_SETLK, &lock);

                write(fd1, DEPOSIT_AMOUNT_SUCCESS, strlen(DEPOSIT_AMOUNT_SUCCESS));
                read(fd1, R_buff, sizeof(R_buff)); // Dummy read

                get_acc_bal(fd1);

                unlock_critical_section(&semOp);

                return 1;
            }
            else
                W_bytes = write(fd1, DEPOSIT_AMOUNT_INVALID, strlen(DEPOSIT_AMOUNT_INVALID));
        }
        else
            write(fd1, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(fd1, R_buff, sizeof(R_buff)); // Dummy read

        unlock_critical_section(&semOp);
    }
    else
    {
        // FAIL
        unlock_critical_section(&semOp);
        return 0;
    }
}

bool withdraw(int fd1)
{
    char R_buff[500], W_buff[500];
    ssize_t R_bytes, W_bytes;

    struct Acc acc;
    acc.accNumber = loggedInCust.acc;

    long int withdrawAmount = 0;

    // Lock the critical section
    struct sembuf semOp;
    lock_critical_section(&semOp);

    if (get_acc_details(fd1, &acc))
    {
        if (acc.active)
        {

            W_bytes = write(fd1, WITHDRAW_AMOUNT, strlen(WITHDRAW_AMOUNT));
            if (W_bytes == -1)
            {
                printf("Error writing WITHDRAW_AMOUNT message to client!");
                unlock_critical_section(&semOp);
                return 0;
            }

            bzero(R_buff, sizeof(R_buff));
            R_bytes = read(fd1, R_buff, sizeof(R_buff));
            if (R_bytes == -1)
            {
                printf("Error reading withdraw amount from client!");
                unlock_critical_section(&semOp);
                return 0;
            }

            withdrawAmount = atol(R_buff);

            if (withdrawAmount != 0 && acc.acc_bal - withdrawAmount >= 0)
            {

                int newTransactionID = write_transaction_to_file(acc.accNumber, acc.acc_bal, acc.acc_bal - withdrawAmount, 0);
                write_transaction_to_array(acc.transactions, newTransactionID);

                acc.acc_bal -= withdrawAmount;

                int accFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
                off_t offset = lseek(accFileDescriptor, acc.accNumber * sizeof(struct Acc), SEEK_SET);

                struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Acc), getpid()};
                int lockingStatus = fcntl(accFileDescriptor, F_SETLKW, &lock);
                if (lockingStatus == -1)
                {
                    printf("Error obtaining write lock on acc record!");
                    unlock_critical_section(&semOp);
                    return 0;
                }

                W_bytes = write(accFileDescriptor, &acc, sizeof(struct Acc));
                if (W_bytes == -1)
                {
                    printf("Error writing updated acc_bal into acc file!");
                    unlock_critical_section(&semOp);
                    return 0;
                }

                lock.l_type = F_UNLCK;
                fcntl(accFileDescriptor, F_SETLK, &lock);

                write(fd1, WITHDRAW_AMOUNT_SUCCESS, strlen(WITHDRAW_AMOUNT_SUCCESS));
                read(fd1, R_buff, sizeof(R_buff)); // Dummy read

                get_acc_bal(fd1);

                unlock_critical_section(&semOp);

                return 1;
            }
            else
                W_bytes = write(fd1, WITHDRAW_AMOUNT_INVALID, strlen(WITHDRAW_AMOUNT_INVALID));
        }
        else
            write(fd1, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(fd1, R_buff, sizeof(R_buff)); // Dummy read

        unlock_critical_section(&semOp);
    }
    else
    {
        // FAILURE while getting acc information
        unlock_critical_section(&semOp);
        return 0;
    }
}

bool get_acc_bal(int fd1)
{
    char buffer[500];
    struct Acc acc;
    acc.accNumber = loggedInCust.acc;
    if (get_acc_details(fd1, &acc))
    {
        bzero(buffer, sizeof(buffer));
        if (acc.active)
        {
            sprintf(buffer, "You have ₹ %ld imaginary money in our bank!^", acc.acc_bal);
            write(fd1, buffer, strlen(buffer));
        }
        else
            write(fd1, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(fd1, buffer, sizeof(buffer)); // Dummy read
    }
    else
    {
        // ERROR while getting acc_bal
        return 0;
    }
}

bool change_password(int fd1)
{
    ssize_t R_bytes, W_bytes;
    char R_buff[500], W_buff[500], hashedPassword[500];

    char newPassword[500];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    int semopStatus = semop(semIdentifier, &semOp, 1);
    if (semopStatus == -1)
    {
        printf("Error while locking critical section");
        return 0;
    }

    W_bytes = write(fd1, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (W_bytes == -1)
    {
        printf("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        unlock_critical_section(&semOp);
        return 0;
    }

    bzero(R_buff, sizeof(R_buff));
    R_bytes = read(fd1, R_buff, sizeof(R_buff));
    if (R_bytes == -1)
    {
        printf("Error reading old password response from client");
        unlock_critical_section(&semOp);
        return 0;
    }

    if (strcmp(crypt(R_buff, SALT_BAE), loggedInCust.password) == 0)
    {
        // Password matches with old password
        W_bytes = write(fd1, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (W_bytes == -1)
        {
            printf("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            unlock_critical_section(&semOp);
            return 0;
        }
        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            printf("Error reading new password response from client");
            unlock_critical_section(&semOp);
            return 0;
        }

        strcpy(newPassword, crypt(R_buff, SALT_BAE));

        W_bytes = write(fd1, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (W_bytes == -1)
        {
            printf("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            unlock_critical_section(&semOp);
            return 0;
        }
        bzero(R_buff, sizeof(R_buff));
        R_bytes = read(fd1, R_buff, sizeof(R_buff));
        if (R_bytes == -1)
        {
            printf("Error reading new password reenter response from client");
            unlock_critical_section(&semOp);
            return 0;
        }

        if (strcmp(crypt(R_buff, SALT_BAE), newPassword) == 0)
        {
            // New & reentered passwords match

            strcpy(loggedInCust.password, newPassword);

            int custFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
            if (custFileDescriptor == -1)
            {
                printf("Error opening cust file!");
                unlock_critical_section(&semOp);
                return 0;
            }

            off_t offset = lseek(custFileDescriptor, loggedInCust.id * sizeof(struct Cust), SEEK_SET);
            if (offset == -1)
            {
                printf("Error seeking to the cust record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Cust), getpid()};
            int lockingStatus = fcntl(custFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                printf("Error obtaining write lock on cust record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            W_bytes = write(custFileDescriptor, &loggedInCust, sizeof(struct Cust));
            if (W_bytes == -1)
            {
                printf("Error storing updated cust password into cust record!");
                unlock_critical_section(&semOp);
                return 0;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(custFileDescriptor, F_SETLK, &lock);

            close(custFileDescriptor);

            W_bytes = write(fd1, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read

            unlock_critical_section(&semOp);

            return 1;
        }
        else
        {
            // New & reentered passwords don't match
            W_bytes = write(fd1, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
            R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
        }
    }
    else
    {
        // Password doesn't match with old password
        W_bytes = write(fd1, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        R_bytes = read(fd1, R_buff, sizeof(R_buff)); // Dummy read
    }

    unlock_critical_section(&semOp);

    return 0;
}

bool lock_critical_section(struct sembuf *semOp)
{
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        printf("Error while locking critical section");
        return 0;
    }
    return 1;
}

bool unlock_critical_section(struct sembuf *semOp)
{
    semOp->sem_op = 1;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        printf("Error while operating on semaphore!");
        _exit(1);
    }
    return 1;
}

void write_transaction_to_array(int *transactionArray, int ID)
{
    // Check if there's any free space in the array to write the new transaction ID
    int iter = 0;
    while (transactionArray[iter] != -1)
        iter++;

    if (iter >= M_TXN)
    {
        // No space
        for (iter = 1; iter < M_TXN; iter++)
            // Shift elements one step back discarding the oldest transaction
            transactionArray[iter - 1] = transactionArray[iter];
        transactionArray[iter - 1] = ID;
    }
    else
    {
        // Space available
        transactionArray[iter] = ID;
    }
}

int write_transaction_to_file(int accNumber, long int oldacc_bal, long int newacc_bal, bool operation)
{
    struct Transaction newTransaction;
    newTransaction.accNumber = accNumber;
    newTransaction.oldacc_bal = oldacc_bal;
    newTransaction.newacc_bal = newacc_bal;
    newTransaction.operation = operation;
    newTransaction.transactionTime = time(NULL);

    ssize_t R_bytes, W_bytes;

    int transactionFileDescriptor = open(TRANSACTION_FILE, O_CREAT | O_APPEND | O_RDWR, S_IRWXU);

    // Get most recent transaction number
    off_t offset = lseek(transactionFileDescriptor, -sizeof(struct Transaction), SEEK_END);
    if (offset >= 0)
    {
        // There exists at least one transaction record
        struct Transaction prevTransaction;
        R_bytes = read(transactionFileDescriptor, &prevTransaction, sizeof(struct Transaction));

        newTransaction.transactionID = prevTransaction.transactionID + 1;
    }
    else
        // No transaction records exist
        newTransaction.transactionID = 0;

    W_bytes = write(transactionFileDescriptor, &newTransaction, sizeof(struct Transaction));

    return newTransaction.transactionID;
}



#endif
