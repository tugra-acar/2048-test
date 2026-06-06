// qgameboard.cpp - sets up all the UI components and links them to game logic
#include "gui/qgameboard.h"
#include "core/board.h"
#include "core/game.h"
#include "gui/qresetbutton.h"
#include "gui/qtile.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>

// ─── shared colors ──────────────────────────────────────────────────────────

static const QString BG_MUTED = "#bbada0";
static const QString FG_LIGHT = "#f9f6f2";
static const QString FG_CAPTION = "#eee4da";
static const QString FG_DARK = "#776e65";

// ─── styles ─────────────────────────────────────────────────────────────────

QString QGameBoard::scoreLabelStyle() {
  return QString("QLabel {"
                 "  background-color: %1;"
                 "  color: %2;"
                 "  border-radius: 6px;"
                 "  padding: 4px 14px;"
                 "  font-size: 11pt;"
                 "  font-weight: bold;"
                 "}")
      .arg(BG_MUTED, FG_LIGHT);
}

QString QGameBoard::actionBtnStyle() {
  return "QPushButton {"
         "  background-color: #8f7a66;"
         "  color: #f9f6f2;"
         "  border: none;"
         "  border-radius: 6px;"
         "  padding: 6px 16px;"
         "  font-size: 10pt;"
         "  font-weight: bold;"
         "}"
         "QPushButton:hover  { background-color: #9f8b77; }"
         "QPushButton:pressed { background-color: #7a6655; }";
}

QString QGameBoard::modeBtnActiveStyle() {
  return "QPushButton {"
         "  background-color: #8f7a66;"
         "  color: #f9f6f2;"
         "  border: none;"
         "  border-radius: 5px;"
         "  padding: 5px 12px;"
         "  font-size: 10pt;"
         "  font-weight: bold;"
         "}";
}

QString QGameBoard::modeBtnInactiveStyle() {
  return QString("QPushButton {"
                 "  background-color: %1;"
                 "  color: %2;"
                 "  border: none;"
                 "  border-radius: 5px;"
                 "  padding: 5px 12px;"
                 "  font-size: 10pt;"
                 "}"
                 "QPushButton:hover { background-color: #cbbfb4; }")
      .arg(BG_MUTED, FG_CAPTION);
}

// ─── setup ──────────────────────────────────────────────────────────────────

