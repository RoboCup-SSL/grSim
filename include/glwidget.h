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

#define GL_SILENCE_DEPRECATION
#include <QGLWidget>
#include <QTime>
#include <QMenu>

#include "sslworld.h"
#include "configwidget.h"


class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    enum class CursorMode {
        STEADY = 0,
        PLACE_ROBOT = 1,
        PLACE_BALL = 2,
    };
    enum class CameraMode {
        BIRDS_EYE_FROM_TOUCH_LINE = 0,
        CURRENT_ROBOT_VIEW = 1,
        TOP_VIEW = 2,
        BIRDS_EYE_FROM_OPPOSITE_TOUCH_LINE = 3,
        BIRDS_EYE_FROM_BLUE = 4,
        BIRDS_EYE_FROM_YELLOW = 5,
        MAX_ACTIVE_MODE_FOR_CHANGEMODE=BIRDS_EYE_FROM_YELLOW,
        // non-avaliable modes when "toggle camera mode" called
        LOCK_TO_ROBOT = -1,
        LOCK_TO_BALL = -2,
    };
    GLWidget(QWidget *parent,ConfigWidget* _cfg);
    ~GLWidget();
    dReal getFPS();
    ConfigWidget* cfg;   
    SSLWorld* ssl;
    RobotsFormation* forms[4];
    QMenu* robpopup,*ballpopup,*mainpopup;
    QMenu *blueRobotsMenu,*yellowRobotsMenu,*allRobotsMenu;
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
    int Current_robot,Current_team;
    int lockedIndex;
    bool ctrl,alt,kickingball,altTrigger;
    bool chiping;
    double kickpower, chipAngle;
    void update3DCursor(int mouse_x,int mouse_y);
    void putBall(dReal x,dReal y);
    void reform(int team,const QString& act);    
    void step();
public slots:
    void moveRobot();
    void selectRobot();
    void unselectRobot();    
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
    CursorMode state;
    CameraMode cammode;
    int moving_robot_id,clicked_robot;
    int frames;
    bool first_time;
    QTime time,rendertimer;
    dReal m_fps;
    QPoint lastPos;
};

#endif // WIDGET_H
