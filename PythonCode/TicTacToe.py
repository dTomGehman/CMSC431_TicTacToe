
#  This class provides support for playing a Tic Tac Toe game,
#  but doesn't enforce which player goes when.  It does protect
#  the board so a player can't change another player's move or
#  mark the same place twice.
class TicTacToe:
    __max = 3
##	  ' ' - Blank place
##	  'O' - 'O' Player controls that spot
##	  'X' - 'X' Player controls that spot
    __game_board = []
    
##	  -1 = game not started
##	  0 = 'O' Player
##	  1 = 'X' Player
    __player_move = -1
	
    def __init__(self) :
        self.initializeBoard();
	
    # Sets all the places on the board to blank.
    def initializeBoard(self):
        for r in range(self.__max):
            new_row = []
            for c in range(self.__max):
                new_row.append(' ')
            self.__game_board.append(new_row)
	
    def resetGame(self):
        self.__game_board.clear()
        initializeBoard(self)
        self.__player_move = -1
	
    def setPlayer(self,player):
        if player == -1:
            player_move = -1
        elif player == 0:
            player_move = 0
        elif player == 1:
            player_move = 1
        else:
            player_move = -1

    def getPlayer(self):
        return self.__player_move;
	

##	  Will update the game_board based on location and
##	  which player is actively moving.  Will not update a location
##	  if a player has already played there or if player_move is
##	  set to -1.
##	  @param r Row player is making a move.
##	  @param c Column player is making a move.
##	  @return True if the game_board was updated, false otherwise.
    def makeMove(self, r, c):
        success = false;
        if game_board[r][c] == ' ':
            if player_move == 0:
                self.__game_board[r][c] = 'O'
                success = true;
            if player_move == 1:
                self.__game_board[r][c] = 'X'
                success = true;
        return success;
	
	
##	 Checks to see if the player_move player has won.
##	 @return true if the player has won, false otherwise.
    def checkForWin(self):
        win = false;
        win_token = '-'
        if self.__player_move == 0:
            win_token = 'O'
        if self.__player_move == 1:
            win_token = 'X'

        board = self.__game_board
        for r in range(self.__max):
            if (board[r][0] == win_token) and (board[r][1] == win_token) and (board[r][2] == win_token):
                win = true
    
        for c in range(self.__max):
            if (board[0][c] == win_token) and (board[1][c] == win_token) and (board[2][c] == win_token):
                win = true;
		
        if (board[0][0] == win_token) and (board[1][1] == win_token) and (board[2][2] == win_token):
            win = true
		
        if (game_board[0][2] == win_token) and (board[1][1] == win_token) and (board[2][0] == win_token):
            win = true
        return win

	
	
##	  @return The state of the board.
    def toString(self):
        board = "-------\n";
        for r in range(self.__max):
            for c in range(self.__max):
                board += "|" + self.__game_board[r][c];
            board += "|\n";
            board += "-------\n";
        return board;

