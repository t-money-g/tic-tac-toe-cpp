/************************************************************************/
/* Tic-Tac-Toe Psyonix programming assignment 
/* Author: Terrence Young - tyoung2005@gmail.com 2020-06-22
/* Code referenced:
*       Beginning C++ Through Game Programming fourth edition - Michale Dawson
*       Programming Principles and Practices second edtion - Bjarne Stroustrup 
*       Cplusplus.com/
/************************************************************************/ 



#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <random>
#include <chrono>
#include <functional>
#include <cmath>

using namespace std;

enum class Piece { X,O };
enum class State { NEW, PLAYER_1_WIN, PLAYER_2_WIN, DRAW, QUIT };

constexpr int BOARD_SIZE = 9; 
constexpr int MAX_PLAYERS = 2;

constexpr int FIRST_PLAYER = 0; 
constexpr int SECOND_PLAYER = 1;
const char EMPTY = ' ';
const char XCHAR = 'X'; 
const char OCHAR = 'O';

constexpr int RANDMIN = 1;
constexpr int RANDMAX = 100;

const int WINNING_ROWS[8][3] = {{0, 1, 2},
                                {3, 4, 5},
                                {6, 7, 8},
                                {0, 3, 6},
                                {1, 4, 7},
                                {2, 5, 8},
                                {0, 4, 8},
                                {2, 4, 6}};

constexpr int TOTAL_ROWS = 8; 


// fwds for input utilities 
int get_int();                  // read an int from cin
int get_int(int low, int high); // read an int in [low:high] from cin 
void skip_to_int();             // read until we find an integer from input stream 
int randint(int min, int max);  // generate random numbers from a range of ints

int randint(int min, int max)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    return std::uniform_int_distribution<int>{min, max}(generator);
}

struct Player {

    string name; 
    Piece piece;
    int games_won = 0;
    char getPieceChar();
    void setPieceChar(char letter);
};

char Player::getPieceChar()
{
    if (piece == Piece::X) return XCHAR; 
    return OCHAR;
}

void Player::setPieceChar(char letter)
{
    if (letter == XCHAR) piece = Piece::X; 
    else if (letter == OCHAR) piece = Piece::O;
}

struct Board {
    Board() { board = vector<char>(BOARD_SIZE, EMPTY);}
    void print();
    bool isCellEmpty(int cell);
    void setPiece(unsigned cell, char piece); 
    bool checkDraw();
    const char GetCell(unsigned cellid);
    void resetBoard();
    bool checkWinner(); 
    
private:
     vector<char>board; 
};

struct Game{
    
    Game() {  board = Board(); game_no = 1; game_won = false;}

    bool makeMove(int moveIdx, char piece);
    void setPlayers(const Player& player1, const Player& player2); 
    int determineStartingPlayer();  
    void setStartingPlayer(int player); 
    void start();
    void displayBoard(); 
    bool checkBoardForWinner();
    void runGame();
    void playerTurn(int player); 
    void setWinningPlayer(int player);
    void gameOver(); 
    void quitGame();
    void startPlayerChoosePiece();
    const State getState(); 
    void setState(State newState);
    int getGameNumber() { return game_no; }
    void incrementGameNumber() { ++game_no; }
    void resetGame(int winningPlayer); 

private:
    vector<Player> players;
    Board board;
    int game_no;
    bool game_won;
    State game_state; 
};

int Game::determineStartingPlayer()
{

    int magicNumber = randint(RANDMIN, RANDMAX);
    Player firstPlayer = players[FIRST_PLAYER];
    Player secondPlayer = players[SECOND_PLAYER];

    cout << "Lets determine the start player by playing a guessing game!\n\n";
    cout << "Whoever guesses the number closest to the secret will start first and choose their piece\n";

    cout << firstPlayer.name << "... \n";
    int player1Num = get_int(RANDMIN, RANDMAX); 

    cout << secondPlayer.name << "... \n";
    int player2Num = get_int(RANDMIN, RANDMAX);

    //cout << " Pssh, secret num was " << magicNumber << " all along " << '\n'; 

    int player1Result =  magicNumber - player1Num;
    int player2Result =  magicNumber - player2Num;

    player1Result = abs(player1Result);
    player2Result = abs(player2Result);

    if(player1Result < player2Result) 
    {
        cout << firstPlayer.name << " was closer with a result of " << player1Result << '\n';  
        return FIRST_PLAYER; 
    }
    else if (player2Result < player1Result) {
        cout << secondPlayer.name << " was closer with a result of " << player2Result << '\n';
        return SECOND_PLAYER; 
    }
    
    cout << "Unable to determine who was closer to magic number defaulting to first player\n" <<  firstPlayer.name << '\n'; 
    return FIRST_PLAYER;
}



