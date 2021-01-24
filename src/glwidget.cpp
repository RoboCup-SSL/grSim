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
#include <QtGui>

#include <QPainter>
#include "glwidget.h"
#include "physics/pball.h"
#include "physics/pground.h"
#include "logger.h"
#include <QLabel>

#include <iostream>

GLWidget::GLWidget(QWidget *parent, ConfigWidget* _cfg)
    : QGLWidget(parent)
{
    frames = 0;
    state = CursorMode::STEADY;
    first_time = true;
    cfg = _cfg;

    forms[FORMATION_OUTSIDE] = new RobotsFormation(FORMATION_OUTSIDE, cfg);
    forms[FORMATION_INSIDE_1] = new RobotsFormation(FORMATION_INSIDE_1, cfg);
    forms[FORMATION_INSIDE_2] = new RobotsFormation(FORMATION_INSIDE_2, cfg);
    forms[FORMATION_OUTSIDE_FIELD] = new RobotsFormation(FORMATION_OUTSIDE_FIELD, cfg);

    ssl = new SSLWorld(this,cfg,forms[FORMATION_INSIDE_1],forms[FORMATION_INSIDE_1]);
    Current_robot = 0;
    Current_team = 0;
    cammode = CameraMode::BIRDS_EYE_FROM_TOUCH_LINE;
    setMouseTracking(true);

    blueRobotsMenu = new QMenu("&Blue Robots");
    yellowRobotsMenu = new QMenu("&Yellow Robots");
    allRobotsMenu = new QMenu("&All Robots");
    blueRobotsMenu->addAction(tr("Put all inside with formation 1"));
    blueRobotsMenu->addAction(tr("Put all inside with formation 2"));
    blueRobotsMenu->addAction(tr("Put all outside"));
    blueRobotsMenu->addAction(tr("Put all out of field"));
    blueRobotsMenu->addAction(tr("Turn all off"));
    blueRobotsMenu->addAction(tr("Turn all on"));
    yellowRobotsMenu->addAction(tr("Put all inside with formation 1"));
    yellowRobotsMenu->addAction(tr("Put all inside with formation 2"));
    yellowRobotsMenu->addAction(tr("Put all outside"));
    yellowRobotsMenu->addAction(tr("Put all out of field"));
    yellowRobotsMenu->addAction(tr("Turn all off"));
    yellowRobotsMenu->addAction(tr("Turn all on"));
    allRobotsMenu->addAction(tr("Put all inside with formation 1"));
    allRobotsMenu->addAction(tr("Put all inside with formation 2"));
    allRobotsMenu->addAction(tr("Put all outside"));
    allRobotsMenu->addAction(tr("Put all out of field"));
    allRobotsMenu->addAction(tr("Turn all off"));
    allRobotsMenu->addAction(tr("Turn all on"));
    robpopup = new QMenu(this);
    moveRobotAct = new QAction(tr("&Locate robot"),this);
    selectRobotAct = new QAction(tr("&Select robot"),this);
    resetRobotAct = new QAction(tr("&Reset robot"),this);
    onOffRobotAct = new QAction(tr("Turn &off"),this);
    lockToRobotAct = new QAction(tr("Loc&k camera to this robot"),this);    
    robpopup->addAction(moveRobotAct);
    robpopup->addAction(resetRobotAct);
    robpopup->addAction(onOffRobotAct);
    robpopup->addAction(lockToRobotAct);
    robpopup->addMenu(blueRobotsMenu);
    robpopup->addMenu(yellowRobotsMenu);
    robpopup->addMenu(allRobotsMenu);

    moveBallAct = new QAction(tr("&Locate ball"),this);
    lockToBallAct = new QAction(tr("Loc&k camera to ball"),this);
    ballpopup = new QMenu(this);
    ballpopup->addAction(moveBallAct);
    ballpopup->addAction(lockToBallAct);

    moveRobotHereAct = new QAction(tr("Locate current &robot here"),this);
    moveBallHereAct = new QAction(tr("Locate &ball here"),this);
    mainpopup = new QMenu(this);
    mainpopup->addAction(moveBallHereAct);
    mainpopup->addAction(moveRobotHereAct);
    cameraMenu = new QMenu("&Camera");
    changeCamModeAct=new QAction(tr("Change &mode"),this);
    changeCamModeAct->setShortcut(QKeySequence("C"));
    cameraMenu->addAction(changeCamModeAct);
    cameraMenu->addAction(lockToRobotAct);
    cameraMenu->addAction(lockToBallAct);

    mainpopup->addMenu(cameraMenu);
    mainpopup->addMenu(blueRobotsMenu);
    mainpopup->addMenu(yellowRobotsMenu);
    mainpopup->addMenu(allRobotsMenu);


    connect(moveRobotAct, SIGNAL(triggered()), this, SLOT(moveRobot()));
    connect(selectRobotAct, SIGNAL(triggered()), this, SLOT(selectRobot()));
    connect(resetRobotAct, SIGNAL(triggered()), this, SLOT(resetCurrentRobot()));
    connect(moveBallAct, SIGNAL(triggered()), this, SLOT(moveBall()));
    connect(onOffRobotAct, SIGNAL(triggered()), this, SLOT(switchRobotOnOff()));
    connect(yellowRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(yellowRobotsMenuTriggered(QAction*)));
    connect(blueRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(blueRobotsMenuTriggered(QAction*)));
    connect(allRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(yellowRobotsMenuTriggered(QAction*)));
    connect(allRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(blueRobotsMenuTriggered(QAction*)));
    connect(moveBallHereAct, SIGNAL(triggered()),this , SLOT(moveBallHere()));
    connect(moveRobotHereAct, SIGNAL(triggered()),this , SLOT(moveRobotHere()));
    connect(lockToRobotAct, SIGNAL(triggered()), this, SLOT(lockCameraToRobot()));
    connect(lockToBallAct, SIGNAL(triggered()), this, SLOT(lockCameraToBall()));
    connect(changeCamModeAct,SIGNAL(triggered()),this,SLOT(changeCameraMode()));
    setFocusPolicy(Qt::StrongFocus);
    ctrl = false;
    alt = false;
    kickingball = false;
    kickpower = 3.0;
    altTrigger = false;
    chipAngle = M_PI/4;
    chiping = false;
}

