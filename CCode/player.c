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
#include "time.h"

#define PORT 9000
#define BUFFSIZE 1024
#define SA struct sockaddr

int isX; //indicate who I am
struct TicTacToe *game; //game structure
FILE *fp; //log file
char*peeraddress;
char*myaddress;


/**
 * take(buff, sockfd);
 * Tom Gehman
 *
 * read a line from the socket
 * we also write this to the log file
 */
void take(char*buff, int sockfd){
    bzero(buff, BUFFSIZE);
    read(sockfd, buff, BUFFSIZE);
    printf("Received: %s\n", buff);
    fprintf(fp,"Received from %s to %s: %s", peeraddress, myaddress, buff);
}

/**
 * playgame(sockfd)
 * Tom Gehman
 *
 * gameplay function.  Take turns making moves.
 * Take input from stdin and the socket, and update the board accordingly.  
 * When a win or draw is made, indicate such, and ask to play again.  
 * call this function recursively for every new game
 *
 * This function is used by both client (o) and server (x). 
 */
void playgame(int sockfd)
{
    if (!game) {game = malloc(sizeof(struct TicTacToe));}
    resetGame(game);
    printf("%s", toString(game));

    char buff[BUFFSIZE];
    
    //move format:  Command [row col]
    //Commands:  e (exit), m (move)
    // e.g., on the first move, suppose X wants to move to row 1, column 2
    // m12

    int turn=0;//use this variable to count turns to determine stalemate

    //X sends the first move.  
    setPlayer(game, 1);
    if (isX){ //make first move
        turn++;
        printf("\nTurn %d\n", turn);
        do { //take input and check it before proceeding
            bzero(buff, sizeof(buff));
            printf("Enter move:  ");
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'e') {
                buff[1]='\0';
                write(sockfd, buff, sizeof(buff));
                fclose(fp);
                close(sockfd);
                exit(0);
            }
        } while (!makeMove(game, buff[1]-'0', buff[2]-'0'));

        //update board, send move out
        printf("\n%s\n", toString(game));
        write(sockfd, buff, sizeof(buff));
        fprintf(fp,"Sending from %s to %s:  %sX moved to Row %c Col %c\n\n\n", 
                myaddress, peeraddress, buff, buff[1], buff[2]); //write log
    }

    // Main game loop : receive a message, then send a message
    for (;;) { //loop until the game is finished.  
        // First iteration:  O begins here by waiting.  X waits here for O's first move
        
        //Ready to receive a move
        turn++; //keep track of turn number to see when board fills.  
        setPlayer(game, !isX);
        printf("Turn %d\n", turn);

        //get input from socket
        take(buff, sockfd);
        if (buff[0] == 'e') {close(sockfd); fclose(fp);exit(0);} //exit
        fprintf(fp,"%c moved to Row %c Col %c\n\n\n", 
                isX?'O':'X', buff[1], buff[2]); //write log

        //make the move and check for win/draw
        makeMove(game, buff[1]-'0', buff[2]-'0');
        printf("%s\n", toString(game));
        if (checkForWin(game)){
            printf("You Lose\n");
            break;
        } else if (turn==9) { //Board is full, stalemate
            printf("Stalemate\n");
            break;
        }

        //make next move
        turn++;
        setPlayer(game, isX);
        printf("Turn %d\n", turn);
        do { //take input from STDIN and check
            bzero(buff, sizeof(buff));
            printf("Enter move:  ");
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            if (buff[0] == 'e') {
                buff[1]='\0';
                write(sockfd, buff, sizeof(buff));
                fprintf(fp,"Sending from %s to %s:  %s\n", myaddress, peeraddress, buff); //write log
                //fclose(fp);
                close(sockfd);
                exit(0); //exit
            }
        } while (!makeMove(game, buff[1]-'0', buff[2]-'0'));

        printf("\n%s\n", toString(game));

        //send it by the socket
        write(sockfd, buff, sizeof(buff));
        fprintf(fp,"Sending from %s to %s:  %s%c moved to Row %c Col %c\n\n\n", 
                myaddress, peeraddress, buff, 
                isX?'X':'O', buff[1], buff[2]); //write log
        if (checkForWin(game)){
            printf("You Win\n");
            break;
        } else if (turn==9) { //Board is full, stalemate.  
            printf("Stalemate\n");
            break;
        }
    }

    //game over; switch players and ask to play again.  
    isX=!isX;
    if (isX) {
        printf("Play again as X? [y/n]: ");
        //(new) X is asked first, sends message
        do {
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            buff[1]='\n';
            buff[2]='\0';
            if (buff[0] == 'n') { //no?  close down
                write(sockfd, buff, sizeof(buff));
                fprintf(fp,"Sending from %s to %s:  %s\n", myaddress, peeraddress, buff); //write log
                fclose(fp);
                close(sockfd);
                exit(0); //exit
            }
            if (buff[0] == 'y') { //yes?  proceed
                write(sockfd, buff, sizeof(buff));
                fprintf(fp,"Sending from %s to %s:  %s\n", myaddress, peeraddress, buff); //write log
                break;
            }
        } while (1);
        //check what (new) O sent:  if no, exit
        take(buff, sockfd);
        if (buff[0] == 'n') {
            close(sockfd); 
            fclose(fp);
            exit(0); 
        } 

    } else { //(new) player O
        //receive from X
        take(buff, sockfd);
        if (buff[0] == 'n') { //if no, exit
            close(sockfd); 
            fclose(fp);
            exit(0);
        } 
        printf("Play again as O? [y/n]: ");
        //(new) O is asked second, just like X.  
        do {
            int n = 0;
            while ((buff[n++] = getchar()) != '\n');
            buff[1]='\n';
            buff[2]='\0';
            if (buff[0] == 'n') {
                write(sockfd, buff, sizeof(buff));
                fprintf(fp,"Sending from %s to %s:  %s\n", myaddress, peeraddress, buff); //write log
                fclose(fp);
                close(sockfd);
                exit(0); //exit
            }
            if (buff[0] == 'y') {
                write(sockfd, buff, sizeof(buff));
                fprintf(fp,"Sending from %s to %s:  %s\n", myaddress, peeraddress, buff); //write log
                break;
            }
        } while (1);
    }

    //Assuming neither player has opted to quit, 
    //we recursively call the playgame function.  
    //The X-O roles are reversed.  
    playgame(sockfd);
}


