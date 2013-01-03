/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include "winmain.h"

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
