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
    move = input("Please enter a move:\n ")
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
    row = int(move[1])
    col = int(move[2])
    if not board.makeMove(row,col):
        print("Board spot in use, or out of bounds")
        return False
    else:
        return (row,col)

def begin_game(board,sock,player):
    # lol
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
        if board.checkForWin():
            print("Your loss.")
            sock.close()
            exit()
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
        if board.checkForWin():
            print("You win! Congrats! すばらしい!")
            sock.close()
            exit()

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
begin_game(board,sock,player)
