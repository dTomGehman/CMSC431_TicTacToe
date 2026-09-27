
/**
 * This class provides support for playing a Tic Tac Toe game,
 * but doesn't enforce which player goes when.  It does protect
 * the board so a player can't change another player's move or
 * mark the same place twice.
 * 
 * Each of these should be able to run as both server and client.
 * @author Dr. Girard
 *
 */
public class TicTacToe 
{
	/**
	 * ' ' - Blank place
	 * 'O' - 'O' Player controls that spot
	 * 'X' - 'X' Player controls that spot
	 */
	private char game_board[][] = new char[3][3];
	/**
	 * -1 = game not started
	 * 0 = 'O' Player
	 * 1 = 'X' Player
	 */
	private int player_move = -1;
	
	public TicTacToe()
	{
        initializeBoard();
	}
	
	/**
	 * Sets all the places on the board to blank.
	 */
	private void initializeBoard()
	{
		for (int r=0;r<game_board.length;r++)
			for (int c=0;c<game_board[r].length;c++)
				game_board[r][c] = ' ';
	}
	
	public void resetGame()
	{
		initializeBoard();
		player_move = -1;
	}
	
	public void setPlayer(int player)
	{
		switch (player)
		{
			case -1:
				player_move = -1;
				break;
			case 0:
				player_move = 0;
				break;
			case 1:
				player_move = 1;
				break;
			default: // Likely should throw an exception here.
				player_move = -1;
		}
	}
	
	public int getPlayer()
	{
		return player_move;
	}
	
	/**
	 * Will update the game_board based on location and
	 * which player is actively moving.  Will not update a location
	 * if a player has already played there or if player_move is
	 * set to -1.
	 * @param r Row player is making a move.
	 * @param c Column player is making a move.
	 * @return True if the game_board was updated, false otherwise.
	 */
	public boolean makeMove(int r, int c)
	{
		boolean success = false;
		if (game_board[r][c] == ' ')
		{
			if (player_move == 0)
			{
				game_board[r][c] = 'O';
				success = true;
			}
			if (player_move == 1)
			{
				game_board[r][c] = 'X';
				success = true;
			}
		}
		return success;
	}
	
	/**
	 * Checks to see if the player_move player has won.
	 * @return true if the player has won, false otherwise.
	 */
	public boolean checkForWin()
	{
		boolean win = false;
		char win_token = '-';
		if (player_move == 0)
			win_token = 'O';
		if (player_move == 1)
			win_token = 'X';
		
		for (int r=0;r<game_board.length;r++)
		{
			if ((game_board[r][0] == win_token) &&
			(game_board[r][1] == win_token) &&
			(game_board[r][2] == win_token))
				win = true;
		}
		
		for (int c=0;c<game_board.length;c++)
		{
			if ((game_board[0][c] == win_token) &&
			(game_board[1][c] == win_token) &&
			(game_board[2][c] == win_token))
				win = true;
		}
		
		if ((game_board[0][0] == win_token) &&
				(game_board[1][1] == win_token) &&
				(game_board[2][2] == win_token))
					win = true;
		
		if ((game_board[0][2] == win_token) &&
				(game_board[1][1] == win_token) &&
				(game_board[2][0] == win_token))
					win = true;
		return win;
	}
	
	/**
	 * @return The state of the board.
	 */
	@Override
	public String toString()
	{
		String board = "-------\n";
		for (int r=0;r<game_board.length;r++)
		{
		    for (int c=0;c<game_board[r].length;c++)
		    {
		        board += "|" + game_board[r][c];
		    }
		    board += "|\n";
		    board += "-------\n";
		}
		return board;
	}

}
