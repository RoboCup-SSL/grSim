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
#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "winmain.h"



int main(int argc, char *argv[])
{
    std::locale::global( std::locale( "" ) );
    
    char** argend = argc + argv;

    if (std::find(argv, argend, std::string("--help")) != argend) {
        std::cout << "grSim [-H/--headless] [-b blue_team]  [-y yellow_team]" << std::endl;
        return 0;
    }

    QCoreApplication::setOrganizationName("Parsian");
    QCoreApplication::setOrganizationDomain("parsian-robotics.com");
    QCoreApplication::setApplicationName("grSim");
    QApplication a(argc, argv);

    boost::optional<std::string> blue_teamname, yellow_teamname;

    auto iter_blue = std::find(argv, argend, std::string("-b"));
    auto iter_yellow = std::find(argv, argend, std::string("-y"));

    if (iter_blue != argend && iter_blue + 1 != argend) {
        blue_teamname = std::string(iter_blue[1]);
    }
    if (iter_yellow != argend && iter_yellow + 1 != argend) {
        yellow_teamname = std::string(iter_yellow[1]);
    }

    MainWindow w(0, yellow_teamname, blue_teamname);

    if (std::find(argv, argend, std::string("--headless")) != argend
        || std::find(argv, argend, std::string("-H")) != argend) {
        // enable headless mode
        w.hide();
        w.setIsGlEnabled(false);
    } else {
        // Run normally
        w.show();
    }
    return a.exec();
}
