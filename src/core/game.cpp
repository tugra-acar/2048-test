// game.cpp - handles move execution, undo, win check, hard mode stuff
#include "core/game.h"
#include "core/tile.h"

#include <cstdlib>

// ─── construction / destruction ─────────────────────────────────────────────

Game::Game(int row, int column)
{
    score  = 0;
    mode   = NORMAL;
    board  = new Board(row, column);
    restart();
}

Game::~Game()
{
    delete board;
    // clean up any remaining undo snapshots
    while (!history.isEmpty())
        delete history.pop().boardCopy;
}

// ─── undo helpers ───────────────────────────────────────────────────────────

// saves a deep copy of the board + score before each move
void Game::pushSnapshot()
{
    Snapshot s;
    s.boardCopy = new Board(*board);
    s.score     = score;
    history.push(s);
}

// ─── core move ──────────────────────────────────────────────────────────────

bool Game::move(Direction dir)
{
    // save state before moving so we can undo later
    pushSnapshot();

    board->move(dir);

    // if board didnt change, the move was invalid - throw away the snapshot
    if (!history.isEmpty() && !history.top().boardCopy->changed(*board)) {
        Snapshot s = history.pop();
        delete s.boardCopy;
        return false;
    }

    // add up score from any merges
    if (board->isTileCollisionLastRound())
        score += board->getPointsScoredLastRound();

    // check if the player is stuck
    if (!board->movePossible())
        gameOver = true;

    notifyObservers();
    return true;
}

// ─── undo ───────────────────────────────────────────────────────────────────

bool Game::undo()
{
    if (history.isEmpty())
        return false;

    Snapshot s = history.pop();

    delete board;
    board     = s.boardCopy;
    score     = s.score;
    gameOver  = false;   // if we're undoing, game continues

    notifyObservers();
    return true;
}

// ─── win detection ──────────────────────────────────────────────────────────

bool Game::won() const
{
    // unlimited mode has no win condition
    if (mode == UNLIMITED)
        return false;

    for (int i = 0; i < board->getRows(); ++i)
        for (int j = 0; j < board->getColumns(); ++j)
            if (board->getTile(i, j) != nullptr &&
                board->getTile(i, j)->getValue() == WINNING_VALUE)
                return true;
    return false;
}

// ─── hard mode: random move ─────────────────────────────────────────────────

// tries all 4 directions in random order, plays the first valid one
int Game::randomValidMove()
{
    Direction dirs[4] = { UP, DOWN, LEFT, RIGHT };

    // shuffle the directions randomly (fisher-yates)
    for (int i = 3; i > 0; --i) {
        int j = rand() % (i + 1);
        Direction tmp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = tmp;
    }

    // try each one on a copy to see if it actually does something
    for (int k = 0; k < 4; ++k) {
        Board copy(*board);
        copy.move(dirs[k]);
        if (copy.changed(*board)) {
            move(dirs[k]);
            return dirs[k];
        }
    }

    return -1;   // no valid move left
}

// ─── restart ────────────────────────────────────────────────────────────────

void Game::restart()
{
    // get rid of all saved undo states
    while (!history.isEmpty())
        delete history.pop().boardCopy;

    board->reset();
    gameOver = false;
    score    = 0;
}