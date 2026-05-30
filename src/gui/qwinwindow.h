// qwinwindow.h - you get this overlay if you reach 2048
#ifndef QWINWINDOW_H
#define QWINWINDOW_H

#include <QFrame>

class QResetButton;

class QWinWindow : public QFrame
{
    Q_OBJECT
public:
    explicit QWinWindow(QWidget *parent = nullptr);

    // game board needs this to handle the play again click
    QResetButton* getRestartBtn() const;

private:
    QResetButton* restartBtn;
};

#endif // QWINWINDOW_H