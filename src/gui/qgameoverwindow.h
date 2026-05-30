// qgameoverwindow.h - the overlay that shows up when you lose
#ifndef QGAMEOVERWINDOW_H
#define QGAMEOVERWINDOW_H

#include <QWidget>

class QResetButton;

class QGameOverWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QGameOverWindow(QWidget *parent = nullptr);

    // we need this so main board can connect the button click to restart game
    QResetButton* getResetBtn() const;

private:
    QResetButton* reset;
};

#endif // QGAMEOVERWINDOW_H
