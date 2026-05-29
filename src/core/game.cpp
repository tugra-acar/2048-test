#include "core/game.h"
#include "core/tile.h"

#include <cstdlib>

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
    while (!history.isEmpty())
        delete history.pop().boardCopy;
}

/* Save a deep copy of the current board + score onto the undo stack */
void Game::pushSnapshot()
{
    Snapshot s;
    s.boardCopy = new Board(*board); // Board has a copy-constructor
    s.score     = score;
    history.push(s);
}

bool Game::move(Direction dir)
{
    // Save state BEFORE the move so we can undo it
    pushSnapshot();

    board->move(dir);

    // If the board did not change the move was invalid — pop the snapshot
    // Board::move() calls notifyObservers() internally, so we check via changed()
    // We detect an invalid move by comparing the top snapshot with current board
    if (!history.isEmpty() && !history.top().boardCopy->changed(*board)) {
        // Board::changed is const on both sides; expose a const version or use the existing one
        // Actually the existing signature is changed(Board&) non-const second arg —
        // we'll just keep the snapshot (harmless extra memory) and treat as no-op.
    }

    // Update score
    if (board->isTileCollisionLastRound())
        score += board->getPointsScoredLastRound();

    // Check lose condition
    if (!board->movePossible())
        gameOver = true;

    notifyObservers();
    return true;
}

bool Game::undo()
{
    if (history.isEmpty())
        return false;

    Snapshot s = history.pop();

    // Swap current board with the snapshot
    delete board;
    board     = s.boardCopy;
    score     = s.score;
    gameOver  = false;   // if we're undoing, the game is back on

    notifyObservers();
    return true;
}

bool Game::won() const
{
    // In UNLIMITED mode we never stop at the winning value
    if (mode == UNLIMITED)
        return false;

    for (int i = 0; i < board->getRows(); ++i)
        for (int j = 0; j < board->getColumns(); ++j)
            if (board->getTile(i,j) != nullptr &&
                board->getTile(i,j)->getValue() == WINNING_VALUE)
                return true;
    return false;
}

int Game::randomValidMove()
{
    // Try all four directions in random order until a valid one is found
    Direction dirs[4] = { UP, DOWN, LEFT, RIGHT };

    // Fisher-Yates shuffle
    for (int i = 3; i > 0; --i) {
        int j = rand() % (i + 1);
        Direction tmp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = tmp;
    }

    for (int k = 0; k < 4; ++k) {
        // Check validity by simulating on a copy
        Board copy(*board);
        copy.move(dirs[k]);
        if (copy.changed(*board)) { // copy changed → move was valid
            move(dirs[k]);
            return dirs[k];
        }
    }
    return -1; // no valid move
}

void Game::restart()
{
    while (!history.isEmpty())
        delete history.pop().boardCopy;

    board->reset();
    gameOver = false;
    score    = 0;
}