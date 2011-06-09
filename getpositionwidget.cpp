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

#include "getpositionwidget.h"
#include <QtGui>
#include <QGridLayout>
#include <QLabel>

GetPositionWidget::GetPositionWidget()
{
    QGridLayout *layout = new QGridLayout(this);
    okBtn = new QPushButton("&Ok",this);
    cancelBtn = new QPushButton("&Cancel",this);
    x = new QLineEdit(this);
    y = new QLineEdit(this);
    a = new QLineEdit(this);
    layout->addWidget(new QLabel("X"),0,0);
    layout->addWidget(new QLabel("Y"),1,0);
    layout->addWidget(new QLabel("Angle"),2,0);
    layout->addWidget(okBtn,3,1);
    layout->addWidget(cancelBtn,3,0);
    layout->addWidget(x,0,1);
    layout->addWidget(y,1,1);
    layout->addWidget(a,2,1);
    setLayout(layout);
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(cancelBtnClicked()));
}

void GetPositionWidget::cancelBtnClicked()
{
    close();
}
