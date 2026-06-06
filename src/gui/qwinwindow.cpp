// qwinwindow.cpp - winning screen UI
#include "gui/qwinwindow.h"
#include "gui/qresetbutton.h"

#include <QLabel>
#include <QVBoxLayout>

QWinWindow::QWinWindow(QWidget *parent) : QDialog(parent) {
  // Use classic window appearance instead of custom overlay
  setWindowFlags(Qt::Dialog);
  setWindowTitle("You Win!");
  setFixedSize(300, 150);
  setStyleSheet("QWinWindow { background-color: #faf8ef; }");

  QVBoxLayout *layout = new QVBoxLayout(this);

  QLabel *winLabel = new QLabel("You Win!", this);
  winLabel->setStyleSheet("QLabel {"
                          "  color: rgb(119,110,101);"
                          "  font-size: 28pt;"
                          "  font-weight: bold;"
                          "}");
  winLabel->setAlignment(Qt::AlignCenter);

  restartBtn = new QResetButton(this);
  restartBtn->setText("Play Again!");
  restartBtn->setFixedHeight(40);
  restartBtn->setFixedWidth(120);

  layout->addWidget(winLabel, 0, Qt::AlignCenter);
  layout->addWidget(restartBtn, 0, Qt::AlignCenter);
  
  // Connect the dialog's finished signal to ensure it hides properly if closed natively
  connect(this, &QDialog::finished, this, &QDialog::hide);
}

QResetButton *QWinWindow::getRestartBtn() const { return restartBtn; }