// qgameoverwindow.cpp - game over screen UI
#include "gui/qgameoverwindow.h"
#include "gui/qresetbutton.h"

#include <QLabel>
#include <QVBoxLayout>

QGameOverWindow::QGameOverWindow(QWidget *parent) : QDialog(parent) {
  // Use classic window appearance instead of custom overlay
  setWindowFlags(Qt::Dialog);
  setWindowTitle("Game Over!");
  setFixedSize(300, 150);
  setStyleSheet("QGameOverWindow { background-color: #faf8ef; }");

  QVBoxLayout *layout = new QVBoxLayout(this);

  // main text
  QLabel *gameover = new QLabel("Game Over!", this);
  gameover->setStyleSheet("QLabel {"
                          "  color: rgb(119,110,101);"
                          "  font-size: 28pt;"
                          "  font-weight: bold;"
                          "}");
  gameover->setAlignment(Qt::AlignCenter);

  // the try again button
  reset = new QResetButton(this);
  reset->setFixedHeight(40);
  reset->setFixedWidth(100);

  layout->addWidget(gameover, 0, Qt::AlignCenter);
  layout->addWidget(reset, 0, Qt::AlignCenter);

  // Connect the dialog's finished signal to ensure it hides properly if closed natively
  connect(this, &QDialog::finished, this, &QDialog::hide);
}

QResetButton *QGameOverWindow::getResetBtn() const { return reset; }
