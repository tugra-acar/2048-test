#ifndef GAME_H
#define GAME_H

#define WINNING_VALUE 2048

#include "core/subject.h"
#include "core/board.h"

#include <QStack>

class Board;

enum GameMode { NORMAL, UNLIMITED, HARD };

class Game : public Subject
{
public:
    Game(int row, int column);
    ~Game();

    void restart();
    bool isGameOver() const { return gameOver; }
    Board* getGameBoard() const { return board; }

    // Move in a direction; returns false if the move was invalid (board didn't change)
    bool move(Direction dir);

    // Undo last move; returns false if nothing to undo
    bool undo();

    bool won() const;
    int getScore() const { return score; }

    // Game mode
    void setMode(GameMode m) { mode = m; }
    GameMode getMode() const { return mode; }

    // Execute a random valid move (used by Hard mode timer).
    // Returns the direction chosen, or -1 if no valid move exists.
    int randomValidMove();

private:
    Board* board;
    bool gameOver;
    int score;
    GameMode mode;

    // Undo history: each entry is (boardSnapshot, scoreSnapshot)
    struct Snapshot {
        Board* boardCopy;
        int    score;
    };
    QStack<Snapshot> history;

    void pushSnapshot();
};

#endif // GAME_H