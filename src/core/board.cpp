// board.cpp - all the movement, merging and spawning logic lives here
#include "core/board.h"
#include "core/tile.h"

#include <cstdlib>
#include <ctime>

#include <QDebug>

// ─── construction / destruction ─────────────────────────────────────────────

Board::Board(int rows, int columns)
{
    this->rows = rows;
    this->columns = columns;
    pointsScoredLastRound = 0;
    tileCollisionLastRound = false;
    init();
}

// deep copy - we use this for undo snapshots and move validation
Board::Board(const Board &other)
{
    rows = other.rows;
    columns = other.columns;
    init();

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j) {
            if (other.board[i][j] == nullptr)
                board[i][j] = nullptr;
            else
                board[i][j] = new Tile(*(other.board[i][j]));
        }
}

Board::~Board()
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            delete board[i][j];
}

// ─── init helpers ───────────────────────────────────────────────────────────

// sets up the 2D vector with all nullptrs
void Board::init()
{
    board.resize(rows);
    for (int i = 0; i < rows; ++i)
        board[i].resize(columns);

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            board[i][j] = nullptr;
}

// picks a random empty cell on the board
// returns {-1, -1} if the board is full
QVector<int> Board::freePosition()
{
    QVector<int> pos;

    if (full()) {
        pos.append(-1);
        pos.append(-1);
    } else {
        int i, j;
        // just keep trying random spots until we hit an empty one
        do {
            i = rand() % rows;
            j = rand() % columns;
        } while (board[i][j] != nullptr);
        pos.append(i);
        pos.append(j);
    }
    return pos;
}

// ─── state queries ──────────────────────────────────────────────────────────

// checks if this board differs from another one (cell by cell)
bool Board::changed(Board& other) const
{
    if (columns != other.columns || rows != other.rows)
        return false;

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; ++j)
            if (((board[i][j] == nullptr && other.board[i][j] != nullptr) ||
                 (board[i][j] != nullptr && other.board[i][j] == nullptr)) ||
                ((board[i][j] != nullptr && other.board[i][j] != nullptr) &&
                  board[i][j]->getValue() != other.board[i][j]->getValue()))
                return true;
    return false;
}

// returns true if theres no empty cell left
bool Board::full() const
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            if (board[i][j] == nullptr)
                return false;
    return true;
}

bool Board::inbounds(int i, int j)
{
    return (i >= 0 && j >= 0 && i < rows && j < columns);
}

Tile* Board::getTile(int i, int j)
{
    return board[i][j];
}

// ─── board reset ────────────────────────────────────────────────────────────

// clears everything and puts two starting tiles
void Board::reset()
{
    pointsScoredLastRound = 0;
    tileCollisionLastRound = false;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            delete board[i][j];
            board[i][j] = nullptr;
        }
    }

    // place two random tiles to start with
    QVector<int> start = freePosition();
    board[start[0]][start[1]] = new Tile();
    start = freePosition();
    board[start[0]][start[1]] = new Tile();
}

// ─── main movement logic ───────────────────────────────────────────────────

// resets the per-move flags before each move
void Board::prepareForNextMove()
{
    tileCollisionLastRound = false;
    pointsScoredLastRound = 0;

    // clear merge guards so tiles can merge again
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (board[i][j] != nullptr) {
                board[i][j]->setUpgratedThisMove(false);
            }
        }
    }
}

