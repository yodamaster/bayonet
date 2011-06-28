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


class CSocketActorBase;
class CEPoller
{
public:
    CEPoller();
    ~CEPoller();

    int Init(int epoll_size,int waittime_ms,int check_time,int gc_maxcount);
    int SetFrame(IFrame* pFrame);
    int AddEpollIO(int fd,unsigned flag);
    int ModEpollIO(int fd,unsigned flag);
    int SetEpollIO(int fd,unsigned flag);
    int DelEpollIO(int fd);
    void AttachSocket(CSocketActorBase* sock);
    void DetachSocket(CSocketActorBase* sock);
    int GetAttachedSocketCount();
    int  LoopForEvent();
    char * GetErrMsg();
protected:
    char            m_szErrMsg[1024];
    int             m_epollFd;              //epoll的句柄
    epoll_event     m_events[EPOLL_EVENTS_MAXSIZE];  //epoll_wait的返回的事件

    int             m_epollSize;
    int             m_waittimeMs;
    int             m_checkTimeMs;
    int             m_gcMaxCount;

    int             m_attachedSocketCount; //被放到map中的socket的个数

    IFrame* m_pFrame;

    //map<int,CSocketActorBase*> m_mapSocketActor;
    map<int,ptr_proxy<CActorBase> > m_mapSocketActorProxy;
};

#endif
