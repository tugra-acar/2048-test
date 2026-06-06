#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/qgameboard.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGameBoard *gameBoard; // our actual game widget
};

#endif // MAINWINDOW_H
