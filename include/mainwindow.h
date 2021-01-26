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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMdiArea>

#include <QLabel>

#include "glwidget.h"
#include "configwidget.h"
#include "statuswidget.h"
#include "robotwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() override;
public slots:
    void update();
    void updateRobotLabel();
    void showHideSimulator(bool v);
    void changeCurrentRobot();
    void changeCurrentTeam();

    void changeRobotCount();

    void changeBallMass();   
    void changeBallGroundSurface();    
    void changeBallDamping();
    void changeGravity();
    void changeTimer();

    void restartSimulator();
    void ballMenuTriggered(QAction* act);
    void toggleFullScreen(bool);
    void setCurrentRobotPosition();
    void takeSnapshot();
    void takeSnapshotToClipboard();

    void customFPS(int fps);
    void showAbout();
    void reconnectCommandSocket();
    void reconnectYellowStatusSocket();
    void reconnectBlueStatusSocket();
    void reconnectVisionSocket();
    void reconnectSimControlSocket();
    void reconnectBlueControlSocket();
    void reconnectYellowControlSocket();
    void recvActions();
    void simControlSocketReady();
    void blueControlSocketReady();
    void yellowControlSocketReady();
    void setIsGlEnabled(bool value);

    int robotIndex(int robot,int team);
private:
    int getInterval();    
    QTimer *timer;
    GLWidget *glwidget;
    ConfigWidget *configwidget;
    ConfigDockWidget *dockconfig;
    RobotWidget *robotwidget;        
    QByteArray prevState;

    CStatusPrinter *printer;
    CStatusWidget *statusWidget;

    QAction *showsimulator, *showconfig, *showrobot;
    QAction* fullScreenAct;
    QLabel *fpslabel,*cursorlabel,*selectinglabel,*vanishlabel,*noiselabel;
    QString current_dir;

    RoboCupSSLServer *visionServer;
    QUdpSocket *commandSocket;
    QUdpSocket *blueStatusSocket,*yellowStatusSocket;
    QUdpSocket *simControlSocket;
    QUdpSocket *blueControlSocket;
    QUdpSocket *yellowControlSocket;
};

#endif // MAINWINDOW_H
