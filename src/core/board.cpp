#include "core/board.h"
#include "core/tile.h"

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <QDebug>

using namespace std;

// Constructor initializes board size and game state
Board::Board(int rows, int columns)
{
    this->rows = rows;
    this->columns = columns;
    pointsScoredLastRound = 0;
    tileCollisionLastRound = false;
    // create empty board
    init();
}

// Copy constructor performs deep copy
Board::Board(const Board &other)
{
    rows = other.rows;
    columns = other.columns;
    init();

    // copy every tile one by one
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j) {
            if (other.board[i][j] == NULL)
                board[i][j] = NULL;
            else
                board[i][j] = new Tile(*(other.board[i][j]));
        }
}

Board::~Board()
{
    // Destructor deletes dynamically allocated tiles
    for (int i = 0 ; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            delete board[i][j];
}

// Creates empty board structure
void Board::init()
{
    // create row count
    board.resize(rows);
    // create column count for each row
    for (int i = 0; i < rows; ++i)
        board[i].resize(columns);

    // initialize all cells as empty
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            board[i][j] = NULL;
}

// Finds random empty position
QVector<int> Board::freePosition()
{
    QVector<int> pos;
    // no free position exists
    if (full()) {
        pos.append(-1);
        pos.append(-1);
    } else {
        int i,j;
        // keep generating random positions until empty cell is found
        do {
            i = rand() % rows;
            j = rand() % columns;
        } while (board[i][j] != NULL);
        pos.append(i);
        pos.append(j);
    }
    return pos;
}

// Checks whether current board differs from another board
bool Board::changed(Board& other) const
{
    if (columns != other.columns | rows != other.rows)
        return false;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; ++j)
            if ( ( (board[i][j] == NULL && other.board[i][j] != NULL) ||
                   (board[i][j] != NULL && other.board[i][j] == NULL) ) ||
                 ( (board[i][j] != NULL && other.board[i][j] != NULL) &&
                   board[i][j]->getValue() != other.board[i][j]->getValue()) )
                return true;
    return false;
}

// Resets the game
void Board::reset()
{
    pointsScoredLastRound = 0;
    tileCollisionLastRound = false;

    for (int i = 0; i < rows ; ++i) {
        for (int j = 0; j < columns; ++j) {
            delete board[i][j];
            board[i][j] = NULL;
        }
    }

    QVector<int> start = freePosition();
    board[start[0]][start[1]] = new Tile();
    start = freePosition();
    board[start[0]][start[1]] = new Tile();

}

// Returns tile at given position
Tile* Board::getTile(int i, int j)
{
    return board[i][j];
}

// Main movement function
void Board::move(Direction direction)
{
    // keep copy of board before move
    Board pre_move_board(*this);
    // reset move flags
    prepareForNextMove();
    // move tiles according to direction

    switch (direction) {
    case UP:
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                moveVertically(i,j,UP);
        break;
    case DOWN:
        for (int i = rows-1; i >= 0; --i)
            for (int j = 0; j < columns; ++j)
                moveVertically(i,j,DOWN);
        break;
    case LEFT:
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                moveHorizontally(i,j,LEFT);
        break;
    case RIGHT:
        for (int i = 0; i < rows; ++i)
            for (int j = columns-1; j >= 0; --j)
                moveHorizontally(i,j, RIGHT);
    }

    // If the board has changed then put a new tile
    if (changed(pre_move_board) /*&& !tileCollisionLastRound*/) {
        QVector<int> newpos = freePosition();
        if (newpos[0] != -1) {

            int chance = rand() % 100;

            // 90% chance for 2
            if (chance < PROB_2) {
                board[newpos[0]][newpos[1]] = new Tile(2);
            }
            // 10% chance for 4
            else {
                board[newpos[0]][newpos[1]] = new Tile(4);
            }
        }
    }
    // notify GUI observers
    notifyObservers();
}

