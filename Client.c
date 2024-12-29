#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

int sockFD;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void writeaInt(int num){
    int choice;
    
    int scan = scanf("%d",&choice);
    while(scan!=1)
    {   
        printf("\033[1;31m\nYou did Not enter a Integer. Try Again\033[1;35m\n\nEnter your number %d again :\033[0m",num);
        while (getchar() != '\n');
        scan = scanf("%d",&choice);
    }
    if(write(sockFD,&choice,sizeof(int))<0)
        printf("\033[1;31m\nError In Writing\033[0m\n");
}

void readstring(){
    char buffer[256];
    if(read(sockFD,buffer,sizeof(buffer))<=0)
        error("\033[1;31m\nError in Reading from the server\033[0m\n");
    printf("%s",buffer);
}


int main(int argc, char* argv[]){
    sockFD = socket(AF_INET, SOCK_STREAM,0);

    struct sockaddr_in server_addr;
    struct hostent *server;

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("\033[1;31m\nError: No such host\033[0m\n\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    bcopy((char *)server->h_addr_list[0], (char *)&server_addr.sin_addr.s_addr, server->h_length);

    if(connect(sockFD, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
        error("\033[1;31m\nError in Connection\033[0m\n");

    printf("\033[1;32m\n------------------------------------------------------------------------------\n                              Network Calculator                              \n------------------------------------------------------------------------------\033[0m\n");
    char buffer[255];
    while(1)
    {
        printf("\n");
        if(read(sockFD,buffer,sizeof(buffer))<=0)
            error("\033[1;31m\nError in Reading Menu from Server\033[0m\n");
        printf("%s",buffer);


        int choice;
        int scan = scanf("%d",&choice);
        while(scan!=1)
        {   
            printf("\033[1;31m\nYou did Not enter a Integer. Try Again\033[1;35m\n\nEnter your choice :\033[0m");
            while (getchar() != '\n');
            scan = scanf("%d",&choice);
        }
        
        if (write(sockFD, &choice, sizeof(int)) <= 0)
            error("\033[1;31m\nError in Sending Choice to Server\033[0m\n");


        if (choice >= 1 && choice <= 4) {
            readstring(); // Prompt for Number 1
            writeaInt(1);  // Send Number 1
            readstring(); // Prompt for Number 2
            writeaInt(2);  // Send Number 2
            readstring(); // Display Result
        }
        else if (choice == 5) {
            printf("\033[1;32mExiting...\033[0m\n");
            break;
        } 
        else 
            readstring();
    }
    close(sockFD);
    return 0;
}
