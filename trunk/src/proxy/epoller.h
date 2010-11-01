/*=============================================================================
#  Author:          DanteZhu - http://www.vimer.cn
#  Email:           dantezhu@vip.qq.com
#  FileName:        epoller.h
#  Description:     定义epoll专用的socket和epoller池
#  Version:         1.0
#  LastChange:      2010-11-02 00:00:34
#  History:
=============================================================================*/
#ifndef _EPOLLER_H_
#define _EPOLLER_H_
#include <iostream> 
#include <string> 
#include <vector> 
#include <map> 
#include <tr1/unordered_map> 

using namespace std; 

#define hashmap std::tr1::unordered_map 

#define NET_ERRMSG_SIZE     512
#define EPOLL_FD_MAX        10240


class CEpollSocket;
class CEpoller;

class CEpollSocket
{
public:
    CEpollSocket (int fd, CEpoller* _epoller);
    virtual ~CEpollSocket ();

    virtual int OnRecv(){return 0;};
    virtual int OnSend(){return 0;};
    virtual int OnClose(){return 0;};
    virtual int OnError(){return 0;};

    int GetSockHandle();
    int SetSockHandle(int fd);
    int AttachEpoller(CEPoller* epoller);
    int DetachEpoller();
    
    int SetEvent(unsigned event);
    int DropSocket();

private:
    int m_socket_fd;//存储的描述符
    CEpoller* m_epoller;//存储的epoller管理器
};

class CEpoller
{
public:
    CEpoller ();
    virtual ~CEpoller ();

    int Create(int maxsize);

    void AttachSocket(CEpollSocket* sock);
    void DetachSocket(CEpollSocket* sock);

    int  LoopForEvent(int timeout);

    char * GetErrMsg();

private:
    hashmap<int,CEpollSocket*> m_MapSockets;

    int m_EpollFd;
    epoll_event m_Events[EPOLL_FD_MAX];

    char m_ErrMsg[NET_ERRMSG_SIZE];
    int m_MaxSize;
};
#endif