void Board::move(Direction direction)
{
    // take a snapshot so we can check if anything moved
    Board pre_move_board(*this);

    prepareForNextMove();

    // we iterate from the edge tiles are moving towards,
    // otherwise earlier tiles would block later ones
    switch (direction) {
    case UP:
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                moveVertically(i, j, UP);
        break;
    case DOWN:
        for (int i = rows - 1; i >= 0; --i)
            for (int j = 0; j < columns; ++j)
                moveVertically(i, j, DOWN);
        break;
    case LEFT:
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < columns; ++j)
                moveHorizontally(i, j, LEFT);
        break;
    case RIGHT:
        for (int i = 0; i < rows; ++i)
            for (int j = columns - 1; j >= 0; --j)
                moveHorizontally(i, j, RIGHT);
        break;
    }

    // only spawn a new tile if something actually moved
    if (changed(pre_move_board)) {
        QVector<int> newpos = freePosition();
        if (newpos[0] != -1) {
            int chance = rand() % 100;
            // 90% chance for 2, 10% for 4
            board[newpos[0]][newpos[1]] =
                new Tile(chance < PROB_2 ? 2 : 4);
        }
    }

    notifyObservers();
}

// ─── move possibility check ────────────────────────────────────────────────

// checks if theres any valid move left
// if the board isnt full there's always a move possible
bool Board::movePossible() const
{
    if (full()) {
        // try all 4 directions on a copy to see if any changes the board
        Board newBoard(*this);
        newBoard.move(UP);
        if (changed(newBoard)) return true;
        newBoard.move(DOWN);
        if (changed(newBoard)) return true;
        newBoard.move(LEFT);
        if (changed(newBoard)) return true;
        newBoard.move(RIGHT);
        if (changed(newBoard)) return true;

        return false;   // completely stuck
    }
    return true;
}

// ─── horizontal / vertical slide ────────────────────────────────────────────

void Board::moveHorizontally(int i, int j, Direction dir)
{
    if (board[i][j] == nullptr)
        return;

    bool tileCollision = false;

    // step one cell in the direction we're sliding
    int newj = (dir == RIGHT) ? j + 1 : j - 1;

    // keep going while next cell is empty
    while (inbounds(i, newj) && board[i][newj] == nullptr) {
        newj += (dir == RIGHT) ? 1 : -1;
    }

    if (!inbounds(i, newj)) {
        // hit the wall, place at the edge
        board[i][(dir == RIGHT) ? columns - 1 : 0] = board[i][j];
    } else if (board[i][newj]->getValue() == board[i][j]->getValue() &&
               !board[i][newj]->getUpgratedThisMove()) {
        // same value and hasnt merged yet -> merge them
        tileCollision = true;
        handleCollision(i, newj);
    } else {
        // different value or already merged, park next to it
        board[i][newj + ((dir == RIGHT) ? -1 : 1)] = board[i][j];
    }

    // clear the old position if tile actually moved
    if ((dir == RIGHT && newj - 1 != j) ||
        (dir == LEFT  && newj + 1 != j) ||
        tileCollision)
        board[i][j] = nullptr;

    if (tileCollision)
        tileCollisionLastRound = true;
}

void Board::moveVertically(int i, int j, Direction dir)
{
    if (board[i][j] == nullptr)
        return;

    bool tileCollision = false;

    int newi = (dir == UP) ? i - 1 : i + 1;

    while (inbounds(newi, j) && board[newi][j] == nullptr) {
        newi += (dir == UP) ? -1 : 1;
    }

    if (!inbounds(newi, j)) {
        board[(dir == UP) ? 0 : rows - 1][j] = board[i][j];
    } else if (board[newi][j]->getValue() == board[i][j]->getValue() &&
               !board[newi][j]->getUpgratedThisMove()) {
        tileCollision = true;
        handleCollision(newi, j);
    } else {
        board[newi + ((dir == UP) ? 1 : -1)][j] = board[i][j];
    }

    if ((dir == UP   && newi + 1 != i) ||
        (dir == DOWN && newi - 1 != i) ||
        tileCollision)
        board[i][j] = nullptr;

    if (tileCollision)
        tileCollisionLastRound = true;
}

// ─── collision handling ─────────────────────────────────────────────────────

// doubles the tile value and marks it so it cant merge again this turn
void Board::handleCollision(int i, int j)
{
    board[i][j]->upgrade();
    board[i][j]->setUpgratedThisMove(true);
    pointsScoredLastRound += board[i][j]->getValue();
}
