//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    robocup_ssl_server.h
  \brief   C++ Interface: robocup_ssl_server
  \author  Stefan Zickler, 2009
  \author  Jan Segre, 2012
*/
//========================================================================
#ifndef ROBOCUP_SSL_SERVER_H
#define ROBOCUP_SSL_SERVER_H
#include <string>
#include <QMutex>
#include <QObject>
#include "ssl_vision_detection.pb.h"
#include "ssl_vision_geometry.pb.h"
#include "ssl_vision_wrapper.pb.h"
using namespace std;

class QUdpSocket;
class QHostAddress;
class QNetworkInterface;

class RoboCupSSLServer
{
friend class MultiStackRoboCupSSL;
public:
    RoboCupSSLServer(QObject *parent=0,
                     const quint16 &port=10002,
                     const string &net_address="224.5.23.2",
                     const string &net_interface="");

    ~RoboCupSSLServer();

    bool send(const SSL_WrapperPacket & packet);
    bool send(const SSL_DetectionFrame & frame);
    bool send(const SSL_GeometryData & geometry);
    void change_port(const quint16 &port);
    void change_address(const string & net_address);
    void change_interface(const string & net_interface);

protected:
    QUdpSocket * _socket;
    QMutex mutex;
    quint16 _port;
    QHostAddress * _net_address;
    QNetworkInterface * _net_interface;
};

#endif