QGameBoard::QGameBoard(QWidget *parent)
    : QWidget(parent), gameOverWindow(this), winWindow(this) {
  resize(520, 620);
  setStyleSheet("QGameBoard { background-color: #faf8ef; }");

  currentScoreValue = 0;
  loadBestScore(); // get the high score from saved settings

  // create the actual game instance (4x4 board)
  int M = 4;
  int N = 4;
  game = new Game(M, N);
  game->registerObserver(this); // so we get told when things change

  // setup our gui grid to match
  gui_board.resize(game->getGameBoard()->getRows());
  for (int i = 0; i < game->getGameBoard()->getRows(); ++i)
    gui_board[i].resize(game->getGameBoard()->getColumns(), nullptr);

  // ─── layout building ──────────────────────────────────────────────────────

  mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(20, 20, 20, 20);
  mainLayout->setSpacing(10);

  // row 1: title + scores
  {
    QHBoxLayout *row1 = new QHBoxLayout();

    QLabel *title = new QLabel("2048", this);
    title->setStyleSheet(QString("QLabel {"
                                 "  color: %1;"
                                 "  font-size: 42pt;"
                                 "  font-weight: 900;"
                                 "}")
                             .arg(FG_DARK));

    // score display
    QWidget *scoreBox = new QWidget(this);
    scoreBox->setStyleSheet(
        QString("QWidget { background-color: %1; border-radius: 6px; }")
            .arg(BG_MUTED));
    QVBoxLayout *scoreBoxL = new QVBoxLayout(scoreBox);
    scoreBoxL->setContentsMargins(10, 4, 10, 4);
    scoreBoxL->setSpacing(0);

    QLabel *scoreCap = new QLabel("SCORE", scoreBox);
    scoreCap->setStyleSheet(
        QString("color: %1; font-size: 8pt; font-weight: bold;"
                " background: transparent;")
            .arg(FG_CAPTION));
    scoreCap->setAlignment(Qt::AlignCenter);

    scorebox = new QLabel("0", scoreBox);
    scorebox->setStyleSheet(
        QString("color: %1; font-size: 14pt; font-weight: bold;"
                " background: transparent;")
            .arg(FG_LIGHT));
    scorebox->setAlignment(Qt::AlignCenter);

    scoreBoxL->addWidget(scoreCap);
    scoreBoxL->addWidget(scorebox);

    // best score display
    QWidget *bestBox = new QWidget(this);
    bestBox->setStyleSheet(
        QString("QWidget { background-color: %1; border-radius: 6px; }")
            .arg(BG_MUTED));
    QVBoxLayout *bestBoxL = new QVBoxLayout(bestBox);
    bestBoxL->setContentsMargins(10, 4, 10, 4);
    bestBoxL->setSpacing(0);

    QLabel *bestCap = new QLabel("BEST", bestBox);
    bestCap->setStyleSheet(
        QString("color: %1; font-size: 8pt; font-weight: bold;"
                " background: transparent;")
            .arg(FG_CAPTION));
    bestCap->setAlignment(Qt::AlignCenter);

    bestbox = new QLabel(QString::number(bestScoreValue), bestBox);
    bestbox->setStyleSheet(
        QString("color: %1; font-size: 14pt; font-weight: bold;"
                " background: transparent;")
            .arg(FG_LIGHT));
    bestbox->setAlignment(Qt::AlignCenter);

    bestBoxL->addWidget(bestCap);
    bestBoxL->addWidget(bestbox);

    row1->addWidget(title);
    row1->addStretch();
    row1->addWidget(scoreBox);
    row1->addSpacing(6);
    row1->addWidget(bestBox);
    mainLayout->addLayout(row1);
  }

  // row 2: subtitle + mode selectors
  {
    QHBoxLayout *row2 = new QHBoxLayout();

    QLabel *sub = new QLabel("Join the tiles, get to 2048!", this);
    sub->setStyleSheet(QString("color: %1; font-size: 9pt;").arg(FG_DARK));

    btnNormal = new QPushButton("Normal", this);
    btnUnlimited = new QPushButton("Unlimited", this);
    btnHard = new QPushButton("Hard", this);

    connect(btnNormal, &QPushButton::clicked, this, &QGameBoard::setNormalMode);
    connect(btnUnlimited, &QPushButton::clicked, this,
            &QGameBoard::setUnlimitedMode);
    connect(btnHard, &QPushButton::clicked, this, &QGameBoard::setHardMode);

    updateModeButtons(NORMAL);

    row2->addWidget(sub);
    row2->addStretch();
    row2->addWidget(btnNormal);
    row2->addWidget(btnUnlimited);
    row2->addWidget(btnHard);
    mainLayout->addLayout(row2);
  }

  // row 3: undo / restart / timer warning
  {
    QHBoxLayout *row3 = new QHBoxLayout();

    btnUndo = new QPushButton("← Undo (U)", this);
    btnRestart = new QPushButton("↺ Restart (R)", this);

    btnUndo->setStyleSheet(actionBtnStyle());
    btnRestart->setStyleSheet(actionBtnStyle());

    connect(btnUndo, &QPushButton::clicked, this, &QGameBoard::undoMove);
    connect(btnRestart, &QPushButton::clicked, this, &QGameBoard::resetGame);

    timerLabel = new QLabel("", this);
    timerLabel->setStyleSheet(
        "color: #f65e3b; font-size: 10pt; font-weight: bold;");
    timerLabel->hide(); // only show in hard mode

    row3->addWidget(btnUndo);
    row3->addWidget(btnRestart);
    row3->addStretch();
    row3->addWidget(timerLabel);
    mainLayout->addLayout(row3);
  }

  // row 4: the grid itself
  {
    boardLayout = nullptr;
    drawBoard();
  }

  // setup hard mode timer (5 secs)
  hardTimer = new QTimer(this);
  hardTimer->setInterval(5000);
  hardTimer->setSingleShot(false);
  connect(hardTimer, &QTimer::timeout, this, &QGameBoard::hardTimerFired);

  // hook up the dialog buttons
  connect(gameOverWindow.getResetBtn(), &QResetButton::clicked, this, [this]() {
    gameOverWindow.close();
    resetGame();
  });
  connect(winWindow.getRestartBtn(), &QResetButton::clicked, this, [this]() {
    winWindow.close();
    resetGame();
  });
  
  // Ensure we get keyboard focus back when dialogs are closed via 'X'
  connect(&gameOverWindow, &QDialog::finished, this, [this]() { setFocus(); });
  connect(&winWindow, &QDialog::finished, this, [this]() { setFocus(); });

  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

QGameBoard::~QGameBoard() { delete game; }

// ─── input ──────────────────────────────────────────────────────────────────

void QGameBoard::keyPressEvent(QKeyEvent *event) {
  if (game->isGameOver())
    return;

  Direction dir;
  bool validKey = true;

  // mapping arrow keys to movements (wasd removed)
  switch (event->key()) {
  case Qt::Key_Up:
    dir = UP;
    break;
  case Qt::Key_Down:
    dir = DOWN;
    break;
  case Qt::Key_Left:
    dir = LEFT;
    break;
  case Qt::Key_Right:
    dir = RIGHT;
    break;
  case Qt::Key_U:
    undoMove();
    return;
  case Qt::Key_R:
    resetGame();
    return;
  default:
    validKey = false;
    break;
  }

  if (!validKey)
    return;

  // Block movement if player won in normal mode (they must switch to unlimited
  // to continue)
  if (game->won() && game->getMode() != UNLIMITED)
    return;

  bool move_was_valid = game->move(dir);

  // if playing hard mode, reset the 5s timer after a valid move
  if (move_was_valid && game->getMode() == HARD && !game->isGameOver())
    hardTimer->start();
}
// ─── observer update ────────────────────────────────────────────────────────

void QGameBoard::notify() {
  currentScoreValue = game->getScore();
  saveBestScore(); // will update if we beat it

  scorebox->setText(QString::number(currentScoreValue));
  bestbox->setText(QString::number(bestScoreValue));

  drawBoard(); // redraw tiles

  if (game->isGameOver()) {
    hardTimer->stop();
    timerLabel->hide();
    gameOverWindow.show();
    return;
  }

  if (game->won() && !winMessageShown) {
    winMessageShown = true;
    hardTimer->stop();
    timerLabel->hide();
    winWindow.show();
  }
}

// ─── drawing ────────────────────────────────────────────────────────────────

void QGameBoard::drawBoard() {
  // make the layout if it doesnt exist yet
  if (boardLayout == nullptr) {
    boardLayout = new QGridLayout();
    boardLayout->setSpacing(8);

    QWidget *boardContainer = new QWidget(this);
    boardContainer->setStyleSheet(
        QString("QWidget { background-color: %1;"
                " border-radius: 8px; padding: 8px; }")
            .arg(BG_MUTED));
    boardContainer->setLayout(boardLayout);
    mainLayout->addWidget(boardContainer, 1);
  }

  // clear old tile widgets
  QLayoutItem *child;
  while ((child = boardLayout->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }

  // create new tile widgets based on current board state
  int rows = game->getGameBoard()->getRows();
  int cols = game->getGameBoard()->getColumns();

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      gui_board[i][j] = new QTile(game->getGameBoard()->getTile(i, j));
      gui_board[i][j]->setMinimumSize(90, 90);
      boardLayout->addWidget(gui_board[i][j], i, j);
      gui_board[i][j]->draw();
    }
  }
}

