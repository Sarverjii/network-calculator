
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include<pthread.h>
#include <arpa/inet.h>

int sockFD;
int count = 0;

void error(const char *msg) {
    perror(msg);
    exit(1);
}


void createsocketandbind(int port_num){

    struct sockaddr_in server_addr;

     sockFD = socket(AF_INET,SOCK_STREAM,0);
     if(sockFD<0)
        error("\033[1;31mError In Making The Socket\033[0m");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockFD, (struct sockaddr *)&server_addr,sizeof(server_addr)))
        error("\033[1;31mError While Binding\033[0m");

}


int readfromClient(int newsockFD, int* n){
    if(read(newsockFD, n,sizeof(int))<=0){
        return 1;
    }
    return 0;
}

int writingtoClient(char buffer[], int newsockFD)
{
    if(write(newsockFD, buffer, 255)<=0){
        return 1;
    }
    return 0;
}




void *AcceptedClientProcessing(void *arg) {
    int newsockFD = *(int *)arg;
    char buffer[255];
    int num1, num2, choice;


    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);



      // Get the client's address
    if (getpeername(newsockFD, (struct sockaddr *)&client_addr, &client_len) == -1) {
        perror("\033[1;31mgetpeername failed\033[0m");
        return NULL;
    }



    while (1) {
        // Send the menu
        if(writingtoClient(" \033[1;37mNetwork Calculator\n1) Addition\n2) Subtraction\n3) Multiplication\n4) Division\n5) Exit\033[1;35m\nEnter your choice: \033[0m", newsockFD)==1)
            break;

        // Read the client's choice
        if(readfromClient(newsockFD,&choice)==1)
            break;
        if (choice >= 1 && choice <= 4) {
            if(writingtoClient("\033[1;35mEnter Number 1 : \033[0m", newsockFD)==1)
                break;
            if(readfromClient(newsockFD,&num1)==1)
                break;

            if(writingtoClient("\033[1;35mEnter Number 2 : \033[0m", newsockFD)==1)
                break;
            if(readfromClient(newsockFD,&num2)==1)
                break;

            char result[50];
            switch (choice) {
                case 1: snprintf(result, sizeof(result), "\033[1;33mThe Result is: %d\033[0m\n", num1 + num2); break;
                case 2: snprintf(result, sizeof(result), "\033[1;33mThe Result is: %d\033[0m\n", num1 - num2); break;
                case 3: snprintf(result, sizeof(result), "\033[1;33mThe Result is: %d\033[0m\n", num1 * num2); break;
                case 4: snprintf(result, sizeof(result), "\033[1;33mThe Result is: %.2f\033[0m\n", num2 ? (float)num1 / num2 : 0.0); break;
            }
            if(writingtoClient(result, newsockFD)==1)
                break;
        } else if (choice == 5) {
            printf("\033[1;31mClient requested to exit\033[0m\n");
            break;
        } else {
            if(writingtoClient("\033[1;31mInvalid choice. Try again.\033[0m\n", newsockFD)==1)
                break;
        }
    }
    exit:close(newsockFD);
    printf("\033[1;31mClient disconnected. IP: %s. [Connected Clients -> %d]\033[0m\n", inet_ntoa(client_addr.sin_addr), --count);
}


void AcceptingClient(){
    while(1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int newsockfd = accept(sockFD, (struct sockaddr *)&client_addr, &client_len);
        printf("\033[1;32mClient Connected with IP - %s. [Connected Clients -> %d]\033[0m\n",inet_ntoa(client_addr.sin_addr),++count);
        pthread_t id;
        pthread_create(&id,NULL,AcceptedClientProcessing,&newsockfd);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "\033[1;31mUsage: %s <Port>\033[0m\n", argv[0]);
        exit(1);
    }

    createsocketandbind(atoi(argv[1]));

    listen(sockFD,5);

    AcceptingClient();

    return 0;
}

