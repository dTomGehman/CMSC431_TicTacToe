import TicTacUtils as ttu
import sys
import socket
# Note: socket module is "transliteration of the Unix system call and library interface"
# In other words, most docs for socket are in man pages, man 7 socket, man 7 ip, and man 2 socket for example.

# Choose player
if len(sys.argv) != 2:
    exit("Please pass 'x' or 'o' as the only arg. Choosing 'x' means you're host.")
if sys.argv[1] != 'x' and sys.argv[1] != 'o':
    exit("Invalid player choice")
player = sys.argv[1]
print(f"player: {player}")

# Get socket addr data
port = 9000
if player == 'o':
    usr_input = input("Please enter destination IP, using port 9000. Blank IP for localhost.")
    if usr_input != '':
        addr = usr_input
# '' for INADDR_ANY in address
initsock = socket.create_server(address=('',port),family=socket.AF_INET)
initsock.listen()
print("listening...")
retval = initsock.accept()
connsock = retval[0]
retaddr = retval[1]
print("connection socket successfully created...")
print(connsock)
firstmsg = connsock.recv(100)
# Begin the game
board = ttu.TicTacToe() 
print(board.toString())


# RANDOM 4AM THEORY, THE CONNECTION IS CLOSING BECAUSE RECV IS SETTING A MAX BEFORE CONNECTING CLOSE, READ THE DOCS 