// ─── saving/loading scores ──────────────────────────────────────────────────

// tracked only in memory for the current session
void QGameBoard::loadBestScore() { bestScoreValue = 0; }

void QGameBoard::saveBestScore() {
  if (currentScoreValue > bestScoreValue) {
    bestScoreValue = currentScoreValue;
    bestbox->setText(QString::number(bestScoreValue));
  }
}

// ─── actions & modes ────────────────────────────────────────────────────────

void QGameBoard::resetGame() {
  hardTimer->stop();
  timerLabel->hide();
  game->restart();
  drawBoard();
  scorebox->setText("0");
  gameOverWindow.hide();
  winWindow.hide();
  winMessageShown = false;
  setFocus(); // grab focus back so keyboard works

  if (game->getMode() == HARD)
    hardTimer->start();
}

void QGameBoard::undoMove() {
  if (game->isGameOver())
    return;
  game->undo();
  
  // If we undid the move that won the game, reset the flag so we can experience the win again!
  if (!game->won()) {
    winMessageShown = false;
  }
  
  setFocus();
}

void QGameBoard::setNormalMode() {
  game->setMode(NORMAL);
  updateModeButtons(NORMAL);
  hardTimer->stop();
  timerLabel->hide();
  winWindow.hide();
  gameOverWindow.hide();
  game->notifyObservers(); // re-evaluates win/loss for the new mode
  setFocus();
}

void QGameBoard::setUnlimitedMode() {
  game->setMode(UNLIMITED);
  updateModeButtons(UNLIMITED);
  hardTimer->stop();
  timerLabel->hide();
  winWindow.hide();
  gameOverWindow.hide();
  game->notifyObservers();
  setFocus();
}

void QGameBoard::setHardMode() {
  game->setMode(HARD);
  updateModeButtons(HARD);
  timerLabel->show();
  timerLabel->setText("Auto-move in 5s");
  winWindow.hide();
  gameOverWindow.hide();

  if (!game->isGameOver() && !game->won()) {
    hardTimer->start();
  } else {
    hardTimer->stop();
  }

  game->notifyObservers();
  setFocus();
}

void QGameBoard::hardTimerFired() {
  if (game->isGameOver()) {
    hardTimer->stop();
    timerLabel->hide();
    return;
  }

  // player was too slow, force a random move
  game->randomValidMove();
  timerLabel->setText("Auto-move in 5s");
}

void QGameBoard::updateModeButtons(GameMode active) {
  btnNormal->setStyleSheet(active == NORMAL ? modeBtnActiveStyle()
                                            : modeBtnInactiveStyle());
  btnUnlimited->setStyleSheet(active == UNLIMITED ? modeBtnActiveStyle()
                                                  : modeBtnInactiveStyle());
  btnHard->setStyleSheet(active == HARD ? modeBtnActiveStyle()
                                        : modeBtnInactiveStyle());
}