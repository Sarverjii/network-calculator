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

void writeaInt(){
    int choice;
    
    int scan = scanf("%d",&choice);
    while(scan!=1)
    {   
        printf("You did Not enter a Integer. Try Again\nEnter your choice :");
        while (getchar() != '\n');
        scan = scanf("%d",&choice);
    }
    if(write(sockFD,&choice,sizeof(int))<0)
        printf("Error In Writing");
}

void readstring(){
    char buffer[256];
    if(read(sockFD,buffer,sizeof(buffer))<=0)
        error("Error in Reading from the server");
    printf("%s",buffer);
}


int main(int argc, char* argv[]){
    sockFD = socket(AF_INET, SOCK_STREAM,0);

    struct sockaddr_in server_addr;
    struct hostent *server;

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    bcopy((char *)server->h_addr_list[0], (char *)&server_addr.sin_addr.s_addr, server->h_length);

    if(connect(sockFD, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
        error("Error in Connection");

    printf("Connected to the Network Calculator\n");
    char buffer[255];
    while(1)
    {
        printf("\n");
        if(read(sockFD,buffer,sizeof(buffer))<=0)
            error("Error in Reading Menu from Server");
        printf("%s",buffer);


        int choice;
        int scan = scanf("%d",&choice);
        while(scan!=1)
        {   
            printf("You did Not enter a Integer. Try Again\nEnter your choice :");
            while (getchar() != '\n');
            scan = scanf("%d",&choice);
        }
        
        if (write(sockFD, &choice, sizeof(int)) <= 0)
            error("Error in Sending Choice to Server");


        if (choice >= 1 && choice <= 4) {
            readstring(); // Prompt for Number 1
            writeaInt();  // Send Number 1
            readstring(); // Prompt for Number 2
            writeaInt();  // Send Number 2
            readstring(); // Display Result
        }
        else if (choice == 5) {
            printf("Exiting...\n");
            break;
        } 
        else 
            readstring();
    }
    close(sockFD);
    return 0;
}
