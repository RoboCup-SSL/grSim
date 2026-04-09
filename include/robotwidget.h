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

#ifndef ROBOTWIDGET_H
#define ROBOTWIDGET_H

#include <QDockWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include "configwidget.h"
#include "getpositionwidget.h"

class RobotWidget : public QDockWidget
{
    Q_OBJECT
public:    
    RobotWidget(QWidget* parent, ConfigWidget* cfg);
    void setPicture(QImage* img);
    void changeRobotCount(int newRobotCount);
    void changeCurrentRobot(int CurrentRobotID);
    QComboBox *teamCombo,*robotCombo;
    QLabel *robotpic;
    QLabel *vellabel,*acclabel;
    QPushButton *resetBtn,*locateBtn;
    QPushButton *onOffBtn,*setPoseBtn;
    GetPositionWidget* getPoseWidget;
    int id;
public slots:
    void changeRobotOnOff(int,bool);
    void setPoseBtnClicked();
};

#endif // ROBOTWIDGET_H
