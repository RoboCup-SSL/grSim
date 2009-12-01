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
    ssl = new SSLWorld(this);
    Current_robot = 0;
    Current_team = 0;
    cammode = 0;
}

Widget::~Widget()
{
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    ssl->g->zoomCamera(event->delta()*0.002);
}


void Widget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = -(event->x() - lastPos.x());
    int dy = -(event->y() - lastPos.y());

    if (event->buttons() & Qt::LeftButton) {
        ssl->g->cameraMotion(1,dx,dy);
    } else if (event->buttons() & Qt::RightButton) {
        ssl->g->cameraMotion(4,dx,dy);
    }
    else if (event->buttons() & Qt::MidButton)
    {
        ssl->g->cameraMotion(2,dx,dy);
    }
    lastPos = event->pos();
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
    QImage* img = new QImage(QString(":/Graphics/%1%2").arg(yb).arg(i)+QString(".bmp"));
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
    ssl->g->loadTexture(createBlob('b',1));
    ssl->g->loadTexture(createBlob('b',2));
    ssl->g->loadTexture(createBlob('b',3));
    ssl->g->loadTexture(createBlob('b',4));
    ssl->g->loadTexture(createBlob('b',5));
    ssl->g->loadTexture(createBlob('y',1));
    ssl->g->loadTexture(createBlob('y',2));
    ssl->g->loadTexture(createBlob('y',3));
    ssl->g->loadTexture(createBlob('y',4));
    ssl->g->loadTexture(createBlob('y',5));
    ssl->g->loadTexture(new QImage("./Graphics/sky/neg_x.bmp"));
    ssl->g->loadTexture(new QImage("./Graphics/sky/pos_x.bmp"));
    ssl->g->loadTexture(new QImage("./Graphics/sky/neg_y.bmp"));
    ssl->g->loadTexture(new QImage("./Graphics/sky/pos_y.bmp"));
    ssl->g->loadTexture(new QImage("./Graphics/sky/neg_z.bmp"));
    ssl->g->loadTexture(new QImage("./Graphics/sky/pos_z.bmp"));
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
    ssl->step();

    m_fps.setNum(frames /(time.elapsed() / 1000.0), 'f', 2);

    if (!(frames % (1000/_RENDER_INTERVAL))) {
        time.start();
        frames = 0;
    }

    frames ++;
}