void Game::setStartingPlayer(int player)
{
    Player temp = players[player];

    Player player1 = players[FIRST_PLAYER];
    Player player2 = players[SECOND_PLAYER]; 
    
    if(player1.name == temp.name)
        return; 

    players[FIRST_PLAYER] = player2;
    players[SECOND_PLAYER] = player1; 

}

void Game::playerTurn(int player) 
{
    Player currentPlayer = players[player];
    cout << currentPlayer.name << ", your move.\n";
    
    bool move_successful = false; 
    while(!move_successful) {

        int move = get_int(1, 9); 
        move_successful = makeMove(move, currentPlayer.getPieceChar());
    }
   
}


void Game::setWinningPlayer(int player)
{
    
    if(player == FIRST_PLAYER)
        setState(State::PLAYER_1_WIN);
    else if(player == SECOND_PLAYER)
        setState(State::PLAYER_2_WIN);

    cout << players[player].name << " wins!\n\n";
    players[player].games_won += 1; 
}

void Game::gameOver()
{
    cout << "Game over\n";
    
    cout << "Score:";
    cout << "\n\t" << players[FIRST_PLAYER].name << " has " << players[FIRST_PLAYER].games_won; 
    cout << "\n\t" << players[SECOND_PLAYER].name << " has " << players[SECOND_PLAYER].games_won;  
    cout << "\n";
  
}

void Game::quitGame()
{
    cout << "Thanks for playing!\n";
    exit(0);
}

void Game::startPlayerChoosePiece()
{
    Player& startingPlayer = players[FIRST_PLAYER]; 
    Player& secondPlayer = players[SECOND_PLAYER];
    cout << startingPlayer.name << " choose your piece, will you be the glorious 'X' or the wonderful 'O'?\n"; 
    cout << "Enter 1 for 'X'\n";
    cout << "Enter 2 for '0'\n";

    int choice = get_int(1,2);

    switch (choice) {
        case 1:
            startingPlayer.setPieceChar(XCHAR);
            secondPlayer.setPieceChar(OCHAR); 
            break;
        case 2:
            startingPlayer.setPieceChar(OCHAR);
            secondPlayer.setPieceChar(XCHAR);
            break;
        default: 
            cout << "Didn't recognize that number defaulting to 'X'\n"; 
            startingPlayer.setPieceChar(XCHAR);
            secondPlayer.setPieceChar(OCHAR);
    }

    cout << "Huzzah! " << startingPlayer.name << " will be '" << startingPlayer.getPieceChar() << "'\n";
    cout << ",and " << secondPlayer.name << " will be '" << secondPlayer.getPieceChar() <<"'\n";
}

const State Game::getState()
{
    return game_state;
}

void Game::setState(State newState)
{
    game_state = newState;
}

void Game::resetGame(int winningPlayer)
{
    board.resetBoard();
    board = Board();
    incrementGameNumber();
    setStartingPlayer(winningPlayer);
    setState(State::NEW);
    game_won = false;
}

void Game::runGame()
{
    while(!game_won)
    {
        for(unsigned i =0; i < MAX_PLAYERS; ++i)
        {
            displayBoard();
            playerTurn(i);
            if(checkBoardForWinner())
            {
                setWinningPlayer(i);
                game_won = true;
                break;
            }
            
            if (board.checkDraw())
            {
                setState(State::DRAW);
                break;
            }
        }

        if(getState() == State::DRAW)
            break;
    }
    displayBoard();
}

void Game::displayBoard()
{
    board.print();
}

bool Game::checkBoardForWinner()
{
    return board.checkWinner();
        
}

bool Game::makeMove(int moveIdx, char piece)
{
   --moveIdx;
   if( board.isCellEmpty(moveIdx))
   {
       try {
            board.setPiece(moveIdx, piece);
       }
       catch(out_of_range)
       {
            cout << "No such cell: " << moveIdx << "\n";
            return false; 
       }
   }
   else 
   {
       cout << "That cell is occupied.\n";
       return false; 
   }

   return true;

}

void Game::setPlayers(const Player& player1, const Player& player2) 
{

    players.push_back(player1);
    players.push_back(player2);

}

