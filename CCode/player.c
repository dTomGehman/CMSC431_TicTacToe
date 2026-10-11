/**
 * player.c
 * Tom Gehman
 * 
 * This contains the main procedure and helpers for playing Tic Tac Toe.  
 * Run two instances to set up a TCP connection and play a game.  
 * run ./play x
 *      first and then ./play o
 *      on another terminal.   
 */

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
struct TicTacToe *game; 

/**
 * take(buff, sockfd);
 * Tom Gehman
 *
 * read a line from the socket
 */
void take(char*buff, int sockfd){
    bzero(buff, BUFFSIZE);
    read(sockfd, buff, BUFFSIZE);
    printf("Received: %s\n", buff);
}

/**
 * playgame(sockfd)
 * Tom Gehman
 *
 * gameplay function.  Take turns making moves.
 */
void playgame(int sockfd)
{
    if (!game) {game = malloc(sizeof(struct TicTacToe));}
    resetGame(game);
    printf("%s", toString(game));

    char buff[BUFFSIZE];
    
    //move format:  Command [row col]
    //Commands:  e (exit), m (move), p (play again)
    // e.g., on the first move, suppose X wants to move to row 3, column 2
    // m32

    int turn=0;//use this variable to count turns to determine stalemate

    setPlayer(game, 1);
    if (isX){ //make first move

        turn++;
        printf("\nTurn %d\n", turn);
        do {
            bzero(buff, sizeof(buff));
            printf("Enter move:  ");
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'e') {
                write(sockfd, buff, sizeof(buff));
                exit(0);
            }
        } while (!makeMove(game, buff[1]-'0', buff[2]-'0'));

        printf("\n%s\n", toString(game));
        //buff[3]='0'+turn;
        write(sockfd, buff, sizeof(buff));
    }

    for (;;) { //loop until the game is finished.  Player O receives X's message. 
        //receive a move
        turn++;
        setPlayer(game, !isX);
        printf("Turn %d\n", turn);
        take(buff, sockfd);
        if (buff[0] == 'e') {close(sockfd); exit(0);} //exit
        makeMove(game, buff[1]-'0', buff[2]-'0');
        printf("%s\n", toString(game));
        if (checkForWin(game)){
            printf("You Lose\n");
            break;
        } else if (turn==9) {
            printf("Stalemate\n");
            break;
        }

        //make next move
        turn++;
        setPlayer(game, isX);
        printf("Turn %d\n", turn);
        do {
            bzero(buff, sizeof(buff));
            printf("Enter move:  ");
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'e') {
                write(sockfd, buff, sizeof(buff));
                exit(0); //exit
            }
        } while (!makeMove(game, buff[1]-'0', buff[2]-'0'));

        printf("\n%s\n", toString(game));

        write(sockfd, buff, sizeof(buff));
        if (checkForWin(game)){
            printf("You Win\n");
            break;
        } else if (turn==9) {
            printf("Stalemate\n");
            break;
        }
    }

    //game over; switch players and ask to play again.  
    isX=!isX;
    if (isX) {
        printf("Play again as X? [y/n]: ");
        do {
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'n') {
                write(sockfd, buff, sizeof(buff));
                exit(0); //exit
            }
            if (buff[0] == 'y') {
                write(sockfd, buff, sizeof(buff));
                break;
            }
        } while (1);
        take(buff, sockfd);
        if (buff[0] == 'n') {close(sockfd); exit(0);} //exit
    } else {
        take(buff, sockfd);
        if (buff[0] == 'n') {close(sockfd); exit(0);} //exit
        printf("Play again as O? [y/n]: ");
        do {
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'n') {
                write(sockfd, buff, sizeof(buff));
                exit(0); //exit
            }
            if (buff[0] == 'y') {
                write(sockfd, buff, sizeof(buff));
                break;
            }
        } while (1);
    }
    playgame(sockfd);
}


/* main()
 * socket code adapted from 
 *      https://thecodeforge.io/c-cpp/c-networking-sockets/
 * Set up a TCP connection.  
 */
int main(int argc, char**argv) {
    if (argv[1][0]=='o')
        isX=0;
    else
        isX=1;
//same for both players
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
//split here
    if (isX){ //player X goes first and acts as server
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
        playgame(connfd);

   } else { //player O goes second and acts as the client
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

        playgame(sockfd);
    }

    // close the socket in case it was not closed already
    close(sockfd);
}
