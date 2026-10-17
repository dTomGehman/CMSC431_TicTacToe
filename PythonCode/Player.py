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
    return (initsock,connsock)

def opponent_move(board,sock):
    raw = sock.recv(10)
    if len(raw) == 0:
        exit("Critical error, client has disconnected.")
    move = str(raw, "ascii")
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
        exit("Invalid move, not p or e, no r/c spec")
    row = int(move[1])
    col = int(move[2])
    if not board.makeMove(row,col):
        print("Move not allowed, board may be desynced?",flush=True)
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
        print("Invalid move, not p or e, no r/c spec")
        return False
    row = int(move[1])
    col = int(move[2])
    if not board.makeMove(row,col):
        print("Board spot in use, or other err")
        return False
    else:
        return (row,col)

def begin_game(board,sock):
    playerTurn = 'x'
    board.setPlayer(playerTurn)
    if player == 'x':
        # Make first move
        pass
    while True:
        # Recieve opponents move and update board
        move_success = opponent_move(board,sock)
        while not move_success:
            print("NOT MOVE SUCCESS",flush=True)
            opponent_move(board,sock)
        row = move_success[0]
        col = move_success[1]
        print(f"Player {playerTurn} marked {row},{col}",flush=True)
        print(board.toString())
        move_success = False
        move_success = my_move(board,sock)
        while not move_success:
            print("NOT MOVE SUCCESS",flush=True)
            move_success = my_move(board,sock)
        row = move_success[0]
        col = move_success[1]
        sock.send(bytes("m"+row+col))

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

board = ttu.TicTacToe() 
begin_game(board,sock)
