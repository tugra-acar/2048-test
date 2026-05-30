// qtile.h - visual representation of a tile on the board
// basically a QLabel that colors itself based on the tile value
#ifndef QTILE_H
#define QTILE_H

#include <QLabel>

class Tile;

class QTile : public QLabel
{
    Q_OBJECT
public:
    QTile(Tile* tile);   // tile can be nullptr for empty cells
    void draw();         // sets the right color/font based on value

private:
    Tile* tile;

    // helper to build stylesheet strings without repeating ourselves
    static QString tileStyle(const QString& bg, const QString& fg, int pt = 40);

    // adds a yellow glow effect (for high value tiles)
    void applyGlow(int radius);
};

#endif // QTILE_H
