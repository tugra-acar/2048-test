// qgameoverwindow.cpp - game over screen UI
#include "gui/qgameoverwindow.h"
#include "gui/qresetbutton.h"

#include <QLabel>
#include <QVBoxLayout>

QGameOverWindow::QGameOverWindow(QWidget *parent) : QWidget(parent) {
  // Make sure the background stylesheet actually renders for this custom
  // QWidget
  setAttribute(Qt::WA_StyledBackground, true);

  // light background for game over overlay (added alpha to look like an
  // overlay)
  setStyleSheet("QGameOverWindow { background: rgba(237, 224, 200, 220); }");
  setFixedSize(425, 205);

  QVBoxLayout *layout = new QVBoxLayout(this);

  // main text
  QLabel *gameover = new QLabel("Game Over!", this);
  gameover->setStyleSheet("QLabel {"
                          "  color: rgb(119,110,101);"
                          "  font-size: 40pt;"
                          "  font-weight: bold;"
                          "}");
  gameover->setAlignment(Qt::AlignCenter);

  // the try again button
  reset = new QResetButton(this);
  reset->setFixedHeight(50);
  reset->setFixedWidth(100);

  layout->addWidget(gameover, 0, Qt::AlignCenter);
  layout->addWidget(reset, 0, Qt::AlignCenter);

  hide(); // hide by default on startup
}

QResetButton *QGameOverWindow::getResetBtn() const { return reset; }
