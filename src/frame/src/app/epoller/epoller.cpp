/*=============================================================================
#
#     FileName: epoller.cpp
#         Desc: 
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-08-04 15:26:08
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-08-04 15:26:08 | initialization
#
=============================================================================*/

#include "epoller.h"
#include "appactor_base.h"
namespace bayonet {

CEPoller::CEPoller()
{
    m_epollFd = -1;
    m_pFrame = NULL;
    m_epollSize = EPOLL_FD_MAXSIZE;
    m_waitTimeMs = EPOLL_WAIT_TIMEMS;
    m_checkSockIntervalTimeMs = CHECK_SOCK_INTERVAL_TIMEMS;
    m_checkAppIntervalTimeMs = CHECK_APP_INTERVAL_TIMEMS;
    m_attachedSocketCount = 0;
    m_gcMaxCount = GC_MAX_COUNT;
}

CEPoller::~CEPoller()
{
}
int CEPoller::SetFrame(IFrame* pFrame)
{
    m_pFrame = pFrame;
    return 0;
}
int CEPoller::Init(int epoll_size,int wait_time_ms,int check_sock_interval_time_ms,int check_app_interval_time_ms,int gc_maxcount)
{
    m_epollSize = epoll_size;
    m_waitTimeMs = wait_time_ms;
    m_checkSockIntervalTimeMs = check_sock_interval_time_ms;
    m_checkAppIntervalTimeMs = check_app_interval_time_ms;
    m_gcMaxCount = gc_maxcount;

    m_epollFd = epoll_create(m_epollSize);
    if ( m_epollFd <= 0 )
    {
        snprintf(m_szErrMsg,sizeof(m_szErrMsg),"epoller init error,size:%d,error:%s\n",m_epollSize,strerror(errno));
        return -1;
    }

    return 0;
}

void CEPoller::AttachSocket(CSocketActorBase* pSocketActor)
{
    if (pSocketActor == NULL)
    {
        return;
    }

    int fd = pSocketActor->GetSocketFd();
    if ( fd > 0 )
    {
        m_mapSocketActorProxy[fd]=pSocketActor->get_ptr_proxy();
    }
}

void CEPoller::DetachSocket(CSocketActorBase* pSocketActor)
{
    if (pSocketActor == NULL)
    {
        return;
    }
    int fd = pSocketActor->GetSocketFd();
    if ( fd > 0 )
    {
        DelEpollIO(fd);
        //这里加上性能会稍微好一点，处理量的波动也会小一些
        m_mapSocketActorProxy.erase(fd);
    }
}

int CEPoller::GetAttachedSocketCount()
{
    return m_attachedSocketCount;
}

int CEPoller::LoopForEvent()
{
    int fd;
    int nfds;
    CSocketActorBase*  pSocketActor = NULL;
    CEpollEvent t_event;
    struct timeval prev_tm_sock,prev_tm_app;
    struct timeval next_tm;
    long    use_time_usec;
    gettimeofday(&prev_tm_sock,NULL);
    gettimeofday(&prev_tm_app,NULL);

    for(;;)
    {
        nfds = epoll_wait(m_epollFd, m_events, EPOLL_EVENTS_MAXSIZE, m_waitTimeMs);

        if (nfds < 0)
        {
            if ( errno == EINTR )
                continue;

            snprintf(m_szErrMsg,sizeof(m_szErrMsg),"epoll-wait rtn:%d error:%s\n",nfds,strerror(errno));
            return -1;
        }

        next_tm = CRecordTime::Ins()->record_time();

        for( int i=0;i<nfds;i++ )
        {
            fd = m_events[i].data.fd;
            pSocketActor = (CSocketActorBase*)(m_mapSocketActorProxy[fd].true_ptr());
            if ( pSocketActor == NULL )
            {
                DelEpollIO(fd); close(fd);
                continue;
            }
            t_event.evt = m_events[i].events;
            pSocketActor->HandleEvent(&t_event);
        }

        use_time_usec = (next_tm.tv_sec - prev_tm_sock.tv_sec)*1000000 +
            (next_tm.tv_usec - prev_tm_sock.tv_usec);
        if ( use_time_usec > (m_checkSockIntervalTimeMs*1000))
        {
            CheckTimeOutSocketActor();
            prev_tm_sock = next_tm;
        }

        use_time_usec = (next_tm.tv_sec - prev_tm_app.tv_sec)*1000000 +
            (next_tm.tv_usec - prev_tm_app.tv_usec);
        if ( use_time_usec > (m_checkAppIntervalTimeMs*1000))
        {
            CheckTimeOutAppActor();
            prev_tm_app = next_tm;
        }

        //进行垃圾回收
        if (m_pFrame->GetNeedGCCount() > m_gcMaxCount)
        {
            m_pFrame->GCActors();
        }
    }
}

char * CEPoller::GetErrMsg()
{
    return m_szErrMsg;
}

int CEPoller::SetEpollIO(int fd,unsigned flag)
{
    if ( ModEpollIO(fd, flag) < 0 )    
    {
        if ( AddEpollIO(fd, flag) < 0 )
        {
            snprintf(m_szErrMsg,sizeof(m_szErrMsg),"epoll_ctl fd:%d err:%s\n",fd,strerror(errno));
            return -1;
        }
    }

    return 0;
}

int CEPoller::AddEpollIO(int fd,unsigned flag)
{
    epoll_event ev;
    memset((void*)&ev,0,sizeof(epoll_event));
    ev.data.fd = fd;
    ev.events = flag;

    if ( epoll_ctl(m_epollFd, EPOLL_CTL_ADD , fd, &ev) < 0 )
    {
        return -1;
    }

    ++m_attachedSocketCount;
    return 0;
}

int CEPoller::ModEpollIO(int fd,unsigned flag)
{
    epoll_event ev;
    memset((void*)&ev,0,sizeof(epoll_event));
    ev.data.fd = fd;
    ev.events = flag;

    if ( epoll_ctl(m_epollFd, EPOLL_CTL_MOD , fd, &ev) < 0 )    
    {
        return -1;
    }

    return 0;
}

int CEPoller::DelEpollIO(int fd)
{
    epoll_event ev;
    memset((void*)&ev,0,sizeof(epoll_event));
    ev.data.fd = fd;
    ev.events = 0;
    if ( epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev) < 0 )
    {
        return -1;
    }

