#ifndef ROBOTWIDGET_H
#define ROBOTWIDGET_H

#include <QDockWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "getpositionwidget.h"
class RobotWidget : public QDockWidget
{
    Q_OBJECT
public:    
    RobotWidget(QWidget* parent);
    void setPicture(QImage* img);
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
