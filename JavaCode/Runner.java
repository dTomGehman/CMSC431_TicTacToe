/**
 * Runner.java
 *
 * Main procedure and helpers for playing Tic Tac Toe.
 * Run two instances to connect and play a game.
 * Run java Runner x first, then java Runner o.
 */

import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;




public class Runner
{
  private static final int PORT = 9000;
  private static final int MESSAGE_SIZE = 1024;

  private static Scanner keyboard = new Scanner(System.in);
  private static TicTacToe game = new TicTacToe();

  private static Socket socket;
  private static DataInputStream input;
  private static OutputStream output;
  private static BufferedWriter log;

  // 0 = O player,, 1 = X player
  private static int myPlayer;



  /**
   * Set up the connection and play games until a player exits.
   */
  public static void main(String[] args)
  {
    if (args.length != 1
            || (!args[0].equalsIgnoreCase("x")
            && !args[0].equalsIgnoreCase("o")))
    {
      System.out.println("Usage: java Runner x OR java Runner o");
      return;
    }

    myPlayer = args[0].equalsIgnoreCase("x") ? 1 : 0;
    String logName = "game-log-" + args[0].toLowerCase() + ".txt";

    try (Socket connection = connect();
         BufferedWriter logFile =
                 new BufferedWriter(new FileWriter(logName, true)))
    {
      socket = connection;
      log = logFile;
      input = new DataInputStream(socket.getInputStream());
      output = socket.getOutputStream();

      boolean playing = true;

      while (playing)
      {
        if (!playGame())
        {
          break;
        }

        // Game over -> switch players and ask to play again.
        myPlayer = 1 - myPlayer;
        playing = playAgain();
      }
    }
    catch (EOFException exception)
    {
      System.out.println("The other player disconnected.");
    }
    catch (IOException exception)
    {
      System.out.println("Connection or file error: "
              + exception.getMessage());
    }
    finally
    {
      keyboard.close();
    }
  }

  /**
   * X = server. O = client.
   */
  private static Socket connect() throws IOException
  {
    if (myPlayer == 1)
    {
      try (ServerSocket server = new ServerSocket(PORT))
      {
        System.out.println("Waiting for a player...");
        return server.accept();
      }
    }

    System.out.print("Server IP address: ");
    String address = keyboard.nextLine().trim();

    return new Socket(address, PORT);
  }

  /**
   * Take turns making moves until a win, stalemate, or exit
   * return false if a player exits
   */
  private static boolean playGame() throws IOException
  {
    game.resetGame();

    // X moves first, count valid moves to determine stalemate.
    int currentPlayer = 1;
    int moves = 0;


    System.out.println("You are " + playerSymbol(myPlayer));
    System.out.print(game);


    while (moves < 9)
    {
      game.setPlayer(currentPlayer);

      if (currentPlayer == myPlayer)
      {
        // Our move
        String message = getMove();

        send(message);

        if (message.equals("e"))
        {
          return false;
        }
      }
      else
      {
        // Opponent's move
        System.out.println("Waiting for opponent's move...");
        String message = receive();

        if (message.equals("e"))
        {
          System.out.println("The other player exited.");
          return false;
        }

        if (!validMoveFormat(message))
        {
          System.out.println("The other player sent a bad move.");
          send("e");
          return false;
        }

        int row = message.charAt(1) - '0';
        int column = message.charAt(2) - '0';

        if (!game.makeMove(row, column))
        {
          System.out.println("The other player chose a taken spot.");
          send("e");
          return false;
        }
      }

      moves++;
      System.out.print(game);

      if (game.checkForWin())
      {
        if (currentPlayer == myPlayer)
        {
          System.out.println("You win!");
        }
        else
        {
          System.out.println("You lose.");
        }

        return true;
      }

      currentPlayer = 1 - currentPlayer;
    }

    System.out.println("Stalemate.");
    return true;
  }

  /**
   * Read a move and update the board
   * Move format: mRC, row and column from 0 to 2
   * e exits the game
   */
  private static String getMove()
  {
    while (true)
    {
      System.out.print("Move (mRC, row/column 0-2, or e to exit): ");

      if (!keyboard.hasNextLine())
      {
        return "e";
      }

      String message = keyboard.nextLine().trim().toLowerCase();

      if (message.equals("e"))
      {
        return message;
      }

      if (!validMoveFormat(message))
      {
        System.out.println("Use m00 through m22, or e to exit.");
        continue;
      }

      int row = message.charAt(1) - '0';
      int column = message.charAt(2) - '0';

      if (game.makeMove(row, column))
      {
        return message;
      }

      System.out.println("That spot is already taken.");
    }
  }

  /**
   * Check that the move contains m and two valid coordinates
   */
  private static boolean validMoveFormat(String message)
  {
    return message.length() == 3
            && message.charAt(0) == 'm'
            && message.charAt(1) >= '0'
            && message.charAt(1) <= '2'
            && message.charAt(2) >= '0'
            && message.charAt(2) <= '2';
  }

  /**
   * The new X answers first, then the new O
   * Play again if both answer y
   */
  private static boolean playAgain() throws IOException
  {
    if (myPlayer == 1)
    {
      String answer = getReplayAnswer();
      send(answer);

      if (answer.equals("n"))
      {
        return false;
      }

      return receive().equals("y");
    }

    String otherAnswer = receive();

    if (!otherAnswer.equals("y"))
    {
      return false;
    }

    String answer = getReplayAnswer();
    send(answer);

    return answer.equals("y");
  }

  /**
   * Ask the player to enter y or n
   */
  private static String getReplayAnswer()
  {
    while (true)
    {
      System.out.print("Play again as "
              + playerSymbol(myPlayer) + "? [y/n]: ");

      if (!keyboard.hasNextLine())
      {
        return "n";
      }

      String answer = keyboard.nextLine().trim().toLowerCase();

      if (answer.equals("y") || answer.equals("n"))
      {
        return answer;
      }

      System.out.println("Enter y or n.");
    }
  }

  /**
   * Send a message through the socket and save it to the log
   */
  private static void send(String message) throws IOException
  {
    byte[] buffer = new byte[MESSAGE_SIZE];
    byte[] text = (message + "\n").getBytes(StandardCharsets.US_ASCII);

    System.arraycopy(text, 0, buffer, 0, text.length);

    output.write(buffer);
    output.flush();

    logMessage(myPlayer, message,
            socket.getLocalAddress().getHostAddress(),
            socket.getInetAddress().getHostAddress());
  }

  /**
   * Read a message from the socket and save it to the log
   */
  private static String receive() throws IOException
  {
    byte[] buffer = new byte[MESSAGE_SIZE];

    // Wait for all 1024 bytes even if they arrive separately
    input.readFully(buffer);

    int length = 0;

    // Stop at the newline or zero padding
    while (length < buffer.length
            && buffer[length] != 0
            && buffer[length] != '\n')
    {
      length++;
    }

    String message = new String(buffer, 0, length,
            StandardCharsets.US_ASCII).trim();

    logMessage(1 - myPlayer, message,
            socket.getInetAddress().getHostAddress(),
            socket.getLocalAddress().getHostAddress());

    return message;
  }


  /**
   * Log the player, message, source IP, and destination IP
   */
  private static void logMessage(int player, String message,
                                 String from, String to) throws IOException
  {
    log.write("Player: " + playerSymbol(player)
            + " | Move: " + message
            + " | IP From: " + from
            + " | IP To: " + to);
    log.newLine();
    log.flush();
  }


  /**
   * Return X or O for the player number
   */
  private static String playerSymbol(int player)
  {
    return player == 1 ? "X" : "O";
  }
}