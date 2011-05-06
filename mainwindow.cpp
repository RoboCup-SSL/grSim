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
    initLogger((void*)printer);

    /* Init Workspace */
    workspace = new QWorkspace(this);
    setCentralWidget(workspace);    

    /* Widgets */

    configwidget = new ConfigWidget();
    dockconfig = new ConfigDockWidget(this,configwidget);

    glwidget = new GLWidget(this,configwidget);
    glwidget->setWindowTitle(tr("Simulator"));
    glwidget->resize(512,512);    

    robotwidget = new RobotWidget(this);
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

    QMenu *viewMenu = new QMenu("&View");
    menuBar()->addMenu(viewMenu);
    showsimulator = new QAction("&Simulator", viewMenu);
    showsimulator->setCheckable(true);
    showsimulator->setChecked(true);
    viewMenu->addAction(showsimulator);
    showconfig = new QAction("&Configuration", viewMenu);
    showconfig->setCheckable(true);
    showconfig->setChecked(true);
    viewMenu->addAction(showconfig);

    QMenu *simulatorMenu = new QMenu("&Simulator");
    menuBar()->addMenu(simulatorMenu);
    QMenu *robotMenu = new QMenu("&Robot");
    QMenu *ballMenu = new QMenu("&Ball");
    simulatorMenu->addMenu(robotMenu);
    simulatorMenu->addMenu(ballMenu);

    ballMenu->addAction(tr("Put on Center"))->setShortcut(QKeySequence("Ctrl+0"));
    ballMenu->addAction(tr("Put on Corner 1"))->setShortcut(QKeySequence("Ctrl+1"));
    ballMenu->addAction(tr("Put on Corner 2"))->setShortcut(QKeySequence("Ctrl+2"));
    ballMenu->addAction(tr("Put on Corner 3"))->setShortcut(QKeySequence("Ctrl+3"));
    ballMenu->addAction(tr("Put on Corner 4"))->setShortcut(QKeySequence("Ctrl+4"));
    ballMenu->addAction(tr("Put on Penalty 1"))->setShortcut(QKeySequence("Ctrl+5"));
    ballMenu->addAction(tr("Put on Penalty 2"))->setShortcut(QKeySequence("Ctrl+6"));

    robotMenu->addMenu(glwidget->blueRobotsMenu);
    robotMenu->addMenu(glwidget->yellowRobotsMenu);

    fullScreenAct = new QAction(tr("&Full screen"),simulatorMenu);
    fullScreenAct->setShortcut(QKeySequence("F2"));
    fullScreenAct->setCheckable(true);
    fullScreenAct->setChecked(false);
    simulatorMenu->addAction(fullScreenAct);

    viewMenu->addAction(robotwidget->toggleViewAction());
    viewMenu->addMenu(glwidget->cameraMenu);

    addDockWidget(Qt::LeftDockWidgetArea,dockconfig);
    addDockWidget(Qt::BottomDockWidgetArea, statusWidget);
    addDockWidget(Qt::LeftDockWidgetArea, robotwidget);
    workspace->addWindow(glwidget, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);    
    glwidget->setWindowState(Qt::WindowMaximized);

    timer = new QTimer(this);
    timer->setInterval(getInterval());

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(takeSnapshotAct, SIGNAL(triggered(bool)), this, SLOT(takeSnapshot()));
    QObject::connect(takeSnapshotToClipboardAct, SIGNAL(triggered(bool)), this, SLOT(takeSnapshotToClipboard()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(showsimulator, SIGNAL(triggered(bool)), this, SLOT(showHideSimulator(bool)));
    QObject::connect(showconfig, SIGNAL(triggered(bool)), this, SLOT(showHideConfig(bool)));
    QObject::connect(glwidget, SIGNAL(closeSignal(bool)), this, SLOT(showHideSimulator(bool)));    
    QObject::connect(dockconfig, SIGNAL(closeSignal(bool)), this, SLOT(showHideConfig(bool)));
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
    QObject::connect(glwidget,SIGNAL(toggleFullScreen(bool)),this,SLOT(toggleFullScreen(bool)));
    QObject::connect(glwidget->ssl, SIGNAL(fpsChanged(int)), this, SLOT(customFPS(int)));
    //config related signals
    QObject::connect(configwidget->v_BALLMASS, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallMass()));
    QObject::connect(configwidget->v_CHASSISMASS, SIGNAL(wasEdited(VarType*)), this, SLOT(changeRobotMass()));
    QObject::connect(configwidget->v_KICKERMASS, SIGNAL(wasEdited(VarType*)), this, SLOT(changeKickerMass()));
    QObject::connect(configwidget->v_WHEELMASS, SIGNAL(wasEdited(VarType*)), this, SLOT(changeWheelMass()));    
    QObject::connect(configwidget->v_ballbounce, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_ballbouncevel, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_ballfriction, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_ballslip, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallGroundSurface()));
    QObject::connect(configwidget->v_ballangulardamp, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallDamping()));
    QObject::connect(configwidget->v_balllineardamp, SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallDamping()));
    QObject::connect(configwidget->v_Gravity,  SIGNAL(wasEdited(VarType*)), this, SLOT(changeGravity()));
    QObject::connect(configwidget->v_Kicker_Friction,  SIGNAL(wasEdited(VarType*)), this, SLOT(changeBallKickerSurface()));
    QObject::connect(configwidget->v_plotter_addr, SIGNAL(wasEdited(VarType*)), this, SLOT(reconnectPlotterSocket(VarType*)));
    QObject::connect(configwidget->v_plotter_port, SIGNAL(wasEdited(VarType*)), this, SLOT(reconnectPlotterSocket(VarType*)));

    //geometry config vars
    QObject::connect(configwidget->v_DesiredFPS, SIGNAL(wasEdited(VarType*)), this, SLOT(changeTimer()));
    QObject::connect(configwidget->v_BALLRADIUS, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_BOTTOMHEIGHT, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_CHASSISHEIGHT, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_CHASSISWIDTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_CHASSISLENGTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_KHEIGHT, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_KLENGTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_KWIDTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_KICKERHEIGHT, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_WHEELLENGTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v_WHEELRADIUS, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_LENGTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_MARGIN, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_PENALTY_LINE, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_PENALTY_RADIUS, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_RAD, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_FIELD_WIDTH, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));
    QObject::connect(configwidget->v__SSL_WALL_THICKNESS, SIGNAL(wasEdited(VarType*)), this, SLOT(alertStaticVars()));

    //network
    QObject::connect(configwidget->v_VisionMulticastAddr, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectVisionSocket()));
    QObject::connect(configwidget->v_VisionMulticastPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectVisionSocket()));
    QObject::connect(configwidget->v_BlueCommandListenPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectBlueCommandSocket()));
    QObject::connect(configwidget->v_YellowCommandListenPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectYellowCommandSocket()));
    QObject::connect(configwidget->v_BlueStatusSendPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectBlueStatusSocket()));
    QObject::connect(configwidget->v_YellowStatusSendPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectYellowStatusSocket()));
    timer->start();


    this->showMaximized();
    this->setWindowTitle("Parsian Simulator");

    robotwidget->teamCombo->setCurrentIndex(0);
    robotwidget->robotCombo->setCurrentIndex(0);
    robotwidget->setPicture(glwidget->ssl->robots[robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img);
    robotwidget->id = 0;
    scene = new QGraphicsScene(0,0,800,600);
    plotSocket = NULL;reconnectPlotterSocket(NULL);
    plotterCounter = 0;
    //glwidget->ssl->g->disableGraphics();
}