GLWidget::~GLWidget()
{
}

void GLWidget::moveRobot()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->robotSettings.RobotRadius;
    state = CursorMode::PLACE_ROBOT;
    moving_robot_id = clicked_robot;
}

void GLWidget::unselectRobot()
{
    ssl->show3DCursor = false;
    ssl->cursor_radius = cfg->robotSettings.RobotRadius;
    state = CursorMode::STEADY;
    moving_robot_id= ssl->robotIndex(Current_robot,Current_team);
}

void GLWidget::selectRobot()
{
    if (clicked_robot!=-1)
    {
        Current_robot = clicked_robot%cfg->Robots_Count();
        Current_team = clicked_robot/cfg->Robots_Count();
        emit selectedRobot();
    }
}

void GLWidget::resetCurrentRobot()
{
    if (Current_robot!=-1)
    {
        ssl->robots[ssl->robotIndex(Current_robot, Current_team)]->resetRobot();
    }
}

void GLWidget::switchRobotOnOff()
{
    int k = ssl->robotIndex(Current_robot, Current_team);
    if (Current_robot!=-1)
    {
        auto& robot_on = ssl->robots[k]->on;
        robot_on = !robot_on;
        onOffRobotAct->setText(robot_on ? "Turn &off" : "Turn &on");
        emit robotTurnedOnOff(k,robot_on);
    }
}

void GLWidget::moveCurrentRobot()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->robotSettings.RobotRadius;
    state = CursorMode::PLACE_ROBOT;
    moving_robot_id = ssl->robotIndex(Current_robot,Current_team);
}

