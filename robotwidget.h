#ifndef ROBOTWIDGET_H
#define ROBOTWIDGET_H

#include <QDockWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
class RobotWidget : public QDockWidget
{
public:    
    RobotWidget(QWidget* parent);
    void setPicture(QImage* img);
    QComboBox *teamCombo,*robotCombo;
    QLabel *robotpic;
    QLabel *vellabel,*acclabel;
    QPushButton *resetBtn,*locateBtn;
};

#endif // ROBOTWIDGET_H
