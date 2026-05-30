// qwinwindow.cpp - winning screen UI
#include "gui/qwinwindow.h"
#include "gui/qresetbutton.h"

#include <QVBoxLayout>
#include <QLabel>

QWinWindow::QWinWindow(QWidget *parent) : QFrame(parent)
{
    // Make sure the background stylesheet actually renders for this custom QWidget
    setAttribute(Qt::WA_StyledBackground, true);
    
    // light background for winning overlay (added alpha to look like an overlay)
    setStyleSheet("QWinWindow { background: rgba(237, 224, 200, 220); }");
    setFixedSize(425, 205);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel* winLabel = new QLabel("You Win! 🎉", this);
    winLabel->setStyleSheet("QLabel {"
                            "  color: rgb(119,110,101);"
                            "  font-size: 40pt;"
                            "  font-weight: bold;"
                            "}");
    winLabel->setAlignment(Qt::AlignCenter);

    restartBtn = new QResetButton(this);
    restartBtn->setText("Play Again!");
    restartBtn->setFixedHeight(50);
    restartBtn->setFixedWidth(120);

    layout->addWidget(winLabel,   0, Qt::AlignCenter);
    layout->addWidget(restartBtn, 0, Qt::AlignCenter);

    hide(); // starts hidden until player actually wins
}

QResetButton* QWinWindow::getRestartBtn() const
{
    return restartBtn;
}