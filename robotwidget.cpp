#include "robotwidget.h"
#include <QtGui>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
RobotWidget::RobotWidget(QWidget* parent)
        : QDockWidget("Current Robot",parent)
{    
    QGridLayout *layout = new QGridLayout;    
    robotpic = new QLabel;
    teamCombo = new QComboBox(this);
    teamCombo->addItem("Blue");
    teamCombo->addItem("Yellow");
    robotCombo = new QComboBox(this);
    robotCombo->addItem("0");
    robotCombo->addItem("1");
    robotCombo->addItem("2");
    robotCombo->addItem("3");
    robotCombo->addItem("4");
    vellabel = new QLabel;
    acclabel = new QLabel;
    resetBtn = new QPushButton("Reset");
    locateBtn = new QPushButton("Locate");
    layout->addWidget(robotpic,0,0,5,1);
    layout->addWidget(new QLabel("Team"),0,1);
    layout->addWidget(teamCombo,0,2);
    layout->addWidget(new QLabel("Index"),1,1);
    layout->addWidget(robotCombo,1,2);
    layout->addWidget(new QLabel("Velocity"),2,1);
    layout->addWidget(vellabel,2,2);    
    layout->addWidget(new QLabel("Acceleration"),3,1);
    layout->addWidget(acclabel,3,2);
    layout->addWidget(resetBtn,4,1);
    layout->addWidget(locateBtn,4,2);        
    QWidget *widget = new QWidget(this);    
    widget->setLayout(layout);    
    setWidget(widget);    
    layout->setSizeConstraint(QLayout::SetMinimumSize);    
}

void RobotWidget::setPicture(QImage* img)
{
    robotpic->setPixmap(QPixmap::fromImage(*img));
}