MainWindow::~MainWindow()
{
    if (plotSocket!=NULL)
    {
        plotSocket->close();
        delete plotSocket;
    }
}

void MainWindow::showHideConfig(bool v)
{
    if (v) {dockconfig->show();showconfig->setChecked(true);}
    else {dockconfig->hide();showconfig->setChecked(false);}
}

void MainWindow::showHideSimulator(bool v)
{
    if (v) {glwidget->show();showsimulator->setChecked(true);}
    else {glwidget->hide();showsimulator->setChecked(false);}
}

void MainWindow::changeCurrentRobot()
{
    glwidget->Current_robot=robotwidget->robotCombo->currentIndex();
    robotwidget->setPicture(glwidget->ssl->robots[robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img);
}

void MainWindow::changeCurrentTeam()
{
    glwidget->Current_team=robotwidget->teamCombo->currentIndex();
    robotwidget->setPicture(glwidget->ssl->robots[robotIndex(glwidget->Current_robot,glwidget->Current_team)]->img);
}

void MainWindow::changeGravity()
{
    dWorldSetGravity (glwidget->ssl->p->world,0,0,-configwidget->Gravity());
}

void MainWindow::changeTimer()
{
    timer->setInterval(getInterval());
}

QString floatToStr(float a)
{
    QString s;
    s.setNum(a,'f',3);
    if (a>=0) return QString('+') + s;
    return s;
}

void MainWindow::update()
{
    if (glwidget->ssl->g->isGraphicsEnabled()) glwidget->updateGL();
    else glwidget->step();

    int R = robotIndex(glwidget->Current_robot,glwidget->Current_team);

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
    QString ss;
    fpslabel->setText(QString("Frame rate: %1 fps").arg(ss.sprintf("%06.2f",glwidget->getFPS())));
    static int fff = 0;
    qDebug() << fff << "," << glwidget->getFPS();
    fff ++;
    if (glwidget->ssl->selected!=-1)
    {
        selectinglabel->setVisible(true);
        if (glwidget->ssl->selected==-2)
        {            
            selectinglabel->setText("Ball");
        }
        else
        {            
            int R = glwidget->ssl->selected%ROBOT_COUNT;
            int T = glwidget->ssl->selected/ROBOT_COUNT;
            if (T==0) selectinglabel->setText(QString("%1:Blue").arg(R));
            else selectinglabel->setText(QString("%1:Yellow").arg(R));
        }
    }
    else selectinglabel->setVisible(false);
    vanishlabel->setVisible(configwidget->vanishing());
    noiselabel->setVisible(configwidget->noise());
    cursorlabel->setText(QString("Cursor: [X=%1;Y=%2;Z=%3]").arg(floatToStr(glwidget->ssl->cursor_x)).arg(floatToStr(glwidget->ssl->cursor_y)).arg(floatToStr(glwidget->ssl->cursor_z)));
    statusWidget->update();
    if (configwidget->plotter())
    {
        plotterCounter++;

        {
            float data[PLOT_PACKET_SIZE];
            float x,y,z;
            data[0] = -1;
            glwidget->ssl->ball->getBodyPosition(x,y,z);
            data[1] = x*1000.0f;
            data[2] = y*1000.0f;
            data[3] = z*1000.0f;
            data[4] = 0;
            const dReal * v = dBodyGetLinearVel(glwidget->ssl->ball->body);
            data[5] = v[0]*1000.0f;
            data[6] = v[1]*1000.0f;
            data[7] = v[2]*1000.0f;
            data[8] = 0;
            data[9] = (data[5]-balldata[5])/configwidget->DeltaTime();
            data[10]= (data[6]-balldata[6])/configwidget->DeltaTime();
            data[11]= (data[7]-balldata[7])/configwidget->DeltaTime();
            data[12]= 0;
            for (int k = 0;k<2*ROBOT_COUNT;k++)
            {
                balldata[k] = data[k];
            }
            plotSocket->writeDatagram((char*)data, sizeof(float)*PLOT_PACKET_SIZE, QHostAddress(configwidget->plotter_addr().c_str()), configwidget->plotter_port());
            for (int i=0;i<ROBOT_COUNT*2;i++)
            {
                data[0] = i+(i/ROBOT_COUNT)*100;
                glwidget->ssl->robots[i]->chassis->getBodyPosition(x,y,z);
                data[1] = x*1000.0f;
                data[2] = y*1000.0f;
                data[3] = z*1000.0f;
                data[4] = glwidget->ssl->robots[i]->getDir();
                const dReal * v = dBodyGetLinearVel(glwidget->ssl->robots[i]->chassis->body);
                data[5] = v[0]*1000.0f;
                data[6] = v[1]*1000.0f;
                data[7] = v[2]*1000.0f;
                data[8] = dBodyGetAngularVel(glwidget->ssl->robots[i]->chassis->body)[2] * 180.0 / M_PI;
                data[9] = (data[5]-glwidget->ssl->robots[i]->data[5])/configwidget->DeltaTime();
                data[10]= (data[6]-glwidget->ssl->robots[i]->data[6])/configwidget->DeltaTime();
                data[11]= (data[7]-glwidget->ssl->robots[i]->data[7])/configwidget->DeltaTime();
                data[12]= (data[8]-glwidget->ssl->robots[i]->data[8])/configwidget->DeltaTime();
                for (int k = 0;k<2*ROBOT_COUNT;k++)
                {
                    glwidget->ssl->robots[i]->data[k] = data[k];
                }
                plotSocket->writeDatagram((char*)data, sizeof(float)*PLOT_PACKET_SIZE, QHostAddress(configwidget->plotter_addr().c_str()), configwidget->plotter_port());
            }
        }
    }
}

void MainWindow::updateRobotLabel()
{
    robotwidget->teamCombo->setCurrentIndex(glwidget->Current_team);
    robotwidget->robotCombo->setCurrentIndex(glwidget->Current_robot);
    robotwidget->id = robotIndex(robotwidget->robotCombo->currentIndex(),robotwidget->teamCombo->currentIndex());
    robotwidget->changeRobotOnOff(robotwidget->id,glwidget->ssl->robots[robotwidget->id]->on);
}


void MainWindow::changeBallMass()
{
    glwidget->ssl->ball->setMass(configwidget->BALLMASS());
}

void MainWindow::changeRobotMass()
{    
    for (int i=0;i<10;i++)
    {
        glwidget->ssl->robots[i]->chassis->setMass(configwidget->CHASSISMASS()*0.99f);
        glwidget->ssl->robots[i]->dummy->setMass(configwidget->CHASSISMASS()*0.01f);
    }
}

void MainWindow::changeKickerMass()
{
    for (int i=0;i<10;i++)
    {
        glwidget->ssl->robots[i]->kicker->box->setMass(configwidget->KICKERMASS());
    }
}

void MainWindow::changeWheelMass()
{
    for (int i=0;i<10;i++)
    {
        for (int j=0;j<4;j++)
        {
            glwidget->ssl->robots[i]->wheels[j]->cyl->setMass(configwidget->WHEELMASS());
        }
    }    
}

void MainWindow::changeBallGroundSurface()
{
    PSurface* ballwithwall = glwidget->ssl->p->findSurface(glwidget->ssl->ball,glwidget->ssl->ground);    
    ballwithwall->surface.mode = dContactBounce | dContactApprox1 | dContactSlip1 | dContactSlip2;
    ballwithwall->surface.mu = fric(configwidget->ballfriction());
    ballwithwall->surface.bounce = configwidget->ballbounce();
    ballwithwall->surface.bounce_vel = configwidget->ballbouncevel();
    ballwithwall->surface.slip1 = configwidget->ballslip();
    ballwithwall->surface.slip2 = configwidget->ballslip();
}

void MainWindow::changeBallKickerSurface()
{
    PSurface* ballwithkicker = glwidget->ssl->p->findSurface(glwidget->ssl->ball,glwidget->ssl->robots[0]->kicker->box);
    ballwithkicker->surface.mu = configwidget->Kicker_Friction();
}

void MainWindow::changeBallDamping()
{
    dBodySetLinearDampingThreshold(glwidget->ssl->ball->body,0.001);
    dBodySetLinearDamping(glwidget->ssl->ball->body,configwidget->balllineardamp());
    dBodySetAngularDampingThreshold(glwidget->ssl->ball->body,0.001);
    dBodySetAngularDamping(glwidget->ssl->ball->body,configwidget->ballangulardamp());
}

void MainWindow::alertStaticVars()
{    
    logStatus("You must restart application to see the changes of geometry parameters",QColor("orange"));
}

void MainWindow::ballMenuTriggered(QAction* act)
{
    float l = configwidget->_SSL_FIELD_LENGTH()/2000.0f;
    float w = configwidget->_SSL_FIELD_WIDTH()/2000.0f;
    float p = l - configwidget->_SSL_FIELD_PENALTY_POINT()/1000.f;
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
        view = new GLWidgetGraphicsView(scene,glwidget);
        lastSize = glwidget->size();        
        view->setViewport(glwidget);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        view->setFrameStyle(0);
        view->showFullScreen();
        view->setFocus();        
        glwidget->fullScreen = true;
        fullScreenAct->setChecked(true);
    }
    else {
        view->close();        
        workspace->addWindow(glwidget, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
        glwidget->show();
        glwidget->resize(lastSize);
        glwidget->fullScreen = false;
        fullScreenAct->setChecked(false);
        glwidget->setFocusPolicy(Qt::StrongFocus);
        glwidget->setFocus();
    }
}

void MainWindow::setCurrentRobotPosition()
{
    int i = robotIndex(glwidget->Current_robot,glwidget->Current_team);
    bool ok1=false,ok2=false,ok3=false;
    float x = robotwidget->getPoseWidget->x->text().toFloat(&ok1);
    float y = robotwidget->getPoseWidget->y->text().toFloat(&ok2);
    float a = robotwidget->getPoseWidget->a->text().toFloat(&ok3);
    if (!ok1) {logStatus("Invalid float for x",QColor("red"));return;}
    if (!ok2) {logStatus("Invalid float for y",QColor("red"));return;}
    if (!ok3) {logStatus("Invalid float for angle",QColor("red"));return;}
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

void MainWindow::reconnectPlotterSocket(VarType*)
{
  if (plotSocket!=NULL)
  {
    plotSocket->close();
    delete plotSocket;
  }
  plotSocket = new QUdpSocket(this);
  plotSocket->bind(QHostAddress(configwidget->plotter_addr().c_str()),configwidget->plotter_port());
  logStatus(QString("Plotter socket connected on: %1:%2").arg(configwidget->plotter_addr().c_str()).arg(configwidget->plotter_port()),QColor("green"));
}
