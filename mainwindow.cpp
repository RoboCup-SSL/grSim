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
#include "mainwindow.h"

#include "logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
    QTimer *timer = new QTimer(this);
    timer->setInterval(_RENDER_INTERVAL);

    /* Init Workspace */
    workspace = new QWorkspace(this);
    setCentralWidget(workspace);    

    /* Widgets */

    configwidget = new ConfigWidget();
    dockconfig = new ConfigDockWidget(this,configwidget);
    addDockWidget(Qt::LeftDockWidgetArea,dockconfig);

    fpslabel = new QLabel(this);
    cursorlabel = new QLabel(this);
    selectinglabel = new QLabel(this);
    statusBar()->addWidget(fpslabel);
    statusBar()->addWidget(cursorlabel);
    statusBar()->addWidget(selectinglabel);

    QMenu *fileMenu = new QMenu("&File");
    menuBar()->addMenu(fileMenu);
    QAction *exit = new QAction("E&xit", fileMenu);
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


    QToolBar* toolbar = new QToolBar(this);
    toolbar->addWidget(new QLabel("Selected robot: "));
    teamCombo = new QComboBox(this);
    teamCombo->addItem("Blue");
    teamCombo->addItem("Yellow");
    toolbar->addWidget(teamCombo);
    robotCombo = new QComboBox(this);
    robotCombo->addItem("0");
    robotCombo->addItem("1");
    robotCombo->addItem("2");
    robotCombo->addItem("3");
    robotCombo->addItem("4");
    toolbar->addWidget(robotCombo);
    addToolBar(toolbar);

    printer = new CStatusPrinter();
    statusWidget = new CStatusWidget(printer);
    this->addDockWidget(Qt::BottomDockWidgetArea, statusWidget);
    initLogger((void*)printer);


    glwidget = new GLWidget(this,configwidget);
    glwidget->setWindowTitle(tr("Simulator"));
    glwidget->resize(512,512);
    workspace->addWindow(glwidget, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);



    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(showsimulator, SIGNAL(triggered(bool)), this, SLOT(showHideSimulator(bool)));
    QObject::connect(showconfig, SIGNAL(triggered(bool)), this, SLOT(showHideConfig(bool)));
    QObject::connect(glwidget, SIGNAL(closeSignal(bool)), this, SLOT(showHideSimulator(bool)));    
    QObject::connect(dockconfig, SIGNAL(closeSignal(bool)), this, SLOT(showHideConfig(bool)));
    QObject::connect(glwidget, SIGNAL(selectedRobot()), this, SLOT(updateRobotLabel()));
    QObject::connect(robotCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCurrentRobot()));
    QObject::connect(teamCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCurrentTeam()));
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

    //geometry config vars
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
    QObject::connect(configwidget->v_CommandListenPort, SIGNAL(wasEdited(VarType*)), glwidget->ssl, SLOT(reconnectCommandSocket()));

    timer->start();


    this->showMaximized();
    this->setWindowTitle("Parsian Simulator");

    teamCombo->setCurrentIndex(0);
    robotCombo->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{

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
    glwidget->Current_robot=robotCombo->currentIndex();
}

void MainWindow::changeCurrentTeam()
{
    glwidget->Current_team=teamCombo->currentIndex();
}

void MainWindow::changeGravity()
{
    dWorldSetGravity (glwidget->ssl->p->world,0,0,-configwidget->Gravity());
}

void MainWindow::update()
{
    glwidget->updateGL();
    fpslabel->setText(QString("Frame rate: %1 fps").arg(glwidget->getFPS()));
    QString n1,n2,n3;
    if (glwidget->ssl->selected!=-1)
    {
        selectinglabel->setVisible(true);
        if (glwidget->ssl->selected==-2)
        {            
            selectinglabel->setText("Ball");
        }
        else
        {            
            int R = glwidget->ssl->selected%5;
            int T = glwidget->ssl->selected/5;
            if (T==0) selectinglabel->setText(QString("%1:Blue").arg(R));
            else selectinglabel->setText(QString("%1:Yellow").arg(R));
        }
    }
    else selectinglabel->setVisible(false);
    cursorlabel->setText(QString("Cursor: [X=%1;Y=%2;Z=%3]").arg(n1.setNum(glwidget->ssl->cursor_x,'f',3)).arg(n2.setNum(glwidget->ssl->cursor_y,'f',3)).arg(n3.setNum(glwidget->ssl->cursor_z,'f',3)));
    statusWidget->update();
}

void MainWindow::updateRobotLabel()
{
    teamCombo->setCurrentIndex(glwidget->Current_team);
    robotCombo->setCurrentIndex(glwidget->Current_robot);
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
    ballwithwall->surface.mode = dContactBounce | dContactApprox1 | dContactSlip1;
    ballwithwall->surface.mu = configwidget->ballfriction();
    ballwithwall->surface.bounce = configwidget->ballbounce();
    ballwithwall->surface.bounce_vel = configwidget->ballbouncevel();
    ballwithwall->surface.slip1 = configwidget->ballslip();
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
