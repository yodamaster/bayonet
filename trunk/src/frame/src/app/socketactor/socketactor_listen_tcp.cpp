/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_tcp.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-27 23:52:01
#  History:         
=============================================================================*/
#include "socketactor_listen_tcp.h"
#include "socketactor_passive_tcp.h"
#include "appactor_base.h"
CSocketActorListenTcp::CSocketActorListenTcp() 
{
    m_BackLog = TCP_BACKLOG_SIZE;
}
CSocketActorListenTcp::~CSocketActorListenTcp() 
{}

void CSocketActorListenTcp::SetBackLog(int backlog)
{
    m_BackLog = backlog;
}
int CSocketActorListenTcp::GetBackLog()
{
    return m_BackLog;
}

int CSocketActorListenTcp::OnInit()
{
    if (m_SocketFd <= 0)
    {
        int optval;
        int listen_fd;
        listen_fd = socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd < 0)
        {   
            error_log("[class:%s]Create socket error:%s\n",Name().c_str(), strerror(errno));
            return SOCKET_FSM_FINI; 
        }   
        struct sockaddr_in myaddr;
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(m_Port);
        myaddr.sin_addr.s_addr = inet_addr(m_IP.c_str());

        optval = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if(bind(listen_fd,(struct sockaddr*)&myaddr,sizeof(struct sockaddr)) < 0)
        {   
            close(listen_fd);
            error_log("[class:%s]CreateListen bind ip:%s port:%d sock:%d err:%s\n",
                    Name().c_str(), m_IP.c_str(),m_Port,listen_fd,strerror(errno));
            return SOCKET_FSM_FINI; 
        }
        m_SocketFd = listen_fd;
    }
    if(listen(m_SocketFd, m_BackLog)<0)
    {
        error_log("[class:%s]CreateListen listen fd:%d err:%s\n",
                Name().c_str(), m_SocketFd,strerror(errno));
        return SOCKET_FSM_CLOSING;
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        error_log("[class:%s]pEpoller is NULL",Name().c_str());
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中

    int ret = ActionHandleInit();
    if (ret != 0)
    {
        error_log("m_pAction HandleInit fail,ret:%d",ret);
        return SOCKET_FSM_CLOSING;
    }

    return SOCKET_FSM_INITOVER;
}
int CSocketActorListenTcp::OnInitOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenTcp::OnFini()
{
    SetGCMark();
    return SOCKET_FSM_FINIOVER;
}
int CSocketActorListenTcp::OnFiniOver()
{
    return SOCKET_FSM_ALLOVER;
}
int CSocketActorListenTcp::OnWaitRecv()
{
    ClearFsmNodes();
    return CSocketActorBase::OnWaitRecv();
}
int CSocketActorListenTcp::OnRecv()
{
    struct sockaddr_in addr;
    int length = sizeof(struct sockaddr_in);
    int clientfd = accept(m_SocketFd,(struct sockaddr *)&addr,(socklen_t*)&length);
    if ( clientfd <= 0 )
    {
        error_log("[class:%s]netlisten accept rtn:%d error:%s\n",
                Name().c_str(),clientfd,strerror(errno));
        return SOCKET_FSM_FINI;
    }

    int flag = fcntl (clientfd, F_GETFL);
    if ( fcntl (clientfd, F_SETFL, O_NONBLOCK | flag) < 0 )
    {
        error_log("[class:%s]HandleConnect set noblock socket:%d error:%s\n",
                Name().c_str(),clientfd,strerror(errno));
        close(clientfd);
        return SOCKET_FSM_FINI;
    }

    CSocketActorBase* pSocketActorAccept = new CSocketActorPassiveTcp();
    pSocketActorAccept->SetIActionPtr(m_pAction);
    pSocketActorAccept->AttachFrame(m_pFrame);
    //Del-Begin by dantezhu in 2011-02-02 19:07:23
    //这里udp会发生，tcp不会发生
    /*if (m_pAppActor)
    {
        CAppActorBase* pAppActor = (CAppActorBase*)m_pAppActor;
        pAppActor->AttachCommu(pSocketActorAccept);
        m_pAppActor = NULL;
    }*/
    //Del-End

    pSocketActorAccept->Init(clientfd,m_TimeoutMs,m_ProtoType);
    pSocketActorAccept->SetKeepcnt(m_bKeepcnt);
    pSocketActorAccept->ChangeState(SOCKET_FSM_INIT);

    return SOCKET_FSM_RECVOVER;
}
int CSocketActorListenTcp::OnRecvOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenTcp::OnWaitSend() 
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenTcp::OnSend()
{
    return SOCKET_FSM_SENDOVER;
}
int CSocketActorListenTcp::OnSendOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenTcp::OnClose()
{
    DetachFromEpoller();
    if (m_SocketFd>0)
    {
        close(m_SocketFd);
        m_SocketFd = -1;
    }
    return SOCKET_FSM_CLOSEOVER;
}
int CSocketActorListenTcp::OnCloseOver()
{
    return SOCKET_FSM_FINI;
}
bool CSocketActorListenTcp::IsTimeOut(struct timeval& now_time)
{
    //永不超时
    return false;
}
