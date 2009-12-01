#include "netraw.h"
#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


#include "util.h"

namespace Net{

//====================================================================//
//  Net::Address: Network address class
//  (C) James Bruce
//====================================================================//

bool Address::setHost(const char *hostname,int port)
{
  // printf("%s %d\n",hostname,port);
  addrinfo *res = NULL;
  getaddrinfo(hostname,NULL,NULL,&res);
  if(!res) return(false);

  mzero(addr);
  addr_len = res->ai_addrlen;
  memcpy(&addr,res->ai_addr,addr_len);

  // set port for internet sockets
  sockaddr_in *sockname = (sockaddr_in*)(&addr);
  if(sockname->sin_family == AF_INET){
    sockname->sin_port = htons(port);
  }else{
    // TODO: any way to set port in general?
  }

  freeaddrinfo(res);
  return(true);
}

void Address::setAny(int port)
{
  mzero(addr);
  sockaddr_in *s = (sockaddr_in*)(&addr);
  s->sin_addr.s_addr = htonl(INADDR_ANY);
  s->sin_port = htons(port);
  addr_len = sizeof(sockaddr_in);
}

in_addr_t Address::getInAddr() const
{
  const sockaddr_in *s = (sockaddr_in*)(&addr);
  return(s->sin_addr.s_addr);
}

void Address::print(FILE *out) const
{
  if(!addr_len){
    printf("null");
    return;
  }

  sockaddr_in *sockname = (sockaddr_in*)(&addr);
  if(sockname->sin_family == AF_INET){
    unsigned a = ntohl(sockname->sin_addr.s_addr);
    unsigned short p = ntohs(sockname->sin_port);

    fprintf(out,"%d.%d.%d.%d:%d",
            (a>>24)&0xFF, (a>>16)&0xFF, (a>>8)&0xFF, a&0xFF, p);
  }else{
    fprintf(out,"?");
  }
}

//====================================================================//
//  Net::UDP: Simple raw UDP messaging
//  (C) James Bruce
//====================================================================//

bool UDP::open(int port, bool share_port_for_multicasting, bool multicast_include_localhost, bool blocking)
{
  // open the socket
  if(fd >= 0) ::close(fd);
  fd = socket(PF_INET, SOCK_DGRAM, 0);

  // set socket as non-blocking
  int flags = fcntl(fd, F_GETFL, 0);
  if(flags < 0) flags = 0;
  fcntl(fd, F_SETFL, flags | (blocking ? 0 : O_NONBLOCK));

  if (share_port_for_multicasting) {
    int reuse=1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,sizeof(reuse))!=0) {
      fprintf(stderr,"ERROR WHEN SETTING SO_REUSEADDR ON UDP SOCKET\n");
      fflush(stderr);
    }
    /*if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, 1)!=0) {
      fprintf(stderr,"ERROR WHEN SETTING SO_REUSEPORT ON UDP SOCKET\n");
      fflush(stderr);
    }*/
  }

  if (multicast_include_localhost) {
    int yes = 1;
    // allow packets to be received on this host
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&yes, sizeof(yes))!=0) {
        fprintf(stderr,"ERROR WHEN SETTING IP_MULTICAST_LOOP ON UDP SOCKET\n");
        fflush(stderr);
    }
  }



  // bind socket to port if nonzero
  if(port != 0){
    sockaddr_in sockname;
    sockname.sin_family = AF_INET;
    sockname.sin_addr.s_addr = htonl(INADDR_ANY);
    sockname.sin_port = htons(port);
    bind(fd,(struct sockaddr*)(&sockname),sizeof(sockname));
  }



  /*
  // allow port reuse (for when a program is quickly restarted)
  // (not sure we really need this)
  int one = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  */

  return(true);
}

bool UDP::addMulticast(const Address &multiaddr,const Address &interface)
{
  static const bool debug = false;
  struct ip_mreq imreq;
  imreq.imr_multiaddr.s_addr = multiaddr.getInAddr();
  imreq.imr_interface.s_addr = interface.getInAddr();

  if(debug){
    printf("0x%08X 0x%08X\n",
           (unsigned)interface.getInAddr(),
           (unsigned)INADDR_ANY);
  }

  int ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       &imreq, sizeof(imreq));
  if(debug) printf("ret=%d\n",ret);

  return(ret == 0);
}

void UDP::close()
{
  if(fd >= 0) ::close(fd);
  fd = -1;

  sent_packets = 0;
  sent_bytes   = 0;
  recv_packets = 0;
  recv_bytes   = 0;
}

bool UDP::send(const void *data,int length,const Address &dest)
{
  int len = sendto(fd,data,length,0,&dest.addr,dest.addr_len);

  if(len > 0){
    sent_packets++;
    sent_bytes += len;
  }

  return(len == length);
}

int UDP::recv(void *data,int length,Address &src)
{
  src.addr_len = sizeof(src.addr);
  int len = recvfrom(fd,data,length,0,&src.addr,&src.addr_len);

  if(len > 0){
    recv_packets++;
    recv_bytes += len;
  }

  return(len);
}

bool UDP::wait(int timeout_ms) const
{
  pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLIN;
  pfd.revents = 0;

  return(poll(&pfd,1,timeout_ms) == 1);
}

}; // namespace Net

//====================================================================//
//  Testing Code
//====================================================================//
// compile with: g++ -g -Wall -O2 -DNETRAW_TEST netraw.cc -o nettest

#ifdef NETRAW_TEST

static const int Size = 32;

const char *hostname = "localhost";
int port = 2000;

void EchoServer()
{
  Net::UDP net;
  Net::Address addr;

  char msg[Size+1];
  net.open(port);

  while(true){
    if(net.wait()){
      int l = net.recv(msg,Size,addr);
      if(l > 0){
        msg[l] = 0;

        printf("["); addr.print();
        printf("]: %s",msg);
        if(msg[l-1] != '\n') printf("\n");

        net.send(msg,l,addr);
      }
    }
  }

  net.close();
}

void EchoClient()
{
  Net::UDP net;
  Net::Address addr,raddr;
  char msg[Size+1];
  int l;

  addr.setHost(hostname,port);
  net.open();

  while(fgets(msg,Size,stdin)){
    l = strlen(msg);
    net.send(msg,l,addr);

    net.wait(1000);
    l = net.recv(msg,Size,raddr);

    msg[l] = 0;
    printf("reply: %s",msg);
  }
}

int main(int argc, char **argv)
{
  char ch;
  bool server = false;
  bool client = false;

  while((ch = getopt(argc, argv, "sc:p:")) != EOF){
    switch(ch){
      case 's': server = true; break;
      case 'c': client = true; hostname = optarg; break;
      case 'p': port = atoi(optarg); break;
    }
  }

  if(server == client) exit(1);

  if(server){
    EchoServer();
  }else{
    EchoClient();
  }
}

#endif
#endif
