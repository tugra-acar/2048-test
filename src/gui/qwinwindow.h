#ifndef QWINWINDOW_H
#define QWINWINDOW_H

#include <QWidget>

class QResetButton;

class QWinWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QWinWindow(QWidget *parent = nullptr);

    QResetButton* getRestartBtn() const;

signals:

public slots:

private:
    QResetButton* restartBtn;
};

#endif // QWINWINDOW_H