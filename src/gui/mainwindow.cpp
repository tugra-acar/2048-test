// mainwindow.cpp
#include "gui/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // just put the game board in the center
    gameBoard = new QGameBoard(this);
    setCentralWidget(gameBoard);
}

MainWindow::~MainWindow()
{
}
