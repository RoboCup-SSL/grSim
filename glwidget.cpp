#include <QtGui>

#include <QPainter>
#include "glwidget.h"
#include "Physics/pball.h"
#include "Physics/pground.h"

#include <QDebug>
#include <QLabel>

GLWidget::GLWidget(QWidget *parent,ConfigWidget* _cfg)
    : QGLWidget(parent)
{
    frames = 0;
    state = 0;
    first_time = true;
    cfg = _cfg;
    forms[0] = new RobotsFomation(-1);  //outside
    forms[1] = new RobotsFomation(1);  //inside type 1
    forms[2] = new RobotsFomation(2);  //inside type 2
    ssl = new SSLWorld(this,cfg,forms[1],forms[1]);
    Current_robot = 0;
    Current_team = 0;
    cammode = 0;
    setMouseTracking(true);

    blueRobotsMenu = new QMenu("&Blue Robots");
    yellowRobotsMenu = new QMenu("&Yellow Robots");
    blueRobotsMenu->addAction(tr("Put all inside with formation 1"));
    blueRobotsMenu->addAction(tr("Put all inside with formation 2"));
    blueRobotsMenu->addAction(tr("Put all outside"));
    yellowRobotsMenu->addAction(tr("Put all inside with formation 1"));
    yellowRobotsMenu->addAction(tr("Put all inside with formation 2"));
    yellowRobotsMenu->addAction(tr("Put all outside"));

    robpopup = new QMenu(this);
    moveRobotAct = new QAction(tr("&Locate robot"),this);
    selectRobotAct = new QAction(tr("&Select robot"),this);
    resetRobotAct = new QAction(tr("&Reset robot"),this);
    onOffRobotAct = new QAction(tr("Turn &off"),this);
    robpopup->addAction(selectRobotAct);
    robpopup->addAction(moveRobotAct);
    robpopup->addAction(resetRobotAct);
    robpopup->addAction(onOffRobotAct);
    robpopup->addMenu(blueRobotsMenu);
    robpopup->addMenu(yellowRobotsMenu);

    moveBallAct = new QAction(tr("&Move ball"),this);
    ballpopup = new QMenu(this);
    ballpopup->addAction(moveBallAct);

    mainpopup = new QMenu(this);
    mainpopup->addMenu(blueRobotsMenu);
    mainpopup->addMenu(yellowRobotsMenu);

    connect(moveRobotAct, SIGNAL(triggered()), this, SLOT(moveRobot()));
    connect(selectRobotAct, SIGNAL(triggered()), this, SLOT(selectRobot()));
    connect(resetRobotAct, SIGNAL(triggered()), this, SLOT(resetRobot()));
    connect(moveBallAct, SIGNAL(triggered()), this, SLOT(moveBall()));
    connect(onOffRobotAct, SIGNAL(triggered()), this, SLOT(switchRobotOnOff()));
    connect(yellowRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(yellowRobotsMenuTriggered(QAction*)));
    connect(blueRobotsMenu,SIGNAL(triggered(QAction*)),this,SLOT(blueRobotsMenuTriggered(QAction*)));

    setFocusPolicy(Qt::StrongFocus);
    fullScreen = false;
    ctrl = false;
}

GLWidget::~GLWidget()
{
}

void GLWidget::moveRobot()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->CHASSISWIDTH()*0.5f;
    state = 1;
    moving_robot_id = clicked_robot;
}

void GLWidget::selectRobot()
{
    if (clicked_robot!=-1)
    {
        Current_robot = clicked_robot%5;
        Current_team = clicked_robot/5;
        emit selectedRobot();
    }
}

void GLWidget::resetRobot()
{
    if (clicked_robot!=-1)
    {
        ssl->robots[clicked_robot]->resetRobot();
    }
}

void GLWidget::switchRobotOnOff()
{
    if (clicked_robot!=-1)
    {
        if (ssl->robots[clicked_robot]->on==true)
        {
            ssl->robots[clicked_robot]->on = false;
            onOffRobotAct->setText("Turn &on");
            emit robotTurnedOnOff(clicked_robot,false);
        }
        else {
            ssl->robots[clicked_robot]->on = true;
            onOffRobotAct->setText("Turn &off");
            emit robotTurnedOnOff(clicked_robot,true);
        }
    }
}

void GLWidget::resetCurrentRobot()
{       
    ssl->robots[robotIndex(Current_robot,Current_team)]->resetRobot();
}

void GLWidget::moveCurrentRobot()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->CHASSISWIDTH()*0.5f;
    state = 1;
    moving_robot_id = robotIndex(Current_robot,Current_team);
}