    --m_attachedSocketCount;
    return 0;
}

void CEPoller::AttachAppActor(CActorBase* pAppActor)
{
    if (pAppActor == NULL)
    {
        return;
    }
    m_listAppActorProxy.push_back(pAppActor->get_ptr_proxy());
}
void CEPoller::DetachAppActor(CActorBase* pAppActor)
{
    //啥也不做，否则会有查找效率
}
void CEPoller::CheckTimeOutSocketActor()
{
    for(map<int, ptr_proxy<CActorBase> >::iterator it = m_mapSocketActorProxy.begin(); it != m_mapSocketActorProxy.end();)
    {
        //这样写，就可以自由的删掉自己了
        map<int, ptr_proxy<CActorBase> >::iterator tempIt = it;
        it++;

        CSocketActorBase* pActor = (CSocketActorBase*)(tempIt->second.true_ptr());
        if (pActor == NULL || pActor->GetGCMark() || pActor->GetSocketFd()<=0) //已经被删除，或者已经被标记为GC
        {
            m_mapSocketActorProxy.erase(tempIt);
            continue;
        }
        pActor->CheckTimeOut();
    }
}
void CEPoller::CheckTimeOutAppActor()
{
    for(list<ptr_proxy<CActorBase> >::iterator it = m_listAppActorProxy.begin(); it != m_listAppActorProxy.end();)
    {
        CAppActorBase* pActor = (CAppActorBase*)(it->true_ptr());
        if (pActor == NULL || pActor->GetGCMark()) //已经被删除，或者已经被标记为GC
        {
            it = m_listAppActorProxy.erase(it);
            continue;
        }
        pActor->CheckTimeOut();
        ++it;
    }
}
}
