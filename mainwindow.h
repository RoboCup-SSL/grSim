#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWorkspace>
#include <QLabel>

#include "glwidget.h"
#include "configwidget.h"
#include "statuswidget.h"
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
    void changeRobotMass();
    void changeKickerMass();
    void changeWheelMass();
    void changeBallGroundSurface();
    void changeBallKickerSurface();
    void changeBallDamping();
    void changeGravity();

    void alertStaticVars();
    void ballMenuTriggered(QAction* act);
    void toggleFullScreen(bool);
    void setCurrentRobotPosition();
    void takeSnapshot();
    void takeSnapshotToClipboard();
    void reconnectPlotterSocket(VarType*);
private:
    QWorkspace* workspace;
    GLWidget *glwidget;
    ConfigWidget *configwidget;
    ConfigDockWidget *dockconfig;
    RobotWidget *robotwidget;
    QUdpSocket* plotSocket;    
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
};

#endif // MAINWINDOW_H
