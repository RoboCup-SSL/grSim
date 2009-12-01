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
*/
//========================================================================
#ifndef ROBOCUP_SSL_CLIENT_H
#define ROBOCUP_SSL_CLIENT_H
#include "netraw.h"
#include <string>
#include <QMutex>
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "messages_robocup_ssl_refbox_log.pb.h"
using namespace std;
/**
	@author Author Name
*/

class RoboCupSSLClient{
protected:
  static const int MaxDataGramSize = 65536;
  char * in_buffer;
  Net::UDP mc; // multicast client
  QMutex mutex;
  int _port;
  string _net_address;
  string _net_interface;
public:
    RoboCupSSLClient(int port = 10002,
                     string net_ref_address="127.0.0.1", ///"224.5.23.2",
                     string net_ref_interface="");

    ~RoboCupSSLClient();
    bool open(bool blocking=false);
    void close();
    bool receive(SSL_WrapperPacket & packet);

};

#endif
