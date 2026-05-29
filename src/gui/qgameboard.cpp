#include "gui/qgameboard.h"
#include "core/board.h"
#include "core/game.h"
#include "core/tile.h"
#include "gui/qresetbutton.h"
#include "gui/qtile.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>

// ════════════════════════════════════════════════════════════════════════════
//  Style helpers
// ════════════════════════════════════════════════════════════════════════════

QString QGameBoard::scoreLabelStyle() {
  // Small rounded box like the classic 2048 UI
  return "QLabel {"
         "  background-color: #bbada0;"
         "  color: #f9f6f2;"
         "  border-radius: 6px;"
         "  padding: 4px 14px;"
         "  font-size: 11pt;"
         "  font-weight: bold;"
         "}";
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
         "QPushButton:hover { background-color: #9f8b77; }"
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
  return "QPushButton {"
         "  background-color: #bbada0;"
         "  color: #eee4da;"
         "  border: none;"
         "  border-radius: 5px;"
         "  padding: 5px 12px;"
         "  font-size: 10pt;"
         "}"
         "QPushButton:hover { background-color: #cbbfb4; }";
}

// ════════════════════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ════════════════════════════════════════════════════════════════════════════

QGameBoard::QGameBoard(QWidget *parent) : QWidget(parent) {
  resize(520, 620);
  setStyleSheet("QGameBoard { background-color: #faf8ef; }");

  currentScoreValue = 0;
  loadBestScore();

  // ── Create game logic ─────────────────────────────────────────────────
  game = new Game(7, 7);
  game->registerObserver(this);

  gui_board.resize(game->getGameBoard()->getRows());
  for (int i = 0; i < game->getGameBoard()->getRows(); ++i)
    gui_board[i].resize(game->getGameBoard()->getColumns(), nullptr);

  // ── Build UI ──────────────────────────────────────────────────────────
  mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(20, 20, 20, 20);
  mainLayout->setSpacing(10);

  // ── Row 1: "2048" title  +  SCORE / BEST boxes ───────────────────────
  {
    QHBoxLayout *row1 = new QHBoxLayout();

    QLabel *title = new QLabel("2048", this);
    title->setStyleSheet("QLabel {"
                         "  color: #776e65;"
                         "  font-size: 42pt;"
                         "  font-weight: 900;"
                         "}");

    // Score box (value label below caption)
    QWidget *scoreBox = new QWidget(this);
    scoreBox->setStyleSheet(
        "QWidget { background-color: #bbada0; border-radius: 6px; }");
    QVBoxLayout *scoreBoxL = new QVBoxLayout(scoreBox);
    scoreBoxL->setContentsMargins(10, 4, 10, 4);
    scoreBoxL->setSpacing(0);
    QLabel *scoreCap = new QLabel("SCORE", scoreBox);
    scoreCap->setStyleSheet("color: #eee4da; font-size: 8pt; font-weight: "
                            "bold; background: transparent;");
    scoreCap->setAlignment(Qt::AlignCenter);
    scorebox = new QLabel("0", scoreBox);
    scorebox->setStyleSheet("color: #f9f6f2; font-size: 14pt; font-weight: "
                            "bold; background: transparent;");
    scorebox->setAlignment(Qt::AlignCenter);
    scoreBoxL->addWidget(scoreCap);
    scoreBoxL->addWidget(scorebox);

    // Best box
    QWidget *bestBox = new QWidget(this);
    bestBox->setStyleSheet(
        "QWidget { background-color: #bbada0; border-radius: 6px; }");
    QVBoxLayout *bestBoxL = new QVBoxLayout(bestBox);
    bestBoxL->setContentsMargins(10, 4, 10, 4);
    bestBoxL->setSpacing(0);
    QLabel *bestCap = new QLabel("BEST", bestBox);
    bestCap->setStyleSheet("color: #eee4da; font-size: 8pt; font-weight: bold; "
                           "background: transparent;");
    bestCap->setAlignment(Qt::AlignCenter);
    bestbox = new QLabel(QString::number(bestScoreValue), bestBox);
    bestbox->setStyleSheet("color: #f9f6f2; font-size: 14pt; font-weight: "
                           "bold; background: transparent;");
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

  // ── Row 2: subtitle + mode buttons ───────────────────────────────────
  {
    QHBoxLayout *row2 = new QHBoxLayout();

    QLabel *sub = new QLabel("Join the tiles, get to 2048!", this);
    sub->setStyleSheet("color: #776e65; font-size: 9pt;");

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

  // ── Row 3: Undo + Restart ─────────────────────────────────────────────
  {
    QHBoxLayout *row3 = new QHBoxLayout();

    btnUndo = new QPushButton("← Undo (U)", this);
    btnRestart = new QPushButton("↺ Restart (R)", this);

    btnUndo->setStyleSheet(actionBtnStyle());
    btnRestart->setStyleSheet(actionBtnStyle());

    connect(btnUndo, &QPushButton::clicked, this, &QGameBoard::undoMove);
    connect(btnRestart, &QPushButton::clicked, this, &QGameBoard::resetGame);

    // Timer label (only visible in Hard mode)
    timerLabel = new QLabel("", this);
    timerLabel->setStyleSheet(
        "color: #f65e3b; font-size: 10pt; font-weight: bold;");
    timerLabel->hide();

    row3->addWidget(btnUndo);
    row3->addWidget(btnRestart);
    row3->addStretch();
    row3->addWidget(timerLabel);
    mainLayout->addLayout(row3);
  }

  // ── Row 4: Game board grid ────────────────────────────────────────────
  {
    // boardLayout is created in drawBoard()
    boardLayout = nullptr;
    drawBoard();
  }

  // ── Hard-mode timer ───────────────────────────────────────────────────
  hardTimer = new QTimer(this);
  hardTimer->setInterval(5000); // 5 seconds
  hardTimer->setSingleShot(false);
  connect(hardTimer, &QTimer::timeout, this, &QGameBoard::hardTimerFired);

  // ── Overlay connections ───────────────────────────────────────────────
  connect(gameOverWindow.getResetBtn(), &QResetButton::clicked, this,
          &QGameBoard::resetGame);
  connect(winWindow.getRestartBtn(), &QResetButton::clicked, this,
          &QGameBoard::resetGame);

  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

QGameBoard::~QGameBoard() { delete game; }

// ════════════════════════════════════════════════════════════════════════════
//  Key events
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::keyPressEvent(QKeyEvent *event) {
  if (game->isGameOver())
    return;

  Direction dir;
  bool validKey = true;

  switch (event->key()) {
  case Qt::Key_Up:
  case Qt::Key_W:
    dir = UP;
    break;
  case Qt::Key_Down:
  case Qt::Key_S:
    dir = DOWN;
    break;
  case Qt::Key_Left:
  case Qt::Key_A:
    dir = LEFT;
    break;
  case Qt::Key_Right:
  case Qt::Key_D:
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

  game->move(dir);

  // Restart hard-mode countdown after every player move
  if (game->getMode() == HARD && !game->isGameOver())
    hardTimer->start();
}

// ════════════════════════════════════════════════════════════════════════════
//  Observer callback
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::notify() {
  currentScoreValue = game->getScore();
  saveBestScore(); // saves only if current > best

  scorebox->setText(QString::number(currentScoreValue));
  bestbox->setText(QString::number(bestScoreValue));

  drawBoard();

  if (game->isGameOver()) {
    hardTimer->stop();
    timerLabel->hide();
    gameOverWindow.show();
    return;
  }

  // Win condition (not in unlimited mode — game.cpp::won() handles that)
  if (game->won()) {
    hardTimer->stop();
    timerLabel->hide();
    winWindow.show();
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  Board drawing
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::drawBoard() {
  if (boardLayout == nullptr) {
    boardLayout = new QGridLayout();
    boardLayout->setSpacing(8);

    // Wrap in a styled container widget
    QWidget *boardContainer = new QWidget(this);
    boardContainer->setStyleSheet("QWidget { background-color: #bbada0; "
                                  "border-radius: 8px; padding: 8px; }");
    boardContainer->setLayout(boardLayout);
    mainLayout->addWidget(boardContainer, 1); // stretch=1 so it fills space
  }

  // Clear existing tiles
  QLayoutItem *child;
  while ((child = boardLayout->takeAt(0)) != nullptr) {
    delete child->widget();
    delete child;
  }

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

// ════════════════════════════════════════════════════════════════════════════
//  Score persistence
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::loadBestScore() {
  QSettings settings;
  bestScoreValue = settings.value("bestScore", 0).toInt();
}

void QGameBoard::saveBestScore() {
  if (currentScoreValue > bestScoreValue) {
    bestScoreValue = currentScoreValue;
    QSettings settings;
    settings.setValue("bestScore", bestScoreValue);
    bestbox->setText(QString::number(bestScoreValue));
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  Slots
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::resetGame() {
  hardTimer->stop();
  timerLabel->hide();
  game->restart();
  drawBoard();
  scorebox->setText("0");
  gameOverWindow.hide();
  winWindow.hide();
  setFocus();

  if (game->getMode() == HARD)
    hardTimer->start();
}

void QGameBoard::undoMove() {
  if (game->isGameOver())
    return;
  game->undo();
  setFocus();
}

void QGameBoard::setNormalMode() {
  hardTimer->stop();
  timerLabel->hide();
  game->setMode(NORMAL);
  updateModeButtons(NORMAL);
  resetGame();
}

void QGameBoard::setUnlimitedMode() {
  hardTimer->stop();
  timerLabel->hide();
  game->setMode(UNLIMITED);
  updateModeButtons(UNLIMITED);
  resetGame();
}

void QGameBoard::setHardMode() {
  game->setMode(HARD);
  updateModeButtons(HARD);
  timerLabel->show();
  timerLabel->setText("Auto-move in 5s");
  resetGame(); // resetGame() will start the timer because mode == HARD
}

void QGameBoard::hardTimerFired() {
  if (game->isGameOver()) {
    hardTimer->stop();
    timerLabel->hide();
    return;
  }
  // Execute a random valid move
  game->randomValidMove();
  timerLabel->setText("Auto-move in 5s");
  // Timer keeps running (not single-shot), countdown restarts implicitly
}

// ════════════════════════════════════════════════════════════════════════════
//  UI helpers
// ════════════════════════════════════════════════════════════════════════════

void QGameBoard::updateModeButtons(GameMode active) {
  btnNormal->setStyleSheet(active == NORMAL ? modeBtnActiveStyle()
                                            : modeBtnInactiveStyle());
  btnUnlimited->setStyleSheet(active == UNLIMITED ? modeBtnActiveStyle()
                                                  : modeBtnInactiveStyle());
  btnHard->setStyleSheet(active == HARD ? modeBtnActiveStyle()
                                        : modeBtnInactiveStyle());
}