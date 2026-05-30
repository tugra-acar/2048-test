// qresetbutton.h - custom label that acts like a button
// we use this instead of QPushButton so it matches the overlay style better
#ifndef QRESETBUTTON_H
#define QRESETBUTTON_H

#include <QLabel>

class QResetButton : public QLabel
{
    Q_OBJECT
public:
    explicit QResetButton(QWidget* parent = nullptr);

signals:
    void clicked(); // emitted when user clicks the label

protected:
    // intercepts mouse clicks so we can emit our custom signal
    void mousePressEvent(QMouseEvent* event) override;
};

#endif // QRESETBUTTON_H
