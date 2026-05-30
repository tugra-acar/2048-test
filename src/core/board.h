// board.h - the game grid that holds all the tiles
#ifndef BOARD_H
#define BOARD_H

#include "core/subject.h"

#include <QVector>
#include <QString>

class Tile;

// directions for tile movement
enum Direction { UP, DOWN, LEFT, RIGHT };

// Board stores the NxM grid and handles all the sliding/merging logic
class Board : public Subject
{
public:
    Board(int rows, int columns);
    Board(const Board& other);   // deep copy (needed for undo snapshots)
    ~Board();

    void reset();                // clears board and places 2 random tiles
    Tile* getTile(int i, int j); // returns tile at (i,j), nullptr if empty

    int getRows() const { return rows; }
    int getColumns() const { return columns; }

    // slides tiles in the given direction, merges matching ones,
    // and spawns a new tile if the board actually changed
    void move(Direction direction);

    bool full() const;          // true if every cell is occupied
    bool movePossible() const;  // false only when completely stuck

    // returns points earned from merges in last move
    int getPointsScoredLastRound() const { return pointsScoredLastRound; }

    // did any tiles merge last move?
    bool isTileCollisionLastRound() const { return tileCollisionLastRound; }

    // compares two boards cell by cell
    bool changed(Board& other) const;

private:
    QVector<QVector<Tile*> > board;   // the actual 2D grid
    int rows;
    int columns;

    // probability of spawning a 2 vs 4 (out of 100)
    static constexpr int PROB_2 = 90;
    static constexpr int PROB_4 = 10;

    void init();                           // allocates the grid with nullptrs
    QVector<int> freePosition();           // picks a random empty cell
    bool inbounds(int i, int j);           // bounds check
    void moveHorizontally(int i, int j, Direction dir);
    void moveVertically(int i, int j, Direction dir);
    void handleCollision(int i, int j);    // merges two tiles
    void prepareForNextMove();             // resets per-move flags

    int  pointsScoredLastRound;
    bool tileCollisionLastRound;
};

#endif