import static org.junit.Assert.*;

import org.junit.Test;

public class TicTacToeTest
{
	String blank_board = "-------\n"+
                         "| | | |\n" +
                         "-------\n"+
                         "| | | |\n" +
                         "-------\n"+
                         "| | | |\n" +
                         "-------\n";
	@Test
	public void testConstructor()
	{
		TicTacToe game = new TicTacToe();
		assertEquals(blank_board,game.toString());
	}
	
	@Test
	public void testSetPlayer()
	{
		TicTacToe game = new TicTacToe();
		game.setPlayer(-1);
		assertEquals(-1,game.getPlayer());
		game.setPlayer(0);
		assertEquals(0,game.getPlayer());
		game.setPlayer(1);
		assertEquals(1,game.getPlayer());
		game.setPlayer(-2);
		assertEquals(-1,game.getPlayer());
		game.setPlayer(2);
		assertEquals(-1,game.getPlayer());
	}
	
	@Test
	public void testCheckForWin()
	{
		TicTacToe game = new TicTacToe();
		game.setPlayer(0);
		game.makeMove(0, 0);
		assertFalse(game.checkForWin());
		game.makeMove(0, 1);
		assertFalse(game.checkForWin());
		game.makeMove(0, 2);
		assertTrue(game.checkForWin());
		game.setPlayer(-1);
		assertFalse(game.checkForWin());
		game.setPlayer(1);
		assertFalse(game.checkForWin());
		
		game.resetGame();
		game.setPlayer(1);
		game.makeMove(0, 2);
		game.makeMove(1, 2);
		game.makeMove(2, 2);
		assertTrue(game.checkForWin());
		
		game.resetGame();
		game.setPlayer(0);
		game.makeMove(0, 0);
		game.makeMove(1, 1);
		game.makeMove(2, 2);
		assertTrue(game.checkForWin());
	}

}
