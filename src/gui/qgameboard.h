// qgameboard.h - the main GUI class that brings everything together
// handles drawing the grid, scoring, buttons, and user input
#ifndef QGAMEBOARD_H
#define QGAMEBOARD_H

#include "core/game.h"
#include "core/observer.h"
#include "gui/qgameoverwindow.h"
#include "gui/qwinwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVector>
#include <QWidget>

class QKeyEvent;
class QTile;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;

class QGameBoard : public QWidget, public Observer {
  Q_OBJECT
public:
  explicit QGameBoard(QWidget *parent = nullptr);
  ~QGameBoard();

  // called whenever the board changes so we can update UI
  void notify() override;

protected:
  // catches wasd/arrows and sends to game
  void keyPressEvent(QKeyEvent *event) override;

private:
  Game *game;
  QVector<QVector<QTile *>> gui_board; // keeps track of the visual tiles

  // layouts for arranging the window
  QVBoxLayout *mainLayout;
  QGridLayout *boardLayout;

  // score stuff
  QLabel *scorebox;
  QLabel *bestbox;
  int currentScoreValue;
  int bestScoreValue;

  // game mode selection
  QPushButton *btnNormal;
  QPushButton *btnUnlimited;
  QPushButton *btnHard;

  // undo/restart
  QPushButton *btnUndo;
  QPushButton *btnRestart;

  // for hard mode
  QTimer *hardTimer;
  QLabel *timerLabel;

  // win/loss screens
  QGameOverWindow gameOverWindow;
  QWinWindow winWindow;
  bool winMessageShown = false;

  void drawBoard();
  void loadBestScore();
  void saveBestScore();
  void updateModeButtons(GameMode active);

  // common styling to avoid copy pasting massive strings everywhere
  static QString scoreLabelStyle();
  static QString actionBtnStyle();
  static QString modeBtnActiveStyle();
  static QString modeBtnInactiveStyle();

private slots:
  // what happens when buttons are clicked or timer fires
  void resetGame();
  void undoMove();
  void setNormalMode();
  void setUnlimitedMode();
  void setHardMode();
  void hardTimerFired();
};

#endif // QGAMEBOARD_H
