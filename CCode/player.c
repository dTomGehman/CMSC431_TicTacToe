#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#define PORT 9000
#define ADDRESS "127.0.0.1"
#define BUFFSIZE 1024
#define SA struct sockaddr

//run this code with either 'x' or 'o' as the first argument
//'x' becomes the 'server' and listens first.  
//'o' must be started second and sends a message first.  
int isX;

void take(char*buff, int sockfd){
    bzero(buff, BUFFSIZE);
    read(sockfd, buff, BUFFSIZE);
    printf("Received: %s", buff);
}

void give(char*buff, int sockfd){
    bzero(buff, sizeof(buff));
    printf("Enter the string : ");
    int n = 0;
    while ((buff[n++] = getchar()) != '\n');
    write(sockfd, buff, sizeof(buff));
}

void func(int sockfd)
{
    char buff[BUFFSIZE];
    
    if (isX){
        take(buff, sockfd);
    }
    for (;;) {
        give(buff, sockfd);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Exit...\n");
            break;
        }

        take(buff, sockfd);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Exit...\n");
            break;
        }
    }
}


int main(int argc, char**argv) {
    if (argv[1][0]=='o')
        isX=0;
    else
        isX=1;
//same---------------------
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
//-----------------------------
    if (isX){
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            printf("socket bind failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully binded..\n");

        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("Server listening..\n");
        len = sizeof(cli);

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");

        // Function for chatting between client and server
        func(connfd);

   } else {
        servaddr.sin_addr.s_addr = inet_addr(ADDRESS);
        // connect the client socket to server socket
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
            != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("connected to the server..\n");

        // function for chat
        func(sockfd);
    }


        // close the socket
    close(sockfd);
}
