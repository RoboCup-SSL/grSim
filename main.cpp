/*
GUI and program
============

**separate team parameters


f12
qdebug
select robot (draw circle)
put outside blue( to the other side)
locate ball => set vel = 0 (also robot - reset robot)
grass texture => webots
remove all unused vars in varswidget
formation1 & 2 => ini file
Icon & program title => grSim in all the code
select names for camera modes
[C => toggle mode
 all modes => shortcut keys]
Help => about
turn off bug
remove plotter
all keyboad shortcuts => actions

=============
code

comments
refactor variable names





  */

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
