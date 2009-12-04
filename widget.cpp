#include <QtGui>

#include <QPainter>
#include "widget.h"
#include "Physics/pball.h"
#include "Physics/pground.h"

#include <QDebug>

Widget::Widget(QWidget *parent)
    : QGLWidget(parent)
{
    frames = 0;
    state = 0;
    first_time = true;
    ssl = new SSLWorld(this);
    Current_robot = 0;
    Current_team = 0;
    cammode = 0;
    setMouseTracking(true);
    robpopup = new QMenu(this);
    moveRobotAct = new QAction(tr("&Move robot"),this);
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
}

Widget::~Widget()
{
}

void Widget::moveRobot()
{
    ssl->show3DCursur = true;
    state = 1;
    moving_robot_id = clicked_robot;
}

void Widget::selectRobot()
{
    if (clicked_robot!=-1)
    {
        Current_robot = ssl->selected%5;
        Current_team = ssl->selected/5;
        emit selectedRobot();
    }
}

void Widget::resetRobot()
{
    if (clicked_robot!=-1)
    {
        ssl->robots[clicked_robot]->resetRobot();
    }
}

void Widget::moveBall()
{
    ssl->show3DCursur = true;
    state = 2;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton)
    {
        if (state==1)
        {
            if (moving_robot_id!=-1)
            {
                ssl->robots[moving_robot_id]->setXY(ssl->cursur_x,ssl->cursur_y);
                state = 0;
                ssl->show3DCursur = false;
            }
        }
        if (state==2)
        {
            ssl->ball->setBodyPosition(ssl->cursur_x,ssl->cursur_y,0.2);
            dBodySetAngularVel(ssl->ball->body,0,0,0);
            dBodySetLinearVel(ssl->ball->body,0,0,0);
            ssl->show3DCursur = false;
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

void Widget::wheelEvent(QWheelEvent *event)
{
    ssl->g->zoomCamera(-event->delta()*0.002);
    update3DCursur(event->x(),event->y());
}

void Widget::update3DCursur(int mouse_x,int mouse_y)
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

void Widget::mouseMoveEvent(QMouseEvent *event)
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
    update3DCursur(event->x(),event->y());
}

void Widget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

QString Widget::getFPS()
{
    return m_fps;
}

QImage* createBlob(char yb,int i)
{    
    QImage* img = new QImage(QString(":/Graphics/%1%2").arg(yb).arg(i+1)+QString(".bmp"));
    QPainter *p = new QPainter();
    p->begin(img);
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(3);
    pen.setBrush(Qt::gray);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p->setPen(pen);
    QFont f;
    f.setPointSize(35);
    p->setFont(f);
    p->drawText(img->width()/2-15,img->height()/2-15,30,30,Qt::AlignCenter,QString("%1").arg(i));
    p->end();
    delete p;
    return img;
}

void Widget::initializeGL ()
{
    ssl->g->loadTexture(new QImage(":/Graphics/grass001.bmp"));
    ssl->g->loadTexture(createBlob('b',0));
    ssl->g->loadTexture(createBlob('b',1));
    ssl->g->loadTexture(createBlob('b',2));
    ssl->g->loadTexture(createBlob('b',3));
    ssl->g->loadTexture(createBlob('b',4));
    ssl->g->loadTexture(createBlob('y',0));
    ssl->g->loadTexture(createBlob('y',1));
    ssl->g->loadTexture(createBlob('y',2));
    ssl->g->loadTexture(createBlob('y',3));
    ssl->g->loadTexture(createBlob('y',4));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/neg_x.bmp"));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/pos_x.bmp"));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/neg_y.bmp"));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/pos_y.bmp"));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/neg_z.bmp"));
    ssl->g->loadTextureSkyBox(new QImage("../Graphics/sky/pos_z.bmp"));
    //pos_y neg_x neg_y pos_x pos_z neg_z
    ssl->p->glinit();
}

void Widget::paintGL()
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
        float ddt=rendertimer.elapsed()/1000.0;
        ssl->step(ddt);
    }
    rendertimer.restart();
    m_fps.setNum(frames /(time.elapsed()/1000.0), 'f', 2);

    if (!(frames % (1000/_RENDER_INTERVAL))) {
        time.start();
        frames = 0;
    }

    frames ++;
}
