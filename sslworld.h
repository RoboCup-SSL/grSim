#ifndef SSLWORLD_H
#define SSLWORLD_H


#include <QGLWidget>

#include "Graphics/graphics.h"
#include "Physics/pworld.h"
#include "Physics/pball.h"
#include "Physics/pground.h"
#include "Physics/pfixedbox.h"
#include "Physics/pray.h"

#include "proto/robocup_ssl_server.h"

#include "robot.h"

#include "configwidget.h"

#include <QObject>
#include <QUdpSocket>
#include <QList>

#define ROBOT_COUNT 5

class RobotsFomation;
class SendingPacket {
    public:
    SendingPacket(SSL_WrapperPacket* _packet,int _t);
    SSL_WrapperPacket* packet;
    int t;
};

class SSLWorld : public QObject
{
    Q_OBJECT
private:
    QGLWidget* m_parent;
    int framenum;
    float last_dt;
    QList<SendingPacket*> sendQueue;
    /**BALL TRACKER TEST**/
    float bx,by,vx,vy;    
    char packet[200];
public:
    float customDT;
    bool isGLEnabled;
    SSLWorld(QGLWidget* parent,ConfigWidget* _cfg,RobotsFomation *form1,RobotsFomation *form2);
    virtual ~SSLWorld();
    void glinit();
    void step(float dt=-1);
    SSL_WrapperPacket* generatePacket();
    void sendVisionBuffer();
    bool ballTrainingMode;
    ConfigWidget* cfg;
    CGraphics* g;
    PWorld* p;
    PBall* ball;
    PGround* ground;
    PRay* ray;
    PFixedBox* walls[10];
    int selected;
    bool show3DCursor;
    float cursor_x,cursor_y,cursor_z;
    float cursor_radius;
    RoboCupSSLServer *visionServer;
    QUdpSocket *blueSocket,*yellowSocket;
    QUdpSocket *blueStatusSocket,*yellowStatusSocket;
    bool updatedCursor;
    Robot* robots[ROBOT_COUNT*2];    
    QTime *timer;
    void recvActions(QUdpSocket* commandSocket,QUdpSocket* statusSocket,int statusPort,int team);
public slots:
    void recvFromBlue();
    void recvFromYellow();
    void reconnectBlueCommandSocket();
    void reconnectYellowCommandSocket();
    void reconnectYellowStatusSocket();
    void reconnectBlueStatusSocket();
    void reconnectVisionSocket();
signals:
    void fpsChanged(int newFPS);
};

class RobotsFomation {
    public:
        float x[ROBOT_COUNT];
        float y[ROBOT_COUNT];
        RobotsFomation(int type);
        void setAll(float *xx,float *yy);
        void loadFromFile(const QString& filename);
        void resetRobots(Robot** r,int team);
};

dReal fric(float f);
int robotIndex(int robot,int team);

#endif // SSLWORLD_H
