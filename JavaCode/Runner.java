
public class Runner
{
	public static void main(String[] args)
	{
		TicTacToe game = new TicTacToe();
		int player = 0;
		game.resetGame();
		game.setPlayer(player);
		game.makeMove(1, 1);
		player = 1;
		game.setPlayer(player);
		game.makeMove(0, 0);
		System.out.print(game);
		
	}

}
