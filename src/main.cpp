#include "gui/mainwindow.h"
#include <QApplication>

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "core/board.h"
#include "gui/qgameboard.h"

#include "gui/qgameoverwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("2048 Game - Assignment 3");
    a.setOrganizationName("BOUN-CMPE230");

    srand(time(NULL));
    QGameBoard board;
    board.show();

    return a.exec();
}
