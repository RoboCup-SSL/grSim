#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWorkspace>
#include <QLabel>
#include <QComboBox>

#include "glwidget.h"
#include "configwidget.h"

#include "statuswidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void update();
    void updateRobotLabel();
    void showHideConfig(bool v);
    void showHideSimulator(bool v);
    void changeCurrentRobot();
    void changeCurrentTeam();

    void changeBallMass();
    void changeRobotMass();
    void changeKickerMass();
    void changeWheelMass();
    void changeBallGroundSurface();
    void changeBallDamping();
    void changeGravity();

    void alertStaticVars();
private:
    QWorkspace* workspace;
    GLWidget *glwidget;
    ConfigWidget *configwidget;
    QDockWidget *dockconfig;

    CStatusPrinter *printer;
    CStatusWidget *statusWidget;

    QAction *showsimulator, *showconfig;
    QLabel *fpslabel,*cursorlabel,*selectinglabel;

    QComboBox *teamCombo,*robotCombo;
};

#endif // MAINWINDOW_H
