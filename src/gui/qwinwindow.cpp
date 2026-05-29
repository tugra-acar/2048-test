#include "gui/qwinwindow.h"
#include "gui/qresetbutton.h"

#include <QVBoxLayout>
#include <QLabel>

QWinWindow::QWinWindow(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("QWinWindow { background: rgb(237, 224, 200); }");
    setFixedSize(425, 205);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel* winLabel = new QLabel("You Win! 🎉", this);
    winLabel->setStyleSheet("QLabel { color: rgb(119,110,101); font-size: 40pt; font-weight: bold; }");
    winLabel->setAlignment(Qt::AlignCenter);

    restartBtn = new QResetButton(this);
    restartBtn->setText("Play Again!");
    restartBtn->setFixedHeight(50);
    restartBtn->setFixedWidth(120);

    layout->addWidget(winLabel,    0, Qt::AlignCenter);
    layout->addWidget(restartBtn,  0, Qt::AlignCenter);

    hide();
}

QResetButton* QWinWindow::getRestartBtn() const
{
    return restartBtn;
}