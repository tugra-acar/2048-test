#ifndef BOARD_H
#define BOARD_H

// Subject class used for observer pattern
#include "core/subject.h"

#include <QVector>
#include <QString>

class Tile;

// Directions used for tile movement
enum Direction { UP, DOWN, LEFT, RIGHT };


// Board class stores and manages the game state
class Board : public Subject
{
public:
    // Constructor creates a board with given size
    Board(int rows, int columns);
    // Copy constructor for deep copying the board
    Board(const Board& other);
    // Destructor releases dynamically allocated memory
    ~Board();
    // Resets the board to initial state
    void reset();
    // Returns tile pointer at given position
    Tile* getTile(int i, int j);
    // Returns row count
    int getRows() const { return rows; }
    // Returns column count
    int getColumns() const { return columns; }
    // Moves tiles in given direction
    void move(Direction direction);
    // Checks is the board full
    bool full() const;
    // Returns points gained in last move
    int getPointsScoredLastRound() const
    {
        return pointsScoredLastRound;
    }
    // Returns is a merge happened in last move
    bool isTileCollisionLastRound() const
    {
        return tileCollisionLastRound;
    }
    // Checks are any valid move exists
    bool movePossible() const;
    // Compares current board with another board
    bool changed(Board& other) const;

private:
    // 2D game board storing tile pointers
    QVector<QVector<Tile*> > board;
    // Board dimensions
    int rows;
    int columns;
    // Probability values for spawning tiles
    const int PROB_2 = 90;
    const int PROB_4 = 10;
    // Initializes board structure
    void init();
    // Finds a free position on board
    QVector<int> freePosition();
    // Checks whether indices are inside board limits
    bool inbounds(int i, int j);
    // Handles horizontal movement
    void moveHorizontally(int i, int j, Direction dir);
    // Handles vertical movement
    void moveVertically(int i, int j, Direction dir);
    // Merges tiles if collision is possible
    void handleCollision(int i, int j);
    // Prepares tiles for next move
    void prepareForNextMove();
    // Stores score gained in last move
    int pointsScoredLastRound;
    // Stores whether a collision happened in last move
    bool tileCollisionLastRound;
};


#endif