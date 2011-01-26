#ifndef _EPOLLER_H_
#define _EPOLLER_H_
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <map>
#include "socketactor_base.h"
using namespace std;

#define EPOLL_DFT_MAXSIZE   102400
#define NET_ERRMSG_SIZE     1024
#define EPOLL_FD_MAX        10240

class CSocketActorBase;
class CEPoller
{
public:
    CEPoller();
    ~CEPoller();

    int Init(int epoll_size,int waittime_ms,int check_time,int gc_maxcount);
    int AttachFrame(IFrame* pFrame);
    int AddEpollIO(int fd,unsigned flag);
    int ModEpollIO(int fd,unsigned flag);
    int SetEpollIO(int fd,unsigned flag);
    int DelEpollIO(int fd);
    void AttachSocket(CSocketActorBase* sock);
    void DetachSocket(CSocketActorBase* sock);
    int  LoopForEvent();
    char * GetErrMsg();
protected:
    char            m_szErrMsg[NET_ERRMSG_SIZE];
    int             m_epollFd;              //epoll的句柄
    epoll_event     m_events[EPOLL_FD_MAX];  //epoll_wait的返回的事件

    int             m_epollSize;
    int             m_waittimeMs;
    int             m_checkTimeMs;
    int             m_gcMaxCount;

    IFrame* m_pFrame;

    map<int,CSocketActorBase*> m_mapSocketActor;
};

#endif