/* main()
 * socket code adapted from 
 *      https://thecodeforge.io/c-cpp/c-networking-sockets/
 * modified by Tom Gehman
 * Set up a TCP connection and begin gameplay
 * This function works as both the server and client to 
 *    consolidate some code -- they split midway
 * Once the connection is established, playgame() is called
 */
int main(int argc, char**argv) {
    //check if the player is x (server) 
    //  or o (client)
    if (argv[1][0]=='o')
        isX=0;
    else
        isX=1;

    //socket creation same for both players
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed.\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");

    //enable socket reuse after termination (w/o delay)
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        printf("failed to set reuseaddr\n");
        exit(0);
    }

    // assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    //split client/server code here
    if (isX){ //player X goes first and acts as server
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
        //Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
            printf("socket bind failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully binded..\n");

        //now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("listening..\n");
        len = sizeof(cli);

        //Accept the data packet from client and verification
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("Connected to player O.\n");

        //Open a log file, use the time to make unique name
        char filename[1000];
        sprintf(filename, "xLog%ld", time(NULL));
        fp = fopen(filename, "w");
        
        //Grab peer's ip address as string
        struct sockaddr_in peeraddr;
		socklen_t peeraddrlen = sizeof(peeraddr);
		if (0<getpeername(connfd, (struct sockaddr *) &peeraddr, &peeraddrlen)){
            printf("peername error"); exit(0);
        }
		peeraddress = inet_ntoa(peeraddr.sin_addr);

        //Grab self's ip address
        struct sockaddr_in myaddr;
		socklen_t myaddrlen = sizeof(myaddr);
		if (0<getsockname(connfd, (struct sockaddr *) &myaddr, &myaddrlen)){
            printf("myname error"); exit(0);
        }
		myaddress = inet_ntoa(myaddr.sin_addr);

        //commence
        playgame(connfd);

   } else { //player O goes second and acts as the client
        //grab ip from stdin
        peeraddress = malloc(sizeof(char)*16);
        printf("Address to connect:  ");
        fgets(peeraddress, sizeof(char)*16, stdin);
        peeraddress[strlen(peeraddress)-1]='\0';//remove newline char
        servaddr.sin_addr.s_addr = inet_addr(peeraddress);

        //connect the client socket to server socket
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
            != 0) {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else
            printf("Connected to player X\n");

        //Open a log file, use the time to make unique name
        char filename[1000];
        sprintf(filename, "oLog%ld", time(NULL));
        fp = fopen(filename, "w");

        //get self's ip address as string
        struct sockaddr_in myaddr;
		socklen_t myaddrlen = sizeof(myaddr);
		if (0<getsockname(sockfd, (struct sockaddr *) &myaddr, &myaddrlen)){
            printf("myname error"); exit(0);
        }
		myaddress = inet_ntoa(myaddr.sin_addr);

        //begin
        playgame(sockfd);
    }

    // close the socket in case it was not closed already
    close(sockfd);
    fclose(fp);
}
