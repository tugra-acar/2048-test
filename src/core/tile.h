// tile.h - represents a single tile on the board
#ifndef TILE_H
#define TILE_H

class Tile
{
public:
    // default value is 2, like in the original game
    Tile(int v = 2) : value(v), upgradedThisMove(false) {}

    int getValue() const { return value; }
    void upgrade() { value *= 2; }   // doubles the tile value on merge

    // this flag prevents a tile from merging twice in one move
    void setUpgratedThisMove(bool updated) { upgradedThisMove = updated; }
    bool getUpgratedThisMove() const { return upgradedThisMove; }

private:
    int value;
    bool upgradedThisMove;   // true if already merged this turn
};
#endif