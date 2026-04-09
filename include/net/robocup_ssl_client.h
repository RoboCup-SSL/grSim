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
  \file    robocup_ssl_client.h
  \brief   C++ Interface: robocup_ssl_client
  \author  Stefan Zickler, 2009
  \author  Jan Segre, 2012
*/
//========================================================================
#ifndef ROBOCUP_SSL_CLIENT_H
#define ROBOCUP_SSL_CLIENT_H
#include <string>
#include <QMutex>
#include "ssl_vision_detection.pb.h"
#include "ssl_vision_geometry.pb.h"
#include "ssl_vision_wrapper.pb.h"
using namespace std;

class QUdpSocket;
class QHostAddress;
class QNetworkInterface;

class RoboCupSSLClient
{
public:
    RoboCupSSLClient(const quint16 & port=10002,
                     const string & net_address="224.5.23.2",
                     const string  & net_interface="");

    RoboCupSSLClient(const quint16 & port,
                     const QHostAddress &,
                     const QNetworkInterface &);

    ~RoboCupSSLClient();

    bool open();
    void close();
    bool receive(SSL_WrapperPacket & packet);
    inline void changePort(quint16 port) {_port = port;}

protected:
    QUdpSocket * _socket;
    QMutex mutex;
    quint16 _port;
    QHostAddress * _net_address;
    QNetworkInterface * _net_interface;
};

#endif
