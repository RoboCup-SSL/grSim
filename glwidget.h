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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QGraphicsView>
#include <QTime>
#include <QMenu>

#include "sslworld.h"
#include "configwidget.h"

class GLWidgetGraphicsView;
class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    GLWidget(QWidget *parent,ConfigWidget* _cfg);
    ~GLWidget();
    dReal getFPS();
    ConfigWidget* cfg;   
    SSLWorld* ssl;
    RobotsFomation* forms[6];
    QMenu* robpopup,*ballpopup,*mainpopup;
    QMenu *blueRobotsMenu,*yellowRobotsMenu;
    QAction* moveRobotAct;
    QAction* selectRobotAct;
    QAction* resetRobotAct;
    QAction* moveBallAct;        
    QAction* lockToBallAct;
    QAction* onOffRobotAct;
    QAction* lockToRobotAct;
    QAction* moveBallHereAct;
    QAction* moveRobotHereAct;
    QAction* changeCamModeAct;
    QMenu *cameraMenu;
    int Current_robot,Current_team,cammode;
    int lockedIndex;
    bool ctrl,alt,kickingball;
    double kickpower;
    bool fullScreen;
    void update3DCursor(int mouse_x,int mouse_y);
    void putBall(dReal x,dReal y);
    void reform(int team,const QString& act);    
    void step();
public slots:
    void moveRobot();
    void resetRobot();
    void selectRobot();
    void moveCurrentRobot();
    void resetCurrentRobot();
    void moveBall();
    void changeCameraMode();
    void yellowRobotsMenuTriggered(QAction* act);
    void blueRobotsMenuTriggered(QAction* act);
    void switchRobotOnOff();
    void moveRobotHere();
    void moveBallHere();
    void lockCameraToRobot();
    void lockCameraToBall();
signals:
    void clicked();
    void selectedRobot();
    void closeSignal(bool);
    void toggleFullScreen(bool);
    void robotTurnedOnOff(int,bool);
protected:
    void paintGL ();
    void initializeGL ();        

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);    
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent *event);    
private:
    int state;
    int moving_robot_id,clicked_robot;
    int frames;
    bool first_time;
    QTime time,rendertimer;
    dReal m_fps;
    QPoint lastPos;
friend class GLWidgetGraphicsView;
};

class GLWidgetGraphicsView : public QGraphicsView        
{
    private:
        GLWidget *glwidget;
    public:
        GLWidgetGraphicsView(QGraphicsScene *scene,GLWidget* _glwidget);
    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void closeEvent(QCloseEvent *event);
};

#endif // WIDGET_H
