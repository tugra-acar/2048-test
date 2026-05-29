#ifndef QGAMEBOARD_H
#define QGAMEBOARD_H

#include "core/observer.h"
#include "core/game.h"
#include "gui/qgameoverwindow.h"
#include "gui/qwinwindow.h"

#include <QVector>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

class QKeyEvent;
class QTile;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;

class QGameBoard : public QWidget, public Observer
{
    Q_OBJECT
public:
    explicit QGameBoard(QWidget *parent = nullptr);
    ~QGameBoard();

    void notify();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    // ── Core ────────────────────────────────────────────────────────────────
    Game*  game;
    QVector<QVector<QTile*>> gui_board;

    // ── Layouts ─────────────────────────────────────────────────────────────
    QVBoxLayout* mainLayout;
    QGridLayout* boardLayout;

    // ── Score labels ────────────────────────────────────────────────────────
    QLabel* scorebox;       // shows current score (styled box)
    QLabel* bestbox;        // shows best score   (styled box)
    int     currentScoreValue;
    int     bestScoreValue;

    // ── Mode buttons ────────────────────────────────────────────────────────
    QPushButton* btnNormal;
    QPushButton* btnUnlimited;
    QPushButton* btnHard;

    // ── Action buttons ──────────────────────────────────────────────────────
    QPushButton* btnUndo;
    QPushButton* btnRestart;

    // ── Hard-mode timer ─────────────────────────────────────────────────────
    QTimer* hardTimer;           // fires every 5 s if player hasn't moved
    QLabel* timerLabel;          // optional countdown display

    // ── Overlays ────────────────────────────────────────────────────────────
    QGameOverWindow gameOverWindow;
    QWinWindow      winWindow;

    // ── Helpers ─────────────────────────────────────────────────────────────
    void drawBoard();
    void loadBestScore();
    void saveBestScore();
    void updateModeButtons(GameMode active);
    void setGameInputEnabled(bool enabled);

    // Style helpers
    static QString scoreLabelStyle();
    static QString actionBtnStyle();
    static QString modeBtnActiveStyle();
    static QString modeBtnInactiveStyle();

private slots:
    void resetGame();
    void undoMove();
    void setNormalMode();
    void setUnlimitedMode();
    void setHardMode();
    void hardTimerFired();
};

#endif // QGAMEBOARD_H
