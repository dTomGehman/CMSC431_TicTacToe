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

void initializeBoard(struct TicTacToe *game_board);

void resetGame(struct TicTacToe *game_board);

void setPlayer(struct TicTacToe *game_board, int player);

int makeMove(struct TicTacToe *game_board, int r, int c);

int checkForWin(struct TicTacToe *game_board);

char* toString(struct TicTacToe *game_board);