void GLWidget::moveBall()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->BallRadius();
    state = CursorMode::PLACE_BALL;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!ssl->g->isGraphicsEnabled()) return;
    lastPos = event->pos();
    if (event->buttons() & Qt::LeftButton)
    {
        if (state==CursorMode::PLACE_ROBOT)
        {
            if (moving_robot_id!=-1)
            {
                ssl->robots[moving_robot_id]->setXY(ssl->cursor_x,ssl->cursor_y);
                state = CursorMode::STEADY;
                ssl->show3DCursor = false;
            }
        }
        else if (state==CursorMode::PLACE_BALL)
        {
            ssl->ball->setBodyPosition(ssl->cursor_x,ssl->cursor_y,cfg->BallRadius()*1.1*20.0);
            dBodySetAngularVel(ssl->ball->body,0,0,0);
            dBodySetLinearVel(ssl->ball->body,0,0,0);
            ssl->show3DCursor = false;
            state = CursorMode::STEADY;
        }
        else {
            if (ssl->selected>=0){
                clicked_robot = ssl->selected;
                selectRobot();
            }
            if (kickingball)
            {
                dReal x,y,z;
                ssl->ball->getBodyPosition(x,y,z);                
                x = ssl->cursor_x - x;
                y = ssl->cursor_y - y;
                dReal lxy = hypot(x,y);
                x /= lxy;
                y /= lxy;
                x *= kickpower;
                y *= kickpower;
                dBodySetLinearVel(ssl->ball->body,x,y,0);
                dBodySetAngularVel(ssl->ball->body,-y/cfg->BallRadius(),x/cfg->BallRadius(),0);
            }
            else if (chiping) {
                dReal x,y,z;
                ssl->ball->getBodyPosition(x,y,z);
                x = ssl->cursor_x - x;
                y = ssl->cursor_y - y;
                dReal lxy = hypot(x,y);
                x /= lxy;
                y /= lxy;
                x *= kickpower;
                y *= kickpower;
                z = kickpower*tan(chipAngle);

                dBodySetLinearVel(ssl->ball->body,x,y,z);
                dBodySetAngularVel(ssl->ball->body,-y/cfg->BallRadius(),x/cfg->BallRadius(),z);    
            }
        
        }
    }
    if (event->buttons() & Qt::RightButton)
    {
        if (ssl->selected!=-1 && ssl->selected!=-2)
        {
            clicked_robot = ssl->selected;
            selectRobot();
            if (ssl->robots[ssl->selected]->on)
                onOffRobotAct->setText("Turn &off");
            else onOffRobotAct->setText("Turn &on");
            robpopup->exec(event->globalPos());
        }
        else clicked_robot=-1;
        if (ssl->selected==-2)
            ballpopup->exec(event->globalPos());
        if (ssl->selected==-1)
            mainpopup->exec(event->globalPos());
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (!ssl->g->isGraphicsEnabled()) return;
    ssl->g->zoomCamera(-event->delta()*0.002);
    update3DCursor(event->x(),event->y());
}

void GLWidget::update3DCursor(int mouse_x,int mouse_y)
{
    if (!ssl->g->isGraphicsEnabled()) return;
    ssl->updatedCursor = true;
    dVector3 xyz,hpr;
    dReal fx,fy,fz,rx,ry,rz,ux,uy,uz,px,py,pz;
    ssl->g->getViewpoint(xyz,hpr);
    ssl->g->getCameraForward(fx,fy,fz);
    ssl->g->getCameraRight(rx,ry,rz);    
    ux = ry*fz - rz*fy;
    uy = rz*fx - rx*fz;
    uz = rx*fy - ry*fx;
    dReal w = width();
    dReal h = height();
    dReal xx,yy,z;
    dReal x = 1.0f - 2.0f*(dReal) mouse_x / w;
    dReal y = 1.0f - 2.0f*(dReal) mouse_y / h;
    ssl->g->getFrustum(xx,yy,z);
    x *= xx;
    y *= yy;
    px = -ux*y - rx*x - z*fx;
    py = -uy*y - ry*x - z*fy;
    pz = -uz*y - rz*x - z*fz;
    ssl->ray->setPose(xyz[0],xyz[1],xyz[2],px,py,pz);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!ssl->g->isGraphicsEnabled()) return;
    int dx = -(event->x() - lastPos.x());
    int dy = -(event->y() - lastPos.y());    
    if (event->buttons() & Qt::LeftButton) {
        if (ctrl)
            ssl->g->cameraMotion(CameraMotionMode::MOVE_POSITION_FREELY,dx,dy);
        else
            ssl->g->cameraMotion(CameraMotionMode::ROTATE_VIEW_POINT,dx,dy);
    }
    else if (event->buttons() & Qt::MidButton)
    {
        ssl->g->cameraMotion(CameraMotionMode::MOVE_POSITION_LR,dx,dy);
    }
    lastPos = event->pos();
    update3DCursor(event->x(),event->y());
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

dReal GLWidget::getFPS()
{
    return m_fps;
}


void GLWidget::initializeGL ()
{    
    ssl->glinit();
}

void GLWidget::step()
{
    const dReal* ballV = dBodyGetLinearVel(ssl->ball->body);
    double ballSpeed = ballV[0]*ballV[0] + ballV[1]*ballV[1] + ballV[2]*ballV[2];
    ballSpeed  = sqrt(ballSpeed);
    rendertimer.restart();
    m_fps = frames /(time.elapsed()/1000.0);
    if (!(frames % ((int)(ceil(cfg->DesiredFPS()))))) {
        time.restart();
        frames = 0;
    }
    if (first_time) {ssl->step();first_time = false;}
    else {
        if (cfg->SyncWithGL())
        {
            dReal ddt=rendertimer.elapsed()/1000.0;
            if (ddt>0.05) ddt=0.05;
            ssl->step(ddt);
        }
        else {
            ssl->step(cfg->DeltaTime());
        }
    }
    frames ++;
}

void GLWidget::paintGL()
{
    if (!ssl->g->isGraphicsEnabled()) return;
    if (cammode==CameraMode::CURRENT_ROBOT_VIEW)
    {
        dReal x,y,z;
        int R = ssl->robotIndex(Current_robot,Current_team);
        ssl->robots[R]->getXY(x,y);z = 0.3;
        ssl->g->setViewpoint(x,y,z,ssl->robots[R]->getDir(),-25,0);
    }
    if (cammode==CameraMode::LOCK_TO_ROBOT)
    {
        dReal x,y,z;
        ssl->robots[lockedIndex]->getXY(x,y);z = 0.1;
        ssl->g->lookAt(x,y,z);
    }
    else if(cammode==CameraMode::LOCK_TO_BALL)
    {
        dReal x,y,z;
        ssl->ball->getBodyPosition(x,y,z);
        ssl->g->lookAt(x,y,z);
    }
    step();    
    QFont font;
    for (int i=0;i< cfg->Robots_Count()*2;i++)
    {
        dReal xx,yy;
        ssl->robots[i]->getXY(xx,yy);
        if (i>=cfg->Robots_Count()) qglColor(Qt::yellow);
        else qglColor(Qt::cyan);
        renderText(xx,yy,0.3,QString::number(i%cfg->Robots_Count()),font);
        if (!ssl->robots[i]->on){
            qglColor(Qt::red);
            font.setBold(true);
            renderText(xx,yy,0.4,"Off",font);
        }
        font.setBold(false);
    }
}

