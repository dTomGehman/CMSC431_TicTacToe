#!/usr/bin/env python
"""
Author: Daniel Morine
This is the python server/client for the tictactoe networking game. 
It was entirely authored by Daniel Morine, with comments from tom.
The only sources consulted were python.org sockets page, and man pages.
"""
import TicTacUtils as ttu
import sys
import socket
import ipaddress
import time 
# Note: socket module is "transliteration of the Unix system call and library interface"
# In other words, most docs for socket are in man pages, man 7 socket, man 7 ip, and man 2 socket for example.

"""
client_sock(port)
Daniel Morine
ask for the server's IP address and connect to it using a client socket.
"""
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

"""
server_sock(addr, port)
Daniel Morine
set up the server socket and wait for the other player to connect.
"""
def server_sock(addr, port):
    initsock = socket.create_server(address=(addr,port),family=socket.AF_INET)
    initsock.listen()
    connsock = initsock.accept()[0]
    return connsock


"""
opponent_move(board, sock)
Daniel Morine
get the other player's move from the socket and put it on the board.
also handles exit messages and invalid moves.
"""
def opponent_move(board,sock):
    move = sock.recv(2048).decode('utf8').rstrip('\x00')
    write_log(receiver=True,msg=move)
    if len(move) == 0:
        exit("Critical error, client has disconnected.")
    first_char = move[0]
    if first_char == 'e':
        print("Opponent triggered exit, closing socket...")
        sock.close()
        exit("Exiting game...")
    if len(move) <= 2:
        exit("Invalid move, not exit, no r/c spec. Exiting...")
    row = int(move[1])
    col = int(move[2])
    if not board.makeMove(row,col):
        print("Invalid move received from server, board desynced?")
        return False
    else:
        return (row,col)

"""
my_move(board, sock)
Daniel Morine
ask the player for a move, check if it's valid and put it on the board.
"""
def my_move(board,sock):
    move = input("Please enter a move (e.g. m11 for middle box):\n")
    if len(move) < 1:
        return False
    first_char = move[0]
    if first_char == 'e':
        print("You triggered exit, closing socket...")
        bytestring = b'e'
        sock.sendall(bytestring.ljust(1024, b'\0'))
        write_log(receiver=False,msg='e')
        sock.close()
        exit("Exiting game...")
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

"""
win_check(sock, board, msg)
Daniel Morine
check if someone won or if the game ended in a draw.
ask if the player wants to play again.
"""
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
"""
begin_game(board, sock, player)
Daniel Morine
start a new game and take turns receiving the opponent's moves and
making your own. check for wins and draws, then handle the end of the game.
"""
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
        bytestring = bytes("m"+row+col,"utf8")

        sock.sendall(bytestring.ljust(1024, b'\0'))
        write_log(receiver=False,msg=bytestring.decode('utf8'))
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
        bytestring = bytes("m"+row+col,"utf8")
        sock.sendall(bytestring.ljust(1024, b'\0'))
        write_log(receiver=False,msg=bytestring.decode('utf8'))
        # Returning true means a rematch is desired
        rematch = win_check(sock, board, "You win.")
        if rematch != "none":
            return rematch
"""
not_me(player)
Daniel Morine
Returns the other player's id (x or o), opposite of the passed players.
"""
def not_me(player):
    player = player.casefold()
    if player == 'x':
        return 'o'
    elif player == 'o':
        return 'x'
    else:
        return 'error'
"""
write_log(receiver,msg)
Daniel Morine
Just a helper function to write to the log file with ip, port, and player. As well as whether sending or receiving.
"""
def write_log(receiver, msg):
    if receiver:
        fileobj.write(f"From player {not_me(player)} ({opponent_ip}:{opponent_port}), received --> {msg}\n")
    else:
        fileobj.write(f"From player {player} ({my_ip}:{my_port}), sent --> {msg}\n")

"""
handle_rematch(sock, player, rematch)
Daniel Morine
ask the other player about a rematch and exchange their answers.
if either player says no, close the connection.
"""
def handle_rematch(sock,player,rematch):
    if not rematch:
        print("You denied a rematch, closing...")
        # receive and discard from client
        if player == 'x':
            trash = sock.recv(1024).decode('utf8').rstrip('\x00')
            write_log(receiver=True,msg=trash)
            bytestring = b'n'
            sock.sendall(bytestring.ljust(1024, b'\0'))
            write_log(receiver=False,msg='n')
            sock.close()
            exit()
        else:
            bytestring = b'n'
            sock.sendall(bytestring.ljust(1024, b'\0'))
            write_log(receiver=False,msg='n')
            sock.close()
            exit()
    else:
        print("Requesting rematch...")
        # receive from client if server
        if player == 'x':
            desire = sock.recv(1024).decode('utf8')[0]
            write_log(receiver=True,msg=desire)
            if desire == 'y':
                bytestring = b'y'
                sock.sendall(bytestring.ljust(1024, b'\0'))
                write_log(receiver=False,msg='y')
                return True
        else:
            bytestring = b'y'
            sock.sendall(bytestring.ljust(1024, b'\0'))
            write_log(receiver=False,msg='y')
            desire = sock.recv(1024).decode('utf8')[0]
            write_log(receiver=True,msg=desire)
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

# Open a file for logging
filename = "log_"+str(time.time()).split('.')[0]
fileobj = open(filename,'w') 
opponent_ip, opponent_port = sock.getpeername()
my_ip, my_port = sock.getsockname()

board = ttu.TicTacToe() 
while True:
    rematch = begin_game(board,sock,player)
    handle_rematch(sock,player,rematch)
    # Rematch begins, swap the players for next game call
    player = not_me(player)
