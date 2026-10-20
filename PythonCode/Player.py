#!/usr/bin/env python
import TicTacUtils as ttu
import sys
import socket
import ipaddress
# Note: socket module is "transliteration of the Unix system call and library interface"
# In other words, most docs for socket are in man pages, man 7 socket, man 7 ip, and man 2 socket for example.


def client_sock(port):
    while True:
        try:
            addr = input("Please enter an IP address to connect to on port 9000\n")
            if addr == 'exit':
                exit("Exiting...")
            ipaddress.ip_address(addr)
            break
        except:
            print("IP address was invalid, try again")
    # create_connection w/o sources relies on OS default behavior
    try:
        return socket.create_connection(address=(addr,port))
    except:
        exit(f"Critical error, can't connect to {addr}:{port}")

def server_sock(addr, port):
    initsock = socket.create_server(address=(addr,port),family=socket.AF_INET)
    initsock.listen()
    connsock = initsock.accept()[0]
    return connsock

def opponent_move(board,sock):
    move = str(sock.recv(2048), 'utf8')
    if len(move) == 0:
        exit("Critical error, client has disconnected.")
    first_char = move[0]
    if first_char == 'e':
        print("Opponent triggered exit, closing socket...")
        sock.close()
        exit("Exiting game...")
    #if first_char == 'p':
    #    print("play again")
    #    sock.close()
    #    exit("played")
    if len(move) <= 2:
        exit("Invalid move, not exit, no r/c spec")
    try:
        row = int(move[1])
        col = int(move[2])
    except:
        row = repr(move[1])
        col = repr(move[2])
        print(f"String len: {len(move)}")
        print("String contents: " + move)
        exit(f"Failed to cast integers, move[1] and move[2]")
    if not board.makeMove(row,col):
        print("Invalid move received from server, board desynced?")
        return False
    else:
        return (row,col)
def my_move(board,sock):
    move = input("Please enter a move (e.g. m11 for middle box):\n")
    if len(move) < 1:
        return False
    first_char = move[0]
    if first_char == 'e':
        print("Opponent triggered exit, closing socket...")
        sock.close()
        exit("Exiting game...")
    #if first_char == 'p':
    #    print("play again")
    #    sock.close()
    #    exit("played")
    if len(move) <= 2:
        print("Invalid move, non-exit, no r/c spec")
        return False
    # Ensure we have two integers.
    row = move[1]
    col = move[2]
    if not row.isdecimal() or not col.isdecimal():
        print("Non digit move.")
        return False
    row = int(row)
    col = int(col)
    if not board.makeMove(row,col):
        print("Board spot in use, or out of bounds")
        return False
    else:
        return (row,col)

def win_check(sock, board, msg):
    if board.checkForWin():
        board.resetTurnCount()
        print(msg)
        val = input("Play again? [Y/N]\n").casefold()[0]
        if val == 'y':
            return True
        else:
            return False
    elif board.getTurnCount() == 9:
        board.resetTurnCount()
        print("It's a draw.")
        val = input("Play again? [Y/N]\n").casefold()[0]
        if val == 'y':
            return True
        else:
            return False
    return "none"

def begin_game(board,sock,player):
    board.resetGame()
    print("Starting game and printing board...")
    print(board.toString())
    # Set opponent to opposite of player
    opponent = 'x'
    if ord(player) > ord('o'):
        opponent = 'o'
    # This player var basically marks "am I the server" i.e. initial socket hoster.
    if player == 'x':
        initial_move = False
        board.setPlayer(player)
        initial_move = my_move(board,sock)
        while not initial_move:
            initial_move = my_move(board,sock)
        row = str(initial_move[0])
        col = str(initial_move[1])
        print(f"Player {player} marked {row},{col}")
        print(board.toString())
        sock.send(bytes("m"+row+col,"utf8"))
    while True:
        # Opponents move
        board.setPlayer(opponent)
        move_success = opponent_move(board,sock)
        while not move_success:
            opponent_move(board,sock)
        row = move_success[0]
        col = move_success[1]
        print(f"Player {opponent} marked {row},{col}")
        print(board.toString())
        # win_check returning true means a rematch is desired
        rematch = win_check(sock, board, "You lose.")
        if rematch != "none":
            return rematch
        #if board.checkForWin():
        #    print("Your loss.")
        #    val = input("Play again? [Y/N]\n").casefold()
        #    if val == 'y':
        #        return True
        #    else:
        #        print("Ending game session...")
        #        sock.send(b'e')
        #        sock.close()
        #        exit()
        # Our move
        move_success = False
        board.setPlayer(player)
        move_success = my_move(board,sock)
        while not move_success:
            move_success = my_move(board,sock)
        row = str(move_success[0])
        col = str(move_success[1])
        print(f"Player {player} marked {row},{col}")
        print(board.toString())
        sock.send(bytes("m"+row+col,"utf8"))
        # Returning true means a rematch is desired
        rematch = win_check(sock, board, "You win.")
        if rematch != "none":
            return rematch

# I think, in Tom's code, the client (o) writes the rematch desire, then takes
# And vice versa, the server code receives the rematch desire, and then gives
# We'll try doing it like that, and if it fails, we'll see why.
# If you're the server, receive the desired rematch.
def handle_rematch(sock,player,rematch):
    if not rematch:
        print("You denied a rematch, closing...")
        # receive and discard from client
        if player == 'x':
            sock.recv(1024)
            sock.send(b'n')
            sock.close()
            exit()
        else:
            sock.send(b'n')
            sock.close()
            exit()
    else:
        print("Requesting rematch...")
        # receive from client if server
        if player == 'x':
            desire = str(sock.recv(1024),'utf8')[0]
            if desire == 'y':
                sock.send(b'y')
                return True
        else:
            sock.send(b'y')
            desire = str(sock.recv(1024),'utf8')[0]
            if desire == 'y':
                return True
port = 9000
player = Exception
sock = Exception

if len(sys.argv) != 2:
    exit("Please pass 'x' or 'o' as the only arg. Choosing 'x' means you're host.")
if sys.argv[1].casefold() != 'x' and sys.argv[1].casefold() != 'o':
    exit("Invalid player choice")
player = sys.argv[1]
print(f"player selected: {player}")
# Initalize sockets
if player == 'o':
    sock = client_sock(port)
else:
    # '' means any address, i.e. 0.0.0.0
    sock = server_sock('',port)


board = ttu.TicTacToe() 
while True:
    rematch = begin_game(board,sock,player)
    handle_rematch(sock,player,rematch)
    # Rematch begins, swap the players for next game call
    if player == 'x':
        player = 'o'
    elif player == 'o':
        player = 'x'
    else:
        exit("Critical error, invalid player somehow.")
