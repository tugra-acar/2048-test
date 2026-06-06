#include "gui/mainwindow.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Create a central widget and a layout to center the game board
    QWidget *central = new QWidget(this);
    central->setStyleSheet("QWidget { background-color: #faf8ef; }"); // matching the game background
    QHBoxLayout *layout = new QHBoxLayout(central);
    
    // just put the game board in the center
    gameBoard = new QGameBoard(this);
    gameBoard->setFixedSize(520, 620); // Keep it exactly this size
    
    layout->addWidget(gameBoard, 0, Qt::AlignCenter);
    setCentralWidget(central);
}

MainWindow::~MainWindow()
{
}
