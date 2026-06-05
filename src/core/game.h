// game.h - top level game controller
// wraps the Board with score, undo, modes, win/lose logic etc
#ifndef GAME_H
#define GAME_H

#define WINNING_VALUE 2048

#include "core/board.h"
#include "core/subject.h"

#include <QStack>

class Board;

// the three game modes we support
enum GameMode {
  NORMAL,    // classic 2048 rules
  UNLIMITED, // no win condition, keep going past 2048
  HARD       // a random move fires every 5s if you dont play
};

class Game : public Subject {
public:
  Game(int row, int column);
  ~Game();

  void restart();
  bool isGameOver() const { return gameOver; }
  Board *getGameBoard() const { return board; }

  // tries to move in the given direction
  // returns false if nothing moved (invalid move)
  bool move(Direction dir);

  // goes back to previous state, returns false if nothing to undo
  bool undo();

  // checks if any tile reached 2048 (always false in unlimited mode)
  bool won() const;

  // checks if any tile reached 2048 regardless of mode (used for unlimited banner)
  bool hasReachedWinningValue() const;

  int getScore() const { return score; }

  void setMode(GameMode m) { mode = m; }
  GameMode getMode() const { return mode; }

  // picks a random valid direction and plays it (for hard mode timer)
  // returns the direction used, or -1 if no valid move
  int randomValidMove();

private:
  Board *board;
  bool gameOver;
  int score;
  GameMode mode;

  // undo history - stores board snapshot + score at each move
  struct Snapshot {
    Board *boardCopy;
    int score;
  };
  QStack<Snapshot> history;

  void pushSnapshot(); // saves current state for undo
};

#endif // GAME_H