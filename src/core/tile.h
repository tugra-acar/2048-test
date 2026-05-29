// Tile.h
#ifndef TILE_H
#define TILE_H

class Tile
{
public:
    Tile(int v = 2) : value(v), upgradedThisMove(false) {}

    int getValue() const { return value; }
    void upgrade() { value *= 2; }

    void setUpgratedThisMove(bool updated) { upgradedThisMove = updated; }
    bool getUpgratedThisMove() const { return upgradedThisMove; }

private:
    int value;
    bool upgradedThisMove;
};
#endif