#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWorkspace>
#include <QLabel>

#include "glwidget.h"
#include "configwidget.h"
#include "statuswidget.h"
#include "aboutwidget.h"
#include "robotwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();    
public slots:
    void update();
    void updateRobotLabel();
    void showHideConfig(bool v);
    void showHideSimulator(bool v);
    void changeCurrentRobot();
    void changeCurrentTeam();

    void changeBallMass();   
    void changeBallGroundSurface();    
    void changeBallDamping();
    void changeGravity();
    void changeTimer();

    void restartSimulator();
    void ballMenuTriggered(QAction* act);
    void toggleFullScreen(bool);
    void setCurrentRobotPosition();
    void takeSnapshot();
    void takeSnapshotToClipboard();

    void customFPS(int fps);
    void showAbout();
    void reconnectCommandSocket();
    void reconnectYellowStatusSocket();
    void reconnectBlueStatusSocket();
    void reconnectVisionSocket();
private:
    int getInterval();    
    QTimer *timer;
    QWorkspace* workspace;
    GLWidget *glwidget;
    ConfigWidget *configwidget;
    ConfigDockWidget *dockconfig;
    RobotWidget *robotwidget;    
    float balldata[PLOT_PACKET_SIZE];    

    CStatusPrinter *printer;
    CStatusWidget *statusWidget;

    QAction *showsimulator, *showconfig;
    QAction* fullScreenAct;
    QLabel *fpslabel,*cursorlabel,*selectinglabel,*vanishlabel,*noiselabel;
    QString current_dir;

    QGraphicsScene *scene;
    GLWidgetGraphicsView *view;
    QSize lastSize;
    CAboutWidget *aboutWidget;
    RoboCupSSLServer *visionServer;
    QUdpSocket *commandSocket;
    QUdpSocket *blueStatusSocket,*yellowStatusSocket;
};

#endif // MAINWINDOW_H
