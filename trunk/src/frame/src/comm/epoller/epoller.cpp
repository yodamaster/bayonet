#include "epoller.h"

CEPoller::CEPoller()
{
    m_epollFd = -1;
    m_pFrame = NULL;
    m_epollSize = EPOLL_FD_MAXSIZE;
    m_waittimeMs = EPOLL_WAIT_TIMEMS;
    m_checkTimeMs = CHECK_INTERVAL_MS;
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
int CEPoller::Init(int epoll_size,int waittime_ms,int checktime_ms,int gc_maxcount)
{
    m_epollSize = epoll_size;
    m_waittimeMs = waittime_ms;
    m_checkTimeMs = checktime_ms;
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
        m_mapSocketActorProxy[fd]=pSocketActor->get_ptr_proxy();

    return ;
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
        m_mapSocketActorProxy.erase(fd);
    }

    return ;
}

int CEPoller::LoopForEvent()
{
    int fd;
    int nfds;
    CSocketActorBase*  pSocketActor = NULL;
    CEpollEvent t_event;
    struct timeval prev_tm;
    struct timeval next_tm;
    long    use_time_usec;
    gettimeofday(&prev_tm,NULL);

    for(;;)
    {
        nfds = epoll_wait(m_epollFd, m_events, EPOLL_EVENTS_MAXSIZE, m_waittimeMs);

        if (nfds < 0)
        {
            if ( errno == EINTR )
                continue;

            snprintf(m_szErrMsg,sizeof(m_szErrMsg),"epoll-wait rtn:%d error:%s\n",nfds,strerror(errno));
            return -1;
        }

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

        gettimeofday(&next_tm,NULL);
        use_time_usec = (next_tm.tv_sec - prev_tm.tv_sec)*1000000 +
            (next_tm.tv_usec - prev_tm.tv_usec);
        if ( use_time_usec > (m_checkTimeMs*1000))
        {
            for(map<int, ptr_proxy<CActorBase> >::iterator it = m_mapSocketActorProxy.begin(); it != m_mapSocketActorProxy.end();)
            {
                //这样写，就可以自由的删掉自己了
                map<int, ptr_proxy<CActorBase> >::iterator tempIt = it;
                it++;

                CSocketActorBase* pActor = (CSocketActorBase*)(tempIt->second.true_ptr());
                if (pActor == NULL)
                {
                    m_mapSocketActorProxy.erase(tempIt);
                }
                pActor->CheckTimeOut(next_tm);
            }
            prev_tm = next_tm;
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
    epoll_event ev;
    memset((void*)&ev,0,sizeof(epoll_event));
    ev.data.fd = fd;
    ev.events = flag|EPOLLHUP|EPOLLERR;

    if ( epoll_ctl(m_epollFd, EPOLL_CTL_MOD , fd, &ev) < 0 )    
    {
        if ( epoll_ctl(m_epollFd, EPOLL_CTL_ADD , fd, &ev) < 0 )
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
        return -1;

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
        return -1;

    return 0;
}