// Checks if any valid move exists
void Board::prepareForNextMove()
{
    tileCollisionLastRound = false;
    pointsScoredLastRound = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (board[i][j] != nullptr) {
                board[i][j]->setUpgratedThisMove(false);
            }
        }
    }
}

bool Board::movePossible() const
{
    if (full()) {
        // check if there is still a move to do
        Board newBoard(*this);
        newBoard.move(UP);
        if (changed(newBoard)) return true;
        newBoard.move(DOWN);
        if (changed(newBoard)) return true;
        newBoard.move(LEFT);
        if (changed(newBoard)) return true;
        newBoard.move(RIGHT);
        if (changed(newBoard)) return true;

        // no possible move
        return false;
    }
    // board still has empty cells
    else {
        return true;
    }
}

// Handles horizontal movement
void Board::moveHorizontally(int i, int j, Direction dir)
{
    if (board[i][j] != NULL) {
        bool tileCollision = false;
        int newj;
        if (dir == RIGHT)
            newj = j + 1;
        // to the left to the left
        else
            newj = j - 1;

        // keep going in dir direction until we encounter something or get out of bounds
        while (inbounds(i,newj) && board[i][newj] == NULL) {
            if (dir == RIGHT)
                newj++;
            else
                newj--;
        }

        // out of bounds
        if (!inbounds(i,newj)) {
            if (dir == RIGHT)
                board[i][columns-1] = board[i][j];
            else
                board[i][0] = board[i][j];
        }
        // collision
        else {
            // collision with same value
            if (board[i][newj]->getValue() == board[i][j]->getValue() &&
                !board[i][newj]->getUpgratedThisMove()) {

                tileCollision = true;
                handleCollision(i, newj);
            }
            // collision with other value
            else {
                if (dir == RIGHT)
                    board[i][newj-1] = board[i][j];
                else
                    board[i][newj+1] = board[i][j];
            }
        }
        // remove old tile position
        if ( (dir == RIGHT && newj-1 != j) || (dir == LEFT && newj+1 != j) || tileCollision )
            board[i][j] = NULL;

        // store collision info
        if (tileCollision)
            tileCollisionLastRound = true;
    }
}

// Handles vertical movement
void Board::moveVertically(int i, int j, Direction dir)
{
    if (board[i][j] != NULL) {
        bool tileCollision = false;
        int newi;
        if (dir == UP)
            newi = i - 1;
        // down
        else
            newi = i + 1;

        // continue while next cell is empty
        while (inbounds(newi,j) && board[newi][j] == NULL) {
            if (dir == UP)
                newi--;
            else
                newi++;
        }

        // reached border
        if (!inbounds(newi,j)) {
            if (dir == UP)
                board[0][j] = board[i][j];
            else
                board[rows-1][j] = board[i][j];
        }
        // collision
        else {
            // same value -> merge
            if (board[newi][j]->getValue() == board[i][j]->getValue() &&
                !board[newi][j]->getUpgratedThisMove()) {
                tileCollision = true;
                handleCollision(newi, j);
            }
            // different value -> place next to obstacle
            else {
                if (dir == UP)
                    board[newi+1][j] = board[i][j];
                else
                    board[newi-1][j] = board[i][j];
            }
        }
         // remove old tile position
        if ( (dir == UP && newi+1 != i) || (dir == DOWN && newi-1 != i) || tileCollision )
            board[i][j] = NULL;

        // store collision info
        if (tileCollision)
            tileCollisionLastRound = true;
    }

}

// Merges tiles
void Board::handleCollision(int i, int j)
{
    // double tile value
    board[i][j]->upgrade();
    // mark tile as merged this turn
    board[i][j]->setUpgratedThisMove(true);
     // add score
    pointsScoredLastRound += board[i][j]->getValue();
}

// Checks if board has empty cell
bool Board::full() const
{
    bool full = true;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            if (board[i][j] == NULL)
                full = false;
    return full;
}

// Checks are indices inside board
bool Board::inbounds(int i, int j)
{
    return (i >= 0 && j >= 0 && i < rows && j < columns);
}
