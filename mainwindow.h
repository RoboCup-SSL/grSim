#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>

#include "widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void update();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Widget *widget;
    QLabel *fpslabel;
    QLabel *robotlabel;
};

#endif // MAINWINDOW_H
