// mainwindow.h - main app window 
// we actually just show QGameBoard directly right now, but keeping this
// in case we need a full QMainWindow later
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
