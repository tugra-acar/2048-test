// main.cpp - starts the app
#include "gui/mainwindow.h"

#include <QApplication>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // needed for QSettings to know where to save the high score
    a.setApplicationName("2048 Game - Assignment 3");
    a.setOrganizationName("BOUN-CMPE230");

    // seed random generator so tiles dont spawn the exact same way every time
    srand(static_cast<unsigned>(time(nullptr)));

    // just show the board directly inside the main window
    MainWindow window;
    window.show();

    return a.exec();
}
