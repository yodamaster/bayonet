/*=============================================================================
#
#     FileName: epoller.h
#         Desc: epoller监听，并触发时间
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-30 00:16:32
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-30 00:16:32 | initialization
#
=============================================================================*/
#ifndef _EPOLLER_H_20110630001647_
#define _EPOLLER_H_20110630001647_

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
#include "comm_def.h"
namespace bayonet {

using namespace std;


class CSocketActorBase;
class CEPoller
{
public:
    CEPoller();
    ~CEPoller();

    int Init(int epoll_size,int wait_time_ms,int check_sock_interval_time_ms,int check_app_interval_time_ms,int gc_maxcount);
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

    void AttachAppActor(CActorBase* pAppActor);
    void DetachAppActor(CActorBase* pAppActor);

    void CheckTimeOutSocketActor();
    void CheckTimeOutAppActor();
protected:
    char            m_szErrMsg[1024];
    int             m_epollFd;              //epoll的句柄
    epoll_event     m_events[EPOLL_EVENTS_MAXSIZE];  //epoll_wait的返回的事件

    int             m_epollSize;
    int             m_waitTimeMs;
    int             m_checkSockIntervalTimeMs;
    int             m_checkAppIntervalTimeMs;
    int             m_gcMaxCount;

    int             m_attachedSocketCount; //被放到map中的socket的个数

    IFrame* m_pFrame;

    //map<int,CSocketActorBase*> m_mapSocketActor;
    map<int,ptr_proxy<CActorBase> > m_mapSocketActorProxy;
    list<ptr_proxy<CActorBase> > m_listAppActorProxy;
};
}

#endif
