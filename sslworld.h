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

#ifndef SSLWORLD_H
#define SSLWORLD_H


#include <QGLWidget>
#include <QObject>
#include <QUdpSocket>
#include <QList>


#include "Graphics/graphics.h"
#include "Physics/pworld.h"
#include "Physics/pball.h"
#include "Physics/pground.h"
#include "Physics/pfixedbox.h"
#include "Physics/pray.h"

#include "proto/robocup_ssl_server.h"

#include "robot.h"
#include "configwidget.h"

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
    char packet[200];
    char *in_buffer;    
public:    
    float customDT;
    bool isGLEnabled;
    SSLWorld(QGLWidget* parent,ConfigWidget* _cfg,RobotsFomation *form1,RobotsFomation *form2);
    virtual ~SSLWorld();
    void glinit();
    void step(float dt=-1);
    SSL_WrapperPacket* generatePacket();
    void sendVisionBuffer();
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
    QUdpSocket *commandSocket;
    QUdpSocket *blueStatusSocket,*yellowStatusSocket;
    bool updatedCursor;
    Robot* robots[ROBOT_COUNT*2];    
    QTime *timer;    
public slots:
    void recvActions();
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
