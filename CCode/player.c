#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include "TicTacToe.h"

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
    printf("Received: %s\n", buff);
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
    //to add next time:
    //  clean exit
    //  check for correct input
    //  check for win
    //  check for stalemate
    struct TicTacToe *game = malloc(sizeof(struct TicTacToe));
    resetGame(game);
    printf("%s", toString(game));

    char buff[BUFFSIZE];
    
    //move format:  Command [row col]
    //Commands:  e (exit), m (move), p (play again)
    // e.g., on the first move, suppose X wants to move to row 3, column 2
    // m32
    // program appends 1 to the end to get m321.  

    int turn=0;//use this variable to count turns to determine stalemate

    setPlayer(game, 1);
    if (isX){ //make first move
        turn++;
        bzero(buff, sizeof(buff));
        printf("Enter move:  ");
        int n = 0;
        while ((buff[n++] = getchar()) != '\n');
        if (buff[0] == 'e') return; //exit
        makeMove(game, buff[1]-'0', buff[2]-'0');
        printf("%s", toString(game));
        buff[3]='0'+turn;
        write(sockfd, buff, sizeof(buff));
    }
    for (;;) {
        
        turn++;
        setPlayer(game, !isX);
        take(buff, sockfd);
        if (buff[0] == 'e') break; //exit
        makeMove(game, buff[1]-'0', buff[2]-'0');
        printf("%d\n", turn);
        printf("%s", toString(game));


        turn++;
        setPlayer(game, isX);
        bzero(buff, sizeof(buff));
        printf("Enter move:  ");
        int n = 0;
        while ((buff[n++] = getchar()) != '\n');
        if (buff[0] == 'e') break; //exit
        makeMove(game, buff[1]-'0', buff[2]-'0');
        printf("%d\n", turn);
        printf("%s", toString(game));
        buff[3]='0'+turn;
        write(sockfd, buff, sizeof(buff));

        //if ((strncmp(buff, "exit", 4)) == 0) {
        //    printf("Exit...\n");
        //    break;
        //}
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
            printf("listening..\n");
        len = sizeof(cli);

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("Connected to player O.\n");

        // Function for chatting between client and server
        func(connfd);

   } else {
        char*address = malloc(sizeof(char)*16);
        printf("Address to connect:  ");
        fgets(address, sizeof(char)*16, stdin);

        servaddr.sin_addr.s_addr = inet_addr(address);
        // connect the client socket to server socket
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
            != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("Connected to player X\n");

        func(sockfd);
    }


        // close the socket
    close(sockfd);
}
