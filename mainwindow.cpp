#include <QtGui>

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QTimer>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    widget = new Widget();
    QTimer *timer = new QTimer(this);
    fpslabel = new QLabel(this);
    robotlabel = new QLabel(this);

    timer->setInterval(_RENDER_INTERVAL);

    QGroupBox * groupBox = new QGroupBox(this);
    setCentralWidget(groupBox);
    groupBox->setTitle("Simulator");

    fpslabel->setAlignment(Qt::AlignCenter);
    robotlabel->setAlignment(Qt::AlignLeft);

    QGridLayout *layout = new QGridLayout(groupBox);

    layout->addWidget(widget,0,0,15,1);
    layout->addWidget(fpslabel,15,0);
    layout->addWidget(robotlabel,15,0);

    groupBox->setLayout(layout);

    QMenu *fileMenu = new QMenu("File");
    menuBar()->addMenu(fileMenu);
    QAction *exit = new QAction("Exit", fileMenu);

    fileMenu->addAction(exit);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));

    //QObject::connect(


    timer->start();


    this->setWindowState(Qt::WindowMaximized);

    this->resize(QSize(800,600));

    robotlabel->setText("Current robot: [Blue:1]");

}

MainWindow::~MainWindow()
{

}

void MainWindow::update()
{
    widget->updateGL();
    fpslabel->setText(QString("%1 fps").arg(widget->getFPS()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    char cmd = event->key();
    static float xyz[3],hpr[3];
  const float S = 0.3;
  const dReal* v;
  int RR = widget->Current_robot;  
  int T  = widget->Current_team;
  int R = RR + T*5;
  bool flag=false;  
  switch (cmd) {
  case 's': case 'S':widget->ssl->robots[R]->incSpeed(0,S);widget->ssl->robots[R]->incSpeed(1,S);widget->ssl->robots[R]->incSpeed(2,-S);widget->ssl->robots[R]->incSpeed(3,-S);break;
  case 'w': case 'W':widget->ssl->robots[R]->incSpeed(0,-S);widget->ssl->robots[R]->incSpeed(1,-S);widget->ssl->robots[R]->incSpeed(2,S);widget->ssl->robots[R]->incSpeed(3,S);break;
  case 'a': case 'A':widget->ssl->robots[R]->incSpeed(0,S);widget->ssl->robots[R]->incSpeed(1,S);widget->ssl->robots[R]->incSpeed(2,S);widget->ssl->robots[R]->incSpeed(3,S);break;
  case 'd': case 'D':widget->ssl->robots[R]->incSpeed(0,-S);widget->ssl->robots[R]->incSpeed(1,-S);widget->ssl->robots[R]->incSpeed(2,-S);widget->ssl->robots[R]->incSpeed(3,-S);break;
  case 'e': case 'E':widget->ssl->robots[R]->incSpeed(0,-S);widget->ssl->robots[R]->incSpeed(2,S);break;
  case 'q': case 'Q':widget->ssl->robots[R]->incSpeed(1,-S);widget->ssl->robots[R]->incSpeed(3,S);break;
  case 'n': case 'N':widget->ssl->g->setViewpoint(0,0,2,0,-90,180);break;
  case 't': case 'T':dBodyAddForce(widget->ssl->ball->body,0, 0.05,0);break;
  case 'g': case 'G':dBodyAddForce(widget->ssl->ball->body,0,-0.05,0);break;
  case 'h': case 'H':dBodyAddForce(widget->ssl->ball->body, 0.05,0,0);break;
  case 'f': case 'F':dBodyAddForce(widget->ssl->ball->body,-0.05,0,0);break;
  case 'c': case 'C':
              widget->cammode ++;
              widget->cammode %= 2;
              if (widget->cammode==0)
                widget->ssl->g->setViewpoint(xyz,hpr);
              else if (widget->cammode==1)
              {
                  widget->ssl->g->getViewpoint(xyz,hpr);
              }
              break;
  case '0': widget->ssl->ball->setBodyPosition(0,0,0.3);dBodySetLinearVel(widget->ssl->ball->body,0,0,0);break;
  case '1': RR = 0;flag=true;widget->Current_robot = RR;break;
  case '2': RR = 1;flag=true;widget->Current_robot = RR;break;
  case '3': RR = 2;flag=true;widget->Current_robot = RR;break;
  case '4': RR = 3;flag=true;widget->Current_robot = RR;break;
  case '5': RR = 4;flag=true;widget->Current_robot = RR;break;
  case 'y': case 'Y': T=1;flag=true;widget->Current_team = T; break;
  case 'b': case 'B': T=0;flag=true;widget->Current_team = T; break;
  case 'r':case 'R': widget->ssl->robots[R]->resetRobot();break;
  case 'k':case 'K': widget->ssl->robots[R]->kicker->kick(10);break;
  case 'l':case 'L': widget->ssl->robots[R]->kicker->kick(10,true);break;
  case 'j':case 'J': widget->ssl->robots[R]->kicker->toggleRoller();break;
  case '`':
//         v = dBodyGetLinearVel(body[robots[R]->getBodyIndex()]);
//         printf("Velocity of robot(%d) : %f\n",R,sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])/DT);break;
  case ' ':
    widget->ssl->robots[R]->resetSpeeds();
    dBodySetLinearVel(widget->ssl->ball->body,0,0,0);
    break;

  }
  if (flag)
  {
      if (T==0)
        robotlabel->setText(QString("Current robot: [Blue:%1]").arg(RR+1));
      else
        robotlabel->setText(QString("Current robot: [Yellow:%1]").arg(RR+1));
  }
}
