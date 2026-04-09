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

#include "config.h"
#include "robotwidget.h"
#include <QtGui>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>

RobotWidget::RobotWidget(QWidget* parent, ConfigWidget* cfg)
    : QDockWidget("Current Robot",parent)
{
    QGridLayout *layout = new QGridLayout;
    robotpic = new QLabel;
    teamCombo = new QComboBox(this);
    teamCombo->addItem("Blue");
    teamCombo->addItem("Yellow");
    robotCombo = new QComboBox(this);

    // Add items to the combo box dynamically 
    changeRobotCount(cfg->Robots_Count());

    vellabel = new QLabel;
    acclabel = new QLabel;
    resetBtn = new QPushButton("Reset");
    locateBtn = new QPushButton("Locate");
    onOffBtn = new QPushButton("Turn Off");
    setPoseBtn = new QPushButton("Set Position");
    layout->addWidget(robotpic,0,0,5,1);
    layout->addWidget(new QLabel("Team"),0,1);
    layout->addWidget(teamCombo,0,2);
    layout->addWidget(new QLabel("Index"),1,1);
    layout->addWidget(robotCombo,1,2);
    layout->addWidget(new QLabel("Velocity"),2,1);
    layout->addWidget(vellabel,2,2);
    layout->addWidget(resetBtn,3,1);
    layout->addWidget(locateBtn,3,2);
    layout->addWidget(onOffBtn,4,1);
    layout->addWidget(setPoseBtn,4,2);
    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    widget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setWidget(widget);
    getPoseWidget = new GetPositionWidget();
    QObject::connect(setPoseBtn,SIGNAL(clicked()),this,SLOT(setPoseBtnClicked()));
}

void RobotWidget::changeRobotCount(int newRobotCount) {
    // block signal emitting to avoid segmentation fault
    // if don't do this, CurrentIndexChange() signal will be emitted everytime adding/deleting item
    // and MainWindow::changeCurrentRobot() will be called
    robotCombo->blockSignals(true);

    // increase comboBox element
    if(newRobotCount > robotCombo->count()) {
        for (int i=robotCombo->count(); i<newRobotCount; i++){
            QString item=QString::number(i);
            robotCombo->addItem(item);
        }
    }
    // decrease comboBox element
    else {
        for (int i=robotCombo->count(); i>=newRobotCount; i--) {
            robotCombo->removeItem(i);
        }
    }
    robotCombo->blockSignals(false);
}

void RobotWidget::changeCurrentRobot(int CurrentRobotID) {
    CurrentRobotID = std::min(CurrentRobotID, robotCombo->count()-1);
    robotCombo->setCurrentIndex(CurrentRobotID);
}

void RobotWidget::setPicture(QImage* img)
{
    robotpic->setPixmap(QPixmap::fromImage(*img).scaled(128, 128, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void RobotWidget::changeRobotOnOff(int _id,bool a)
{
    if (_id==id) {
        if (a) onOffBtn->setText("Turn off");
        else onOffBtn->setText("Turn on");
    }
}

void RobotWidget::setPoseBtnClicked()
{
    getPoseWidget->show();
}
