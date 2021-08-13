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

#include <QtGui>

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QTimer>
#include <QToolBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QDir>
#include <QClipboard>
#include <QShortcut>

#include <QStatusBar>
#include <QMessageBox>

#include "mainwindow.h"
#include "logger.h"

int MainWindow::getInterval()
{
    return ceil((1000.0f / configwidget->DesiredFPS()));
}

void MainWindow::customFPS(int fps)
{
    int k = ceil((1000.0f / fps));
    timer->setInterval(k);
    logStatus(QString("new FPS set by user: %1").arg(fps),"red");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QDir dir = qApp->applicationDirPath();
    dir.cdUp();
    current_dir = dir.path();
    /* Status Logger */
    printer = new CStatusPrinter();
    statusWidget = new CStatusWidget(printer);
    statusWidget->setObjectName("CStatusWidget");
    initLogger((void*)printer);

    /* Widgets */

    configwidget = new ConfigWidget();
    dockconfig = new ConfigDockWidget(this,configwidget);
    dockconfig->setObjectName("ConfigDockWidget");
    dockconfig->setWindowTitle("Configuration");

    glwidget = new GLWidget(this,configwidget);
    glwidget->setWindowTitle(tr("Simulator"));
    glwidget->resize(512,512);    

    visionServer = nullptr;
    commandSocket = new QUdpSocket(this);
    blueStatusSocket = nullptr;
    yellowStatusSocket = nullptr;
    simControlSocket = new QUdpSocket(this);
    blueControlSocket = new QUdpSocket(this);
    yellowControlSocket = new QUdpSocket(this);
    reconnectVisionSocket();
    reconnectCommandSocket();
    reconnectBlueStatusSocket();
    reconnectYellowStatusSocket();
    reconnectSimControlSocket();
    reconnectBlueControlSocket();
    reconnectYellowControlSocket();

    QObject::connect(commandSocket,SIGNAL(readyRead()),this,SLOT(recvActions()));
    QObject::connect(simControlSocket,SIGNAL(readyRead()),this,SLOT(simControlSocketReady()));
    QObject::connect(blueControlSocket,SIGNAL(readyRead()),this,SLOT(blueControlSocketReady()));
    QObject::connect(yellowControlSocket,SIGNAL(readyRead()),this,SLOT(yellowControlSocketReady()));
    
    glwidget->ssl->visionServer = visionServer;
    glwidget->ssl->commandSocket = commandSocket;
    glwidget->ssl->blueStatusSocket = blueStatusSocket;
    glwidget->ssl->yellowStatusSocket = yellowStatusSocket;
    glwidget->ssl->simControlSocket = simControlSocket;
    glwidget->ssl->blueControlSocket = blueControlSocket;
    glwidget->ssl->yellowControlSocket = yellowControlSocket;

    robotwidget = new RobotWidget(this, configwidget);
    robotwidget->setObjectName("RobotWidget");
    /* Status Bar */
    fpslabel = new QLabel(this);
    cursorlabel = new QLabel(this);
    selectinglabel = new QLabel(this);
    vanishlabel = new QLabel("Vanishing",this);
    noiselabel = new QLabel("Gaussian noise",this);
    fpslabel->setFrameStyle(QFrame::Panel);
    cursorlabel->setFrameStyle(QFrame::Panel);
    selectinglabel->setFrameStyle(QFrame::Panel);
    vanishlabel->setFrameStyle(QFrame::Panel);
    noiselabel->setFrameStyle(QFrame::Panel);
    statusBar()->addWidget(fpslabel);
    statusBar()->addWidget(cursorlabel);
    statusBar()->addWidget(selectinglabel);
    statusBar()->addWidget(vanishlabel);
    statusBar()->addWidget(noiselabel);
    /* Menus */

    QMenu *fileMenu = new QMenu("&File");
    menuBar()->addMenu(fileMenu);    
    QAction *takeSnapshotAct = new QAction("&Save snapshot to file", fileMenu);
    takeSnapshotAct->setShortcut(QKeySequence("F3"));
    QAction *takeSnapshotToClipboardAct = new QAction("&Copy snapshot to clipboard", fileMenu);
    takeSnapshotToClipboardAct->setShortcut(QKeySequence("F4"));    
    QAction *exit = new QAction("E&xit", fileMenu);
    exit->setShortcut(QKeySequence("Ctrl+X"));
    fileMenu->addAction(takeSnapshotAct);
    fileMenu->addAction(takeSnapshotToClipboardAct);
    fileMenu->addAction(exit);

    QObject::connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this),
                     &QShortcut::activated,
                     this,
                     &MainWindow::close);

    QMenu *viewMenu = new QMenu("&View");
    menuBar()->addMenu(viewMenu);
    showsimulator = new QAction("&Simulator", viewMenu);
    showsimulator->setCheckable(true);
    showsimulator->setChecked(true);
    viewMenu->addAction(showsimulator);
    showconfig = dockconfig->toggleViewAction();
    viewMenu->addAction(showconfig);

    QMenu *simulatorMenu = new QMenu("&Simulator");
    menuBar()->addMenu(simulatorMenu);
    QMenu *robotMenu = new QMenu("&Robot");
    QMenu *ballMenu = new QMenu("&Ball");
    simulatorMenu->addMenu(robotMenu);
    simulatorMenu->addMenu(ballMenu);

    QMenu *helpMenu = new QMenu("&Help");
    QAction* aboutMenu = new QAction("&About", helpMenu);
    menuBar()->addMenu(helpMenu);
    helpMenu->addAction(aboutMenu);

    ballMenu->addAction(tr("Put on Center"))->setShortcut(QKeySequence("Ctrl+0"));
    ballMenu->addAction(tr("Put on Corner 1"))->setShortcut(QKeySequence("Ctrl+1"));
    ballMenu->addAction(tr("Put on Corner 2"))->setShortcut(QKeySequence("Ctrl+2"));
    ballMenu->addAction(tr("Put on Corner 3"))->setShortcut(QKeySequence("Ctrl+3"));
    ballMenu->addAction(tr("Put on Corner 4"))->setShortcut(QKeySequence("Ctrl+4"));
    ballMenu->addAction(tr("Put on Penalty 1"))->setShortcut(QKeySequence("Ctrl+5"));
    ballMenu->addAction(tr("Put on Penalty 2"))->setShortcut(QKeySequence("Ctrl+6"));

    robotMenu->addMenu(glwidget->blueRobotsMenu);
    robotMenu->addMenu(glwidget->yellowRobotsMenu);
    robotMenu->addMenu(glwidget->allRobotsMenu);

    fullScreenAct = new QAction(tr("&Full screen"),simulatorMenu);
    fullScreenAct->setShortcut(QKeySequence("F2"));
    fullScreenAct->setCheckable(true);
    fullScreenAct->setChecked(false);
    simulatorMenu->addAction(fullScreenAct);

    showrobot = robotwidget->toggleViewAction();
    viewMenu->addAction(showrobot);
    viewMenu->addMenu(glwidget->cameraMenu);

    addDockWidget(Qt::LeftDockWidgetArea,dockconfig);
    addDockWidget(Qt::BottomDockWidgetArea, statusWidget);
    addDockWidget(Qt::LeftDockWidgetArea, robotwidget);

    setCentralWidget(glwidget);

    timer = new QTimer(this);
    timer->setInterval(getInterval());


    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(takeSnapshotAct, SIGNAL(triggered(bool)), this, SLOT(takeSnapshot()));
    QObject::connect(takeSnapshotToClipboardAct, SIGNAL(triggered(bool)), this, SLOT(takeSnapshotToClipboard()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(showsimulator, SIGNAL(triggered(bool)), this, SLOT(showHideSimulator(bool)));
    QObject::connect(glwidget, SIGNAL(closeSignal(bool)), this, SLOT(showHideSimulator(bool)));    
    QObject::connect(glwidget, SIGNAL(selectedRobot()), this, SLOT(updateRobotLabel()));
    QObject::connect(robotwidget->robotCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCurrentRobot()));
    QObject::connect(robotwidget->teamCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCurrentTeam()));
    QObject::connect(robotwidget->resetBtn,SIGNAL(clicked()),glwidget,SLOT(resetCurrentRobot()));
    QObject::connect(robotwidget->locateBtn,SIGNAL(clicked()),glwidget,SLOT(moveCurrentRobot()));
    QObject::connect(robotwidget->onOffBtn,SIGNAL(clicked()),glwidget,SLOT(switchRobotOnOff()));
    QObject::connect(robotwidget->getPoseWidget->okBtn,SIGNAL(clicked()),this,SLOT(setCurrentRobotPosition()));
    QObject::connect(glwidget,SIGNAL(robotTurnedOnOff(int,bool)),robotwidget,SLOT(changeRobotOnOff(int,bool)));
    QObject::connect(ballMenu,SIGNAL(triggered(QAction*)),this,SLOT(ballMenuTriggered(QAction*)));
    QObject::connect(fullScreenAct,SIGNAL(triggered(bool)),this,SLOT(toggleFullScreen(bool)));
    QObject::connect(glwidget->ssl, SIGNAL(fpsChanged(int)), this, SLOT(customFPS(int)));
    QObject::connect(aboutMenu, SIGNAL(triggered()), this, SLOT(showAbout()));
    //config related signals
    QObject::connect(configwidget->v_BallRadius.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_BallMass.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallMass()));
    QObject::connect(configwidget->v_BallFriction.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_BallSlip.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_BallBounce.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_BallBounceVel.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_BallLinearDamp.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallDamping()));
    QObject::connect(configwidget->v_BallAngularDamp.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeBallDamping()));
    QObject::connect(configwidget->v_Gravity.get(),  SIGNAL(wasEdited(VarPtr)), this, SLOT(changeGravity()));

    //geometry config vars
    QObject::connect(configwidget->v_DesiredFPS.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeTimer()));
    QObject::connect(configwidget->v_Division.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_Robots_Count.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(changeRobotCount()));

    QObject::connect(configwidget->v_DivA_Field_Line_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Length.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Rad.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Free_Kick.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Penalty_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Penalty_Depth.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Penalty_Point.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Margin.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Field_Referee_Margin.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Wall_Thickness.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Goal_Thickness.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Goal_Depth.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Goal_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivA_Goal_Height.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));

    QObject::connect(configwidget->v_DivB_Field_Line_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Length.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Rad.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Free_Kick.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Penalty_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Penalty_Depth.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Penalty_Point.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Margin.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Field_Referee_Margin.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Wall_Thickness.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Goal_Thickness.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Goal_Depth.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Goal_Width.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_DivB_Goal_Height.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));

    QObject::connect(configwidget->v_YellowTeam.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_BlueTeam.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));

    QObject::connect(configwidget->v_ColorRobotBlue.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));
    QObject::connect(configwidget->v_ColorRobotYellow.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(restartSimulator()));

    //network
    QObject::connect(configwidget->v_VisionMulticastAddr.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectVisionSocket()));
    QObject::connect(configwidget->v_VisionMulticastPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectVisionSocket()));
    QObject::connect(configwidget->v_CommandListenPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectCommandSocket()));
    QObject::connect(configwidget->v_BlueStatusSendPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectBlueStatusSocket()));
    QObject::connect(configwidget->v_YellowStatusSendPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectYellowStatusSocket()));
    QObject::connect(configwidget->v_SimControlListenPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectSimControlSocket()));
    QObject::connect(configwidget->v_BlueControlListenPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectBlueControlSocket()));
    QObject::connect(configwidget->v_YellowControlListenPort.get(), SIGNAL(wasEdited(VarPtr)), this, SLOT(reconnectYellowControlSocket()));
    timer->start();


    this->showMaximized();
    this->setWindowTitle("grSim");

    robotwidget->teamCombo->setCurrentIndex(0);
    robotwidget->robotCombo->setCurrentIndex(0);
    auto robotPicture = glwidget->ssl->robots[robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img;
    if(robotPicture != nullptr) {
        robotwidget->setPicture(robotPicture);
    }
    robotwidget->id = 0;
}