void Game::start()
{
    cout << "Welcome to Tic-Tac-Toe\n";

    cout << "Each player will take turns by entering a number 1-9.\n";
    cout << "Which corresponds to a position on the board:\n";
    
    cout << " 1 | 2 | 3 \n";
    cout << "-----------\n";
    cout << " 4 | 5 | 6 \n";
    cout << "-----------\n";
    cout << " 7 | 8 | 9 \n";


    Player player1; 
    Player player2; 
    cout << "Enter player name:"; 
    cin >> player1.name; 
    
    cout << "Enter opponent name:"; 
    cin >> player2.name; 

    cout << "Welcome, " << player1.name << " & " << player2.name << "\n"; 

    setPlayers(player1, player2);
    
}


void Board::print() 
{
    cout <<"\n\t " << board[0] << "|" << board[1] << "|" << board[2];
    cout <<"\n\t " << "------"; 
    cout <<"\n\t " << board[3] << "|" << board[4] << "|" << board[5]; 
    cout <<"\n\t " << "------"; 
    cout <<"\n\t " << board[6] << "|" << board[7] << "|" << board[8];
    cout <<"\n\n";
} 



bool Board::isCellEmpty(int cell)
{
    char charCell; 
    try {
        charCell = GetCell(cell);
    }
    catch(out_of_range ) 
    {
        cout << "No such cell " << '\n'; 
        return false; 
    }
    return EMPTY == charCell;
}

void Board::setPiece(unsigned cell, char piece)
{   
   
    if(cell < 0 || board.size() <= cell )
        throw out_of_range("Index out of range");

    board[cell] = piece; 
}

bool Board::checkDraw()
{

    int count_filled = 0; 
    for(unsigned i = 0; i < BOARD_SIZE; ++i)
    {
        if(!isCellEmpty(i))
            ++count_filled; 
    }

    if (count_filled == BOARD_SIZE)
        return true; 
    return false;
}

const char Board::GetCell(unsigned cellId)
{
     if(board.size() <= cellId )
        throw out_of_range("Index out of range\n");
     
    return board[cellId];; 
}

void Board::resetBoard()
{
    board.clear();
}

bool Board::checkWinner()
{
    for (int row = 0; row < TOTAL_ROWS; ++row)
    {
        if ((board[WINNING_ROWS[row][0]] != EMPTY) &&
            (board[WINNING_ROWS[row][0]] == board[WINNING_ROWS[row][1]]) &&
            (board[WINNING_ROWS[row][1]] == board[WINNING_ROWS[row][2]]))
        {
            return true;
        }
    }

    return false;
}

int main()
{
    Game currentGame;
    int startPlayer = 0;

    while(true) { 

        if(currentGame.getGameNumber() == 1)
        {
            currentGame.start();
            startPlayer = currentGame.determineStartingPlayer();
            currentGame.setStartingPlayer(startPlayer);
        }

        currentGame.startPlayerChoosePiece();
        currentGame.runGame();

        State currentState = currentGame.getState();

        switch (currentState) {
            case State::DRAW :
                cout << "And the game is a draw!\n";
                break;
            case State::PLAYER_1_WIN:
                startPlayer = FIRST_PLAYER; 
                break;
            case State::PLAYER_2_WIN: 
                startPlayer = SECOND_PLAYER;
                break;
            default:
                cout << "\n";
        }
        currentGame.gameOver();

        char ch;

        cout << "Would you like to play again? 'y' or 'n'\n";
  
        while(cin >> ch) {

            if( ch == 'y') {
                 currentGame.resetGame(startPlayer);
                 break;
            }
            else if (ch == 'n')
                currentGame.quitGame();
            else
                cout << "Please enter 'y' or 'n'\n";
        }
    }
    currentGame.quitGame();
}


int get_int()
{
    int n = 0;
    while(true) {
        if (cin >> n) return n; 
        cout << "Sorry, that was not a number; please try again\n";
        skip_to_int();
    }
}   
int get_int(int low, int high) 
{
    cout << "Please enter an integer in the range "
        << low << " to " << high << " (inclusive):"; 

    while(true) {
        int n = get_int();      
        if(low <= n && n<=high) return n;
        cout << "Sorry "
             << n << " is not in the [" << low << ':' << high
             << "] range; please try again\n";
    
    }
}
void skip_to_int()
{
    if(cin.fail()) { 
        cin.clear();
        for(char ch; cin>>ch;) {
            if(isdigit(ch)){
                cin.unget(); 
                return; 
            }
        }
    }
    throw runtime_error("no input");

}