void GLWidget::changeCameraMode()
{
    static dReal xyz[3],hpr[3];
    if(static_cast<int>(cammode)<0) cammode=CameraMode::BIRDS_EYE_FROM_TOUCH_LINE;
    cammode = static_cast<CameraMode>(static_cast<int>(cammode) + 1);
    cammode = static_cast<CameraMode>(static_cast<int>(cammode)%(static_cast<int>(CameraMode::MAX_ACTIVE_MODE_FOR_CHANGEMODE)+1));

    if (cammode==CameraMode::BIRDS_EYE_FROM_TOUCH_LINE)
        ssl->g->setViewpoint(0,-(cfg->Field_Width()+cfg->Field_Margin()*2.0f)/2.0f,3,90,-45,0);
    else if (cammode==CameraMode::CURRENT_ROBOT_VIEW)
        ssl->g->getViewpoint(xyz,hpr);
    else if (cammode==CameraMode::TOP_VIEW)
        ssl->g->setViewpoint(0,0,5,0,-90,0);
    else if (cammode==CameraMode::BIRDS_EYE_FROM_OPPOSITE_TOUCH_LINE)
        ssl->g->setViewpoint(0, (cfg->Field_Width()+cfg->Field_Margin()*2.0f)/2.0f,3,270,-45,0);
    else if (cammode==CameraMode::BIRDS_EYE_FROM_BLUE)
        ssl->g->setViewpoint(-(cfg->Field_Length()+cfg->Field_Margin()*2.0f)/2.0f,0,3,0,-45,0);
    else if (cammode==CameraMode::BIRDS_EYE_FROM_YELLOW)
        ssl->g->setViewpoint((cfg->Field_Length()+cfg->Field_Margin()*2.0f)/2.0f,0,3,180,-45,0);
}

void GLWidget::putBall(dReal x,dReal y)
{
    ssl->ball->setBodyPosition(x,y,0.3);
    dBodySetLinearVel(ssl->ball->body,0,0,0);
    dBodySetAngularVel(ssl->ball->body,0,0,0);
}

void GLWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) ctrl = false;
    if (event->key() == Qt::Key_Alt) {
        if (!ssl->show3DCursor)
            moveCurrentRobot();
        else {
            unselectRobot();
        }
        alt = false;
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) ctrl = true;
    if (event->key() == Qt::Key_Alt) alt = true;
    char cmd = static_cast<char>(event->key());
    const dReal S = 1.00;
    const dReal BallForce = 2.0;
    int R = ssl->robotIndex(Current_robot,Current_team);
    if (R < 0) return;

    switch (cmd) {
    case 't': case 'T': ssl->robots[R]->incSpeed(0,-S);ssl->robots[R]->incSpeed(1,S);ssl->robots[R]->incSpeed(2,-S);ssl->robots[R]->incSpeed(3,S);break;
    case 'g': case 'G': ssl->robots[R]->incSpeed(0,S);ssl->robots[R]->incSpeed(1,-S);ssl->robots[R]->incSpeed(2,S);ssl->robots[R]->incSpeed(3,-S);break;
    case 'f': case 'F': ssl->robots[R]->incSpeed(0,S);ssl->robots[R]->incSpeed(1,S);ssl->robots[R]->incSpeed(2,S);ssl->robots[R]->incSpeed(3,S);break;
    case 'h': case 'H': ssl->robots[R]->incSpeed(0,-S);ssl->robots[R]->incSpeed(1,-S);ssl->robots[R]->incSpeed(2,-S);ssl->robots[R]->incSpeed(3,-S);break;
    case 'w': case 'W':dBodyAddForce(ssl->ball->body,0, BallForce,0);break;
    case 's': case 'S':dBodyAddForce(ssl->ball->body,0,-BallForce,0);break;
    case 'd': case 'D':dBodyAddForce(ssl->ball->body, BallForce,0,0);break;
    case 'a': case 'A':dBodyAddForce(ssl->ball->body,-BallForce,0,0);break;
    case 'k': case 'K': ssl->robots[R]->kicker->kick(8,0);break;
    case 'l': case 'L': ssl->robots[R]->kicker->kick(3,3);break;
    case 'j': case 'J': ssl->robots[R]->kicker->toggleRoller();break;
    case 'i': case 'I': dBodySetLinearVel(ssl->ball->body,2.0,0,0);dBodySetAngularVel(ssl->ball->body,0,2.0/cfg->BallRadius(),0);break;
    case ';':
        if (!kickingball)
            logStatus(QString("Kick mode On"),QColor("blue"));
        else
            logStatus(QString("Kick mode Off"),QColor("red"));
        kickingball=!kickingball;
        chiping=false;
        break;
    case '\'':
        if (!chiping)
            logStatus(QString("Chip mode On"),QColor("blue"));
        else
            logStatus(QString("Chip mode Off"),QColor("red"));
        chiping=!chiping;
        kickingball=false;
        break;
    case ']': kickpower += 0.1; logStatus(QString("Kick power = %1").arg(kickpower),QColor("orange"));break;
    case '[': kickpower -= 0.1; logStatus(QString("Kick power = %1").arg(kickpower),QColor("cyan"));break;
    case ' ':
        ssl->robots[R]->resetSpeeds();
        break;
    case '`':
        dBodySetLinearVel(ssl->ball->body,0,0,0);
        dBodySetAngularVel(ssl->ball->body,0,0,0);
        break;
        default:break;
    }
}

