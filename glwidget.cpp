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
    ssl = new SSLWorld(this,cfg);
    Current_robot = 0;
    Current_team = 0;
    cammode = 0;
    setMouseTracking(true);
    robpopup = new QMenu(this);
    moveRobotAct = new QAction(tr("&Locate robot"),this);
    selectRobotAct = new QAction(tr("&Select robot"),this);
    resetRobotAct = new QAction(tr("&Reset robot"),this);
    robpopup->addAction(moveRobotAct);
    robpopup->addAction(selectRobotAct);
    robpopup->addAction(resetRobotAct);
    moveBallAct = new QAction(tr("&Move ball"),this);
    ballpopup = new QMenu(this);
    ballpopup->addAction(moveBallAct);
    connect(moveRobotAct, SIGNAL(triggered()), this, SLOT(moveRobot()));
    connect(selectRobotAct, SIGNAL(triggered()), this, SLOT(selectRobot()));
    connect(resetRobotAct, SIGNAL(triggered()), this, SLOT(resetRobot()));
    connect(moveBallAct, SIGNAL(triggered()), this, SLOT(moveBall()));

    setFocusPolicy(Qt::StrongFocus);

}

GLWidget::~GLWidget()
{
}

void GLWidget::moveRobot()
{
    ssl->show3DCursor = true;
    state = 1;
    moving_robot_id = clicked_robot;
}

void GLWidget::selectRobot()
{
    if (clicked_robot!=-1)
    {
        Current_robot = ssl->selected%5;
        Current_team = ssl->selected/5;
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

void GLWidget::moveBall()
{
    ssl->show3DCursor = true;
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
        ssl->g->cameraMotion(1,dx,dy);
    }
    else if (event->buttons() & Qt::RightButton) {
        if (ssl->selected==-1) ssl->g->cameraMotion(4,dx,dy);
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

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    char cmd = event->key();
    static float xyz[3],hpr[3];
  const float S = 0.3;
  const float BallForce = 0.2;
  const dReal* v;
  int RR = this->Current_robot;
  int T  = this->Current_team;
  int R = RR + T*5;
  bool flag=false;
  switch (cmd) {
  case 'w': case 'W':dBodyAddForce(this->ssl->ball->body,0, BallForce,0);break;
  case 's': case 'S':dBodyAddForce(this->ssl->ball->body,0,-BallForce,0);break;
  case 'd': case 'D':dBodyAddForce(this->ssl->ball->body, BallForce,0,0);break;
  case 'a': case 'A':dBodyAddForce(this->ssl->ball->body,-BallForce,0,0);break;
  case 'c': case 'C':
              this->cammode ++;
              this->cammode %= 3;
              if (this->cammode==0)
                this->ssl->g->setViewpoint(xyz,hpr);
              else if (this->cammode==1)
              {
                  this->ssl->g->getViewpoint(xyz,hpr);
              }
              else this->ssl->g->setViewpoint(0,0,5,0,-90,0);break;
              break;
  case '-': this->ssl->ball->setBodyPosition(0,0,0.3);dBodySetLinearVel(this->ssl->ball->body,0,0,0);dBodySetAngularVel(this->ssl->ball->body,0,0,0);break;
  case '0': RR = 0;flag=true;this->Current_robot = RR;break;
  case '1': RR = 1;flag=true;this->Current_robot = RR;break;
  case '2': RR = 2;flag=true;this->Current_robot = RR;break;
  case '3': RR = 3;flag=true;this->Current_robot = RR;break;
  case '4': RR = 4;flag=true;this->Current_robot = RR;break;
  case 'y': case 'Y': T=1;flag=true;this->Current_team = T; break;
  case 'b': case 'B': T=0;flag=true;this->Current_team = T; break;
  case 'r':case 'R': this->ssl->robots[R]->resetRobot();break;
  case 'k':case 'K': this->ssl->robots[R]->kicker->kick(10);break;
  case 'l':case 'L': this->ssl->robots[R]->kicker->kick(10,true);break;
  case 'j':case 'J': this->ssl->robots[R]->kicker->toggleRoller();break;
  case '`':
//         v = dBodyGetLinearVel(body[robots[R]->getBodyIndex()]);
//         printf("Velocity of robot(%d) : %f\n",R,sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])/DT);break;
  case ' ':
    this->ssl->robots[R]->resetSpeeds();
    dBodySetLinearVel(this->ssl->ball->body,0,0,0);
    dBodySetAngularVel(this->ssl->ball->body,0,0,0);
    break;

  }
  if (flag)
  {
      //updateRobotLabel();
  }
}

void GLWidget::closeEvent(QCloseEvent *event)
{
    emit closeSignal(false);
}
