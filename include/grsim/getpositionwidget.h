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

#ifndef GETPOSITIONWIDGET_H
#define GETPOSITIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class GetPositionWidget : public QWidget
{
    Q_OBJECT
public:
    GetPositionWidget();
    QLineEdit *x,*y,*a;
    QPushButton *okBtn,*cancelBtn;
public slots:
    void cancelBtnClicked();
};

#endif // GETPOSITIONWIDGET_H