void GLWidget::closeEvent(QCloseEvent *event)
{
    emit closeSignal(false);
}

void GLWidget::blueRobotsMenuTriggered(QAction *act)
{
    reform(0,act->text());
}

void GLWidget::yellowRobotsMenuTriggered(QAction *act)
{
    reform(1,act->text());
}

void GLWidget::reform(int team,const QString& act)
{
    if (act==tr("Put all inside with formation 1")) forms[FORMATION_INSIDE_1]->resetRobots(ssl->robots, team);
    if (act==tr("Put all inside with formation 2")) forms[FORMATION_INSIDE_2]->resetRobots(ssl->robots,team);
    if (act==tr("Put all outside")) forms[FORMATION_OUTSIDE]->resetRobots(ssl->robots,team);
    if (act==tr("Put all out of field")) forms[FORMATION_OUTSIDE_FIELD]->resetRobots(ssl->robots,team);

    if(act==tr("Turn all off")) {
        for(int i=0; i<cfg->Robots_Count(); i++) {
            int k = ssl->robotIndex(i, team);
            if(ssl->robots[k]->on) {
                ssl->robots[k]->on = false;
                onOffRobotAct->setText("Turn &on");
                emit robotTurnedOnOff(k, false);
            }
        }
    }

    if(act==tr("Turn all on")) {
        for(int i=0; i<cfg->Robots_Count(); i++) {
            int k = ssl->robotIndex(i, team);
            if(!ssl->robots[k]->on) {
                ssl->robots[k]->on = true;
                onOffRobotAct->setText("Turn &off");
                emit robotTurnedOnOff(k, true);
            }
        }
    }

}

void GLWidget::moveBallHere()
{
    ssl->ball->setBodyPosition(ssl->cursor_x,ssl->cursor_y,cfg->BallRadius()*2);
    dBodySetLinearVel(ssl->ball->body, 0.0, 0.0, 0.0);
    dBodySetAngularVel(ssl->ball->body, 0.0, 0.0, 0.0);

}

void GLWidget::lockCameraToRobot()
{
    cammode = CameraMode::LOCK_TO_ROBOT;
    lockedIndex = ssl->robotIndex(Current_robot,Current_team);//clicked_robot;
}

void GLWidget::lockCameraToBall()
{
    cammode = CameraMode::LOCK_TO_BALL;
}

void GLWidget::moveRobotHere()
{
    ssl->robots[ssl->robotIndex(Current_robot,Current_team)]->setXY(ssl->cursor_x,ssl->cursor_y);
    ssl->robots[ssl->robotIndex(Current_robot,Current_team)]->resetRobot();
}
