play: player.o TicTacToe.o 
	gcc -o play player.o TicTacToe.o 

player.o: player.c
	gcc -c player.c 

TicTacToe.o: TicTacToe.c
	gcc -c TicTacToe.c 

clean:
	rm *.o play
