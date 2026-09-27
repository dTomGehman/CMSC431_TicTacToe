#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/**
 * This file provides support for playing a Tic Tac Toe game,
 * but doesn't enforce which player goes when.  
 * 
 * Should be able to run as both server and client.
 * author Dr. Girard
 *
 */
struct TicTacToe 
{
	/**
	 * ' ' - Blank place
	 * 'O' - 'O' Player controls that spot
	 * 'X' - 'X' Player controls that spot
	 */
	char values[3][3];
	/**
	 * -1 = game not started
	 * 0 = 'O' Player
	 * 1 = 'X' Player
	 */
	int player_move;
	int length;
};
	
	/**
	 * Sets all the places on the board to blank.
	 */
	void initializeBoard(struct TicTacToe *game_board)
	{
		game_board->length = 3;
		for (int r=0;r<game_board->length;r++)
			for (int c=0;c<game_board->length;c++)
				game_board->values[r][c] = ' ';
	}
	
	void resetGame(struct TicTacToe *game_board)
	{
		initializeBoard(game_board);
		game_board->player_move = -1;
	}
	
	void setPlayer(struct TicTacToe *game_board, int player)
	{
		switch (player)
		{
			case -1:
				game_board->player_move = -1;
				break;
			case 0:
				game_board->player_move = 0;
				break;
			case 1:
				game_board->player_move = 1;
				break;
			default: // Likely should throw an exception here.
				game_board->player_move = -1;
		}
	}
	
	/**
	 * Will update the game_board based on location and
	 * which player is actively moving.  Will not update a location
	 * if a player has already played there or if player_move is
	 * set to -1.
	 * param r Row player is making a move.
	 * param c Column player is making a move.
	 * return 1 if the game_board was updated, 0 otherwise.
	 */
	int makeMove(struct TicTacToe *game_board, int r, int c)
	{
		int success = 0;
		if (game_board->values[r][c] == ' ')
		{
			if (game_board->player_move == 0)
			{
				game_board->values[r][c] = 'O';
				success = 1;
			}
			if (game_board->player_move == 1)
			{
				game_board->values[r][c] = 'X';
				success = 1;
			}
		}
		return success;
	}
	
	/**
	 * Checks to see if the player_move player has won.
	 * return 1 if the player has won, 0 otherwise.
	 */
	int checkForWin(struct TicTacToe *game_board)
	{
		int win = 0;
		char win_token = '-';
		if (game_board->player_move == 0)
			win_token = 'O';
		if (game_board->player_move == 1)
			win_token = 'X';
		
		for (int r=0;r<game_board->length;r++)
		{
			if ((game_board->values[r][0] == win_token) &&
			(game_board->values[r][1] == win_token) &&
			(game_board->values[r][2] == win_token))
				win = 1;
		}
		
		for (int c=0;c<game_board->length;c++)
		{
			if ((game_board->values[0][c] == win_token) &&
			(game_board->values[1][c] == win_token) &&
			(game_board->values[2][c] == win_token))
				win = 1;
		}
		
		if ((game_board->values[0][0] == win_token) &&
				(game_board->values[1][1] == win_token) &&
				(game_board->values[2][2] == win_token))
					win = 1;
		
		if ((game_board->values[0][2] == win_token) &&
				(game_board->values[1][1] == win_token) &&
				(game_board->values[2][0] == win_token))
					win = 1;
		return win;
	}
	
	/**
	 * return The state of the board.
	 */
	char* toString(struct TicTacToe *game_board)
	{

		char *board = malloc(49*sizeof(char));
		board[0] = '\0';
		strcat(board,"-------\n");
		for (int r=0;r<game_board->length;r++)
		{
		    for (int c=0;c<game_board->length;c++)
		    {
		        strcat(board,"|");
				strncat(board,&game_board->values[r][c],1);
		    }
		    strcat(board,"|\n");
		    strcat(board,"-------\n");
		}
		return board;
	}

    int main(char **argsc,int argsv)
	{
		struct TicTacToe *game = malloc(sizeof(struct TicTacToe));
		int player = 0;
		resetGame(game);
		setPlayer(game,player);
		makeMove(game,1, 1);
		player = 1;
		setPlayer(game,player);
		makeMove(game,0, 0);
	    printf("%s",toString(game));
		
	}
