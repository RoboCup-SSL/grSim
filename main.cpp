/*
GUI and program
============

DONE **separate team parameters


DONE f12
DONE qdebug

put outside blue( to the other side)
DONE locate ball => set vel = 0 (also robot - reset robot)
grass texture => webots
DONE remove all unused vars in varswidget

formation1 & 2 => ini file

Icon & program title => grSim in all the code
select names for camera modes
[C => toggle mode
 all modes => shortcut keys]
Help => about
DONE turn off bug
DONE remove plotter
all keyboad shortcuts => actions

select robot (draw circle)

=============
code

comments
refactor variable names





  */

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Parsian");
    QCoreApplication::setOrganizationDomain("parsian-robotics.com");
    QCoreApplication::setApplicationName("grSim");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
