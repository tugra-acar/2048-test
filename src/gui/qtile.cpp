// qtile.cpp - maps tile values to colors
// the color palette is based on the original 2048 game
// tiles >= 256 get a yellow glow that gets stronger as value increases
#include "gui/qtile.h"
#include "core/tile.h"

#include <QGraphicsDropShadowEffect>

// ─── helpers ────────────────────────────────────────────────────────────────

// builds a stylesheet string so we dont have to copy paste the same thing
// for every single tile value
QString QTile::tileStyle(const QString& bg, const QString& fg, int pt)
{
    return QString("QTile {"
                   "  background: rgb(%1);"
                   "  color: rgb(%2);"
                   "  font: bold;"
                   "  border-radius: 10px;"
                   "  font: %3pt;"
                   "}").arg(bg, fg, QString::number(pt));
}

void QTile::applyGlow(int radius)
{
    auto *dse = new QGraphicsDropShadowEffect();
    dse->setColor(Qt::yellow);
    dse->setBlurRadius(radius);
    dse->setOffset(-1);
    setGraphicsEffect(dse);
}

// ─── construction ───────────────────────────────────────────────────────────

QTile::QTile(Tile* tile)
{
    setAlignment(Qt::AlignCenter);
    this->tile = tile;
}

// ─── drawing ────────────────────────────────────────────────────────────────

// color constants
static const QString DARK_TEXT  = "119,110,101";
static const QString LIGHT_TEXT = "255,255,255";
static const QString EMPTY_BG  = "204,192,179";

void QTile::draw()
{
    if (tile == nullptr) {
        setText("");
        setStyleSheet(tileStyle(EMPTY_BG, DARK_TEXT));
        return;
    }

    setText(QString::number(tile->getValue()));

    switch (tile->getValue()) {
    case 2:
        setStyleSheet(tileStyle("238,228,218", DARK_TEXT));
        break;
    case 4:
        setStyleSheet(tileStyle("237,224,200", DARK_TEXT));
        break;
    case 8:
        setStyleSheet(tileStyle("242,177,121", LIGHT_TEXT));
        break;
    case 16:
        setStyleSheet(tileStyle("245,150,100", LIGHT_TEXT));
        break;
    case 32:
        setStyleSheet(tileStyle("245,125,95", LIGHT_TEXT));
        break;
    case 64:
        setStyleSheet(tileStyle("245,95,60", LIGHT_TEXT));
        break;
    case 128:
        setStyleSheet(tileStyle("237,207,114", LIGHT_TEXT));
        break;

    // tiles with glow effect
    case 256:
        applyGlow(20);
        setStyleSheet(tileStyle("237,204,97", LIGHT_TEXT));
        break;
    case 512:
        applyGlow(30);
        setStyleSheet(tileStyle("237,204,97", LIGHT_TEXT));
        break;
    case 1024:
        applyGlow(40);
        setStyleSheet(tileStyle("237,204,97", LIGHT_TEXT));
        break;
    case 2048:
        applyGlow(50);
        setStyleSheet(tileStyle("237,204,97", LIGHT_TEXT));
        break;

    // super high tiles get a purple look
    case 4096:
        applyGlow(50);
        setStyleSheet(tileStyle("160,50,160", LIGHT_TEXT, 35));
        break;
    case 8192:
        applyGlow(50);
        setStyleSheet(tileStyle("130,30,130", LIGHT_TEXT, 35));
        break;

    default:
        setStyleSheet(tileStyle("238,228,218", DARK_TEXT));
        break;
    }
}