MainWindow::~MainWindow() {

}

void MainWindow::showHideSimulator(bool v)
{
    if (v) {glwidget->show();showsimulator->setChecked(true);}
    else {glwidget->hide();showsimulator->setChecked(false);}
}

void MainWindow::changeCurrentRobot()
{
    // skip when robot count is zero to avoid out-of-range access for robots[]
    // if zero, robotCombo->currentIndex returns -1
    if(configwidget->Robots_Count() != 0) {
        glwidget->Current_robot=robotwidget->robotCombo->currentIndex();
        robotwidget->setPicture(glwidget->ssl->robots[glwidget->ssl->robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img);
        robotwidget->id = robotIndex(glwidget->Current_robot, glwidget->Current_team);
        robotwidget->changeRobotOnOff(robotwidget->id, glwidget->ssl->robots[robotwidget->id]->on);
    }
}

void MainWindow::changeCurrentTeam()
{
    glwidget->Current_team=robotwidget->teamCombo->currentIndex();
    // skip when robot count is zero to avoid out of range access for robots[]
    // (if zero, robotCombo->currentIndex returns -1)
    if(configwidget->Robots_Count() != 0) {
        robotwidget->setPicture(glwidget->ssl->robots[robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img);
        robotwidget->id = robotIndex(glwidget->Current_robot, glwidget->Current_team);
        robotwidget->changeRobotOnOff(robotwidget->id, glwidget->ssl->robots[robotwidget->id]->on);
    }
}

void MainWindow::changeRobotCount()
{
    auto newRobotCount = configwidget->Robots_Count();

    if(newRobotCount < 0 || newRobotCount > MAX_ROBOT_COUNT) {
        newRobotCount = std::min(std::max(0, newRobotCount), static_cast<int>(MAX_ROBOT_COUNT));
        configwidget->v_Robots_Count->setInt(newRobotCount);
    }

    robotwidget->changeRobotCount(newRobotCount);
    
    int newCurrentRobot = std::min(newRobotCount - 1, glwidget->Current_robot);
    robotwidget->changeCurrentRobot(newCurrentRobot);
    restartSimulator();
    changeCurrentRobot();
}

void MainWindow::changeGravity()
{
    dWorldSetGravity (glwidget->ssl->p->world,0,0,-configwidget->Gravity());
}

int MainWindow::robotIndex(int robot,int team)
{
    return glwidget->ssl->robotIndex(robot, team);
}

void MainWindow::changeTimer()
{
    timer->setInterval(getInterval());
}

QString dRealToStr(dReal a)
{
    QString s;
    s.setNum(a,'f',3);
    if (a>=0) return QString('+') + s;
    return s;
}

void MainWindow::update()
{
    if (glwidget->ssl->isGLEnabled) {
        glwidget->ssl->g->enableGraphics();
        glwidget->updateGL();
    } else {
        glwidget->ssl->g->disableGraphics();
        glwidget->step();
    }

    int R = robotIndex(glwidget->Current_robot,glwidget->Current_team);

    if(0 <= R)
    {
        const dReal* vv = dBodyGetLinearVel(glwidget->ssl->robots[R]->chassis->body);
        static dVector3 lvv;
        dVector3 aa;
        aa[0]=(vv[0]-lvv[0])/configwidget->DeltaTime();
        aa[1]=(vv[1]-lvv[1])/configwidget->DeltaTime();
        aa[2]=(vv[2]-lvv[2])/configwidget->DeltaTime();
        robotwidget->vellabel->setText(QString::number(sqrt(vv[0]*vv[0]+vv[1]*vv[1]+vv[2]*vv[2]),'f',3));
        robotwidget->acclabel->setText(QString::number(sqrt(aa[0]*aa[0]+aa[1]*aa[1]+aa[2]*aa[2]),'f',3));
        lvv[0]=vv[0];
        lvv[1]=vv[1];
        lvv[2]=vv[2];
    }
    
    fpslabel->setText(QString("Frame rate: %1 fps").arg(QString::asprintf("%06.2f",glwidget->getFPS())));        
    if (glwidget->ssl->selected!=-1)
    {
        selectinglabel->setVisible(true);
        if (glwidget->ssl->selected==-2)
        {            
            selectinglabel->setText("Ball");
        }
        else
        {            
            int R = glwidget->ssl->selected%configwidget->Robots_Count();
            int T = glwidget->ssl->selected/configwidget->Robots_Count();
            if (T==0) selectinglabel->setText(QString("%1:Blue").arg(R));
            else selectinglabel->setText(QString("%1:Yellow").arg(R));
        }
    }
    else selectinglabel->setVisible(false);
    vanishlabel->setVisible(configwidget->vanishing());
    noiselabel->setVisible(configwidget->noise());
    cursorlabel->setText(QString("Cursor: [X=%1;Y=%2;Z=%3]").arg(dRealToStr(glwidget->ssl->cursor_x)).arg(dRealToStr(glwidget->ssl->cursor_y)).arg(dRealToStr(glwidget->ssl->cursor_z)));
    statusWidget->update();
}

void MainWindow::updateRobotLabel()
{
    robotwidget->teamCombo->setCurrentIndex(glwidget->Current_team);
    robotwidget->robotCombo->setCurrentIndex(glwidget->Current_robot);
    robotwidget->id = robotIndex(glwidget->Current_robot,glwidget->Current_team);
    robotwidget->changeRobotOnOff(robotwidget->id,glwidget->ssl->robots[robotwidget->id]->on);
}


void MainWindow::changeBallMass()
{
    glwidget->ssl->ball->setMass(configwidget->BallMass());
}


void MainWindow::changeBallGroundSurface()
{
    PSurface* ballwithwall = glwidget->ssl->p->findSurface(glwidget->ssl->ball,glwidget->ssl->ground);
    ballwithwall->surface.mode = dContactBounce | dContactApprox1 | dContactSlip1 | dContactSlip2;
    ballwithwall->surface.mu = fric(configwidget->BallFriction());
    ballwithwall->surface.bounce = configwidget->BallBounce();
    ballwithwall->surface.bounce_vel = configwidget->BallBounceVel();
    ballwithwall->surface.slip1 = configwidget->BallSlip();
    ballwithwall->surface.slip2 = configwidget->BallSlip();
}

void MainWindow::changeBallDamping()
{
    dBodySetLinearDampingThreshold(glwidget->ssl->ball->body,0.001);
    dBodySetLinearDamping(glwidget->ssl->ball->body,configwidget->BallLinearDamp());
    dBodySetAngularDampingThreshold(glwidget->ssl->ball->body,0.001);
    dBodySetAngularDamping(glwidget->ssl->ball->body,configwidget->BallAngularDamp());
}

void MainWindow::restartSimulator()
{        
    auto newWorld = new SSLWorld(glwidget,glwidget->cfg,glwidget->forms[FORMATION_INSIDE_1],glwidget->forms[FORMATION_INSIDE_1]);
    newWorld->glinit();
    newWorld->visionServer = visionServer;
    newWorld->commandSocket = commandSocket;
    newWorld->blueStatusSocket = blueStatusSocket;
    newWorld->yellowStatusSocket = yellowStatusSocket;
    newWorld->simControlSocket = simControlSocket;
    newWorld->blueControlSocket = blueControlSocket;
    newWorld->yellowControlSocket = yellowControlSocket;

    auto oldWorld = glwidget->ssl;
    glwidget->ssl = newWorld;
    delete oldWorld;
}

void MainWindow::ballMenuTriggered(QAction* act)
{
    dReal l = configwidget->Field_Length()/2.0;
    dReal w = configwidget->Field_Width()/2.0;
    dReal p = l - configwidget->Field_Penalty_Point();
    if (act->text()==tr("Put on Center")) glwidget->putBall(0,0);
    else if (act->text()==tr("Put on Corner 1")) glwidget->putBall(-l,-w);
    else if (act->text()==tr("Put on Corner 2")) glwidget->putBall(-l, w);
    else if (act->text()==tr("Put on Corner 3")) glwidget->putBall( l,-w);
    else if (act->text()==tr("Put on Corner 4")) glwidget->putBall( l, w);
    else if (act->text()==tr("Put on Penalty 1")) glwidget->putBall( p, 0);
    else if (act->text()==tr("Put on Penalty 2")) glwidget->putBall(-p, 0);
}

void MainWindow::toggleFullScreen(bool a)
{
    if (a)
    {
        prevState = saveState();
        for (auto dockwidget : findChildren<QDockWidget*>())
        {
            dockwidget->hide();
        }
        showconfig->setEnabled(false);
        showrobot->setEnabled(false);
        showFullScreen();
    }
    else {
        showNormal();
        restoreState(prevState);
        showconfig->setEnabled(true);
        showrobot->setEnabled(true);
    }
}

void MainWindow::setCurrentRobotPosition()
{
    int i = robotIndex(glwidget->Current_robot,glwidget->Current_team);
    bool ok1=false,ok2=false,ok3=false;
    dReal x = robotwidget->getPoseWidget->x->text().toFloat(&ok1);
    dReal y = robotwidget->getPoseWidget->y->text().toFloat(&ok2);
    dReal a = robotwidget->getPoseWidget->a->text().toFloat(&ok3);
    if (!ok1) {logStatus("Invalid dReal for x",QColor("red"));return;}
    if (!ok2) {logStatus("Invalid dReal for y",QColor("red"));return;}
    if (!ok3) {logStatus("Invalid dReal for angle",QColor("red"));return;}
    glwidget->ssl->robots[i]->setXY(x,y);
    glwidget->ssl->robots[i]->setDir(a);
    robotwidget->getPoseWidget->close();
}

void MainWindow::takeSnapshot()
{
    QPixmap p(glwidget->renderPixmap(glwidget->size().width(),glwidget->size().height(),false));
    p.save(QFileDialog::getSaveFileName(this, tr("Save Snapshot"), current_dir, tr("Images (*.png)")),"PNG",100);
}

void MainWindow::takeSnapshotToClipboard()
{
    QPixmap p(glwidget->renderPixmap(glwidget->size().width(),glwidget->size().height(),false));
    QClipboard* b = QApplication::clipboard();
    b->setPixmap(p);
}

void MainWindow::showAbout()
{
    QString title = QString("grSim v0.9 - Build r1240");
    QString text = QString("grSim - RoboCup Small Size Soccer Robots Simulator\n\n(C) 2011 - Parsian Robotic Center\nhttp://eew.aut.ac.ir/~parsian/grsim\n\ngrSim is free software released under the terms of GNU GPL v3");
    QMessageBox::about(this, title, text);
}



void MainWindow::reconnectBlueStatusSocket()
{
    delete blueStatusSocket;
    blueStatusSocket = new QUdpSocket(this);
}

void MainWindow::reconnectYellowStatusSocket()
{
    delete yellowStatusSocket;
    yellowStatusSocket = new QUdpSocket(this);
}

void MainWindow::reconnectCommandSocket()
{
    commandSocket->disconnectFromHost();
    if (commandSocket->bind(QHostAddress::Any,configwidget->CommandListenPort()))
        logStatus(QString("Command listen port bound on: %1").arg(configwidget->CommandListenPort()),QColor("green"));
    else
        logStatus(QString("Command listen port could not be bound on: %1").arg(configwidget->YellowControlListenPort()),QColor("red"));
}

void MainWindow::reconnectSimControlSocket()
{
    simControlSocket->disconnectFromHost();
    if (simControlSocket->bind(QHostAddress::Any,configwidget->SimControlListenPort()))
        logStatus(QString("Sim control listen port bound on: %1").arg(configwidget->SimControlListenPort()),QColor("green"));
    else
        logStatus(QString("Sim control listen port could not be bound on: %1").arg(configwidget->YellowControlListenPort()),QColor("red"));
    QObject::connect(simControlSocket,SIGNAL(readyRead()),this,SLOT(simControlSocketReady()));
}

void MainWindow::reconnectBlueControlSocket()
{
    blueControlSocket->disconnectFromHost();
    if (blueControlSocket->bind(QHostAddress::Any,configwidget->BlueControlListenPort()))
        logStatus(QString("Blue control listen port bound on: %1").arg(configwidget->BlueControlListenPort()),QColor("green"));
    else
        logStatus(QString("Blue control listen port could not be bound on: %1").arg(configwidget->YellowControlListenPort()),QColor("red"));
}

void MainWindow::reconnectYellowControlSocket()
{
    yellowControlSocket->disconnectFromHost();
    if (yellowControlSocket->bind(QHostAddress::Any,configwidget->YellowControlListenPort()))
        logStatus(QString("Yellow control listen port bound on: %1").arg(configwidget->YellowControlListenPort()),QColor("green"));
    else
        logStatus(QString("Yellow control listen port could not be bound on: %1").arg(configwidget->YellowControlListenPort()),QColor("red"));
}

void MainWindow::reconnectVisionSocket()
{
    if (visionServer == nullptr) {
        visionServer = new RoboCupSSLServer(this);
    }
    visionServer->change_address(configwidget->VisionMulticastAddr());
    visionServer->change_port(configwidget->VisionMulticastPort());
    logStatus(QString("Vision server connected on: %1").arg(configwidget->VisionMulticastPort()),QColor("green"));
}

void MainWindow::recvActions()
{
    glwidget->ssl->recvActions();
}

void MainWindow::simControlSocketReady()
{
    glwidget->ssl->simControlSocketReady();
    if (glwidget->ssl->restartRequired) {
        restartSimulator();
    }
}

void MainWindow::blueControlSocketReady()
{
    glwidget->ssl->blueControlSocketReady();
}

void MainWindow::yellowControlSocketReady()
{
    glwidget->ssl->yellowControlSocketReady();
}

void MainWindow::setIsGlEnabled(bool value)
{
  glwidget->ssl->isGLEnabled = value;
}
