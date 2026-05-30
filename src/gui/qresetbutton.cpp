// qresetbutton.cpp - styles the try again/play again buttons
#include "gui/qresetbutton.h"

QResetButton::QResetButton(QWidget* parent) : QLabel(parent)
{
    setText("Try again!");
    setAlignment(Qt::AlignCenter);
    setStyleSheet("QResetButton {"
                  "  background-color: rgb(143,122,102);"
                  "  border-radius: 10px;"
                  "  font-weight: bold;"
                  "  color: white;"
                  "}");
}

void QResetButton::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit clicked(); // tell the gameboard that user clicked this
}