void GLWidget::moveBall()
{
    ssl->show3DCursor = true;
    ssl->cursor_radius = cfg->BALLRADIUS();
    state = 2;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if (event->buttons() & Qt::LeftButton)
    {
        if (state==1)
        {
            if (moving_robot_id!=-1)
            {
                ssl->robots[moving_robot_id]->setXY(ssl->cursor_x,ssl->cursor_y);
                state = 0;
                ssl->show3DCursor = false;
            }
        }
        if (state==2)
        {
            ssl->ball->setBodyPosition(ssl->cursor_x,ssl->cursor_y,cfg->BALLRADIUS()*1.1);
            dBodySetAngularVel(ssl->ball->body,0,0,0);
            dBodySetLinearVel(ssl->ball->body,0,0,0);
            ssl->show3DCursor = false;
            state = 0;
        }
    }
    if (event->buttons() & Qt::RightButton)
    {
        if (ssl->selected!=-1 && ssl->selected!=-2)
        {
            clicked_robot = ssl->selected;
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
    ssl->g->zoomCamera(-event->delta()*0.002);
    update3DCursor(event->x(),event->y());
}

void GLWidget::update3DCursor(int mouse_x,int mouse_y)
{
    float xyz[3],hpr[3],fx,fy,fz,rx,ry,rz,ux,uy,uz,px,py,pz;
    ssl->g->getViewpoint(xyz,hpr);
    ssl->g->getCameraForward(fx,fy,fz);
    ssl->g->getCameraRight(rx,ry,rz);
    ////u = r*f ///Up
    ux = ry*fz - rz*fy;
    uy = rz*fx - rx*fz;
    uz = rx*fy - ry*fx;
    float w = width();
    float h = height();
    float xx,yy,z;
    float x = 1.0f - 2.0f*(float) mouse_x / w;
    float y = 1.0f - 2.0f*(float) mouse_y / h;
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
    int dx = -(event->x() - lastPos.x());
    int dy = -(event->y() - lastPos.y());

    if (event->buttons() & Qt::LeftButton) {
        if (ctrl)
            ssl->g->cameraMotion(4,dx,dy);
        else
            ssl->g->cameraMotion(1,dx,dy);
    }
    else if (event->buttons() & Qt::MidButton)
    {
        ssl->g->cameraMotion(2,dx,dy);
    }
    lastPos = event->pos();
    update3DCursor(event->x(),event->y());
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

QString GLWidget::getFPS()
{
    return m_fps;
}


void GLWidget::initializeGL ()
{
    ssl->glinit();
}


void GLWidget::paintGL()
{    
    if (cammode==1)
    {
        float x,y,z;
        int R = Current_robot+Current_team*5;
        ssl->robots[R]->getXY(x,y);z = 0.3;
        ssl->g->setViewpoint(x,y,z,ssl->robots[R]->getDir(),-25,0);
    }
    if (first_time) {ssl->step();first_time = false;}
    else {
        if (cfg->SyncWithGL())
        {
            float ddt=rendertimer.elapsed()/1000.0;
            if (ddt>0.05) ddt=0.05;
            ssl->step(ddt);
        }
        else {
            ssl->step(cfg->DeltaTime());
        }
    }
    rendertimer.restart();
    m_fps.setNum(frames /(time.elapsed()/1000.0), 'f', 2);

    if (!(frames % (1000/_RENDER_INTERVAL))) {
        time.restart();
        frames = 0;
    }

    frames ++;

    /*
    QPainter painter;
    painter.begin(this);

    painter.drawLine(0,0,100,100);
    painter.end();
*/
}

void GLWidget::changeCameraMode()
{
    static float xyz[3],hpr[3];
      this->cammode ++;
      this->cammode %= 3;
      if (this->cammode==0)
        this->ssl->g->setViewpoint(xyz,hpr);
      else if (this->cammode==1)
      {
          this->ssl->g->getViewpoint(xyz,hpr);
      }
      else this->ssl->g->setViewpoint(0,0,5,0,-90,0);
}

void GLWidget::putBall(float x,float y)
{
    ssl->ball->setBodyPosition(x,y,0.3);
    dBodySetLinearVel(ssl->ball->body,0,0,0);
    dBodySetAngularVel(ssl->ball->body,0,0,0);
}

void GLWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) ctrl = false;
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Control) ctrl = true;
  char cmd = event->key();
  if (fullScreen) {
      if (event->key()==Qt::Key_F2) emit toggleFullScreen(false);
  }
  const float S = 0.30;
  const float BallForce = 0.2;
  int R = robotIndex(Current_robot,Current_team);
  bool flag=false;
  switch (cmd) {
      case 't': case 'T': this->ssl->robots[R]->incSpeed(0,-S);this->ssl->robots[R]->incSpeed(1,S);this->ssl->robots[R]->incSpeed(2,-S);this->ssl->robots[R]->incSpeed(3,S);break;
      case 'g': case 'G': this->ssl->robots[R]->incSpeed(0,S);this->ssl->robots[R]->incSpeed(1,-S);this->ssl->robots[R]->incSpeed(2,S);this->ssl->robots[R]->incSpeed(3,-S);break;
      case 'f': case 'F': this->ssl->robots[R]->incSpeed(0,S);this->ssl->robots[R]->incSpeed(1,S);this->ssl->robots[R]->incSpeed(2,S);this->ssl->robots[R]->incSpeed(3,S);break;
      case 'h': case 'H': this->ssl->robots[R]->incSpeed(0,-S);this->ssl->robots[R]->incSpeed(1,-S);this->ssl->robots[R]->incSpeed(2,-S);this->ssl->robots[R]->incSpeed(3,-S);break;
/*
      case 't': case 'T': this->ssl->robots[R]->setSpeed(0,-S);this->ssl->robots[R]->setSpeed(1,S);this->ssl->robots[R]->setSpeed(2,-S);this->ssl->robots[R]->setSpeed(3,S);break;
      case 'g': case 'G': this->ssl->robots[R]->setSpeed(0,S);this->ssl->robots[R]->setSpeed(1,-S);this->ssl->robots[R]->setSpeed(2,S);this->ssl->robots[R]->setSpeed(3,-S);break;
      case 'f': case 'F': this->ssl->robots[R]->setSpeed(0,S);this->ssl->robots[R]->setSpeed(1,S);this->ssl->robots[R]->setSpeed(2,S);this->ssl->robots[R]->setSpeed(3,S);break;
      case 'h': case 'H': this->ssl->robots[R]->setSpeed(0,-S);this->ssl->robots[R]->setSpeed(1,-S);this->ssl->robots[R]->setSpeed(2,-S);this->ssl->robots[R]->setSpeed(3,-S);break;
*/
  case 'w': case 'W':dBodyAddForce(this->ssl->ball->body,0, BallForce,0);break;
  case 's': case 'S':dBodyAddForce(this->ssl->ball->body,0,-BallForce,0);break;
  case 'd': case 'D':dBodyAddForce(this->ssl->ball->body, BallForce,0,0);break;
  case 'a': case 'A':dBodyAddForce(this->ssl->ball->body,-BallForce,0,0);break;
  case 'k':case 'K': this->ssl->robots[R]->kicker->kick(10);break;
  case 'l':case 'L': this->ssl->robots[R]->kicker->kick(10,true);break;
  case 'j':case 'J': this->ssl->robots[R]->kicker->toggleRoller();break;  
  case ' ':
    this->ssl->robots[R]->resetSpeeds();
    break;
  case '`':
    dBodySetLinearVel(this->ssl->ball->body,0,0,0);
    dBodySetAngularVel(this->ssl->ball->body,0,0,0);
    break;
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
    if (act==tr("Put all inside with formation 1")) forms[1]->resetRobots(ssl->robots,team);
    if (act==tr("Put all inside with formation 2")) forms[2]->resetRobots(ssl->robots,team);
    if (act==tr("Put all outside")) forms[0]->resetRobots(ssl->robots,team);    
}

GLWidgetGraphicsView::GLWidgetGraphicsView(QGraphicsScene *scene,GLWidget *_glwidget)
        : QGraphicsView(scene)
{
    glwidget = _glwidget;
}

void GLWidgetGraphicsView::mousePressEvent(QMouseEvent *event) {glwidget->mousePressEvent(event);}
void GLWidgetGraphicsView::mouseMoveEvent(QMouseEvent *event) {glwidget->mouseMoveEvent(event);}
void GLWidgetGraphicsView::mouseReleaseEvent(QMouseEvent *event) {glwidget->mouseReleaseEvent(event);}
void GLWidgetGraphicsView::wheelEvent(QWheelEvent *event) {glwidget->wheelEvent(event);}
void GLWidgetGraphicsView::keyPressEvent(QKeyEvent *event){glwidget->keyPressEvent(event);}
void GLWidgetGraphicsView::closeEvent(QCloseEvent *event){} //{this->viewportEvent(event);}

