/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        net_handler.h
#  Description:     socket的handler
#  Version:         1.0
#  LastChange:      2011-01-27 11:49:52
#  History:         
=============================================================================*/
#ifndef _NET_HANDLER_H_20110630001705_
#define _NET_HANDLER_H_20110630001705_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <error.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

namespace bayonet {
using namespace std;
class CNetHandlerBase
{
public:
    CNetHandlerBase ();
    virtual ~CNetHandlerBase ();

    int Init(string ip,int port);
    int Init(int socketFd);

    int GetSocketFd();

    string GetClientIp();

    int GetClientPort();

    int SetNoBlock(int socketFd);
    
    int Close();

    virtual int Create()=0;
    virtual int Send(char* pBuf,int bufLen)=0;
    virtual int Recv(char* pBuf,int bufSize)=0;

protected:
    string m_IP;
    int m_Port;

    int m_SocketFd;

    string m_ClientIp;
    int m_ClientPort;
};
//=============================================================================
class CNetHandlerTcp : public CNetHandlerBase
{
public:
    virtual int Create();
    virtual int Send(char* pBuf,int bufLen);
    virtual int Recv(char* pBuf,int bufSize);

protected:
    int netConnect();
};
class CNetHandlerUdp : public CNetHandlerBase
{
public:
    virtual int Create();
    virtual int Send(char* pBuf,int bufLen);
    virtual int Recv(char* pBuf,int bufSize);
};
}
//=============================================================================
#endif
