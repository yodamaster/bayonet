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
#include "socketactor_passive.h"
#include "appactor_base.h"
#include "fl_log.h"
namespace bayonet {
CSocketActorListenTcp::CSocketActorListenTcp() 
{
    m_BackLog = TCP_BACKLOG_SIZE;
    m_attachedSocketMaxSize = ATTACHED_SOCKET_MAXSIZE;
}
CSocketActorListenTcp::~CSocketActorListenTcp() 
{}

int CSocketActorListenTcp::Init(string ip,int port,int timeout_ms,int protoType)
{
    int ret = CSocketActorBase::Init(ip, port, timeout_ms, protoType);
    if (ret != 0)
    {
        return ret;
    }
    int optval;
    m_SocketFd = socket(AF_INET,SOCK_STREAM,0);
    if(m_SocketFd < 0)
    {   
        byt_error_log("[class:%s]Create socket error:%s\n",Name().c_str(), strerror(errno));
        return -1; 
    }   
    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(m_Port);
    myaddr.sin_addr.s_addr = inet_addr(m_IP.c_str());

    optval = 1;
    setsockopt(m_SocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if(bind(m_SocketFd,(struct sockaddr*)&myaddr,sizeof(struct sockaddr)) < 0)
    {   
        //close(listen_fd);
        //到CLOSING状态会帮你关闭掉
        byt_error_log("[class:%s]CreateListen bind ip:%s port:%d sock:%d err:%s\n",
                Name().c_str(), m_IP.c_str(),m_Port,m_SocketFd,strerror(errno));
        return -2; 
    }
    return 0;
}

void CSocketActorListenTcp::SetBackLog(int backlog)
{
    m_BackLog = backlog;
}
int CSocketActorListenTcp::GetBackLog()
{
    return m_BackLog;
}
void CSocketActorListenTcp::SetAttachedSocketMaxSize(int attachedSocketMaxSize)
{
    m_attachedSocketMaxSize = attachedSocketMaxSize;
}

int CSocketActorListenTcp::GetAttachedSocketMaxSize()
{
    return m_attachedSocketMaxSize;
}

int CSocketActorListenTcp::OnInit()
{
    if(listen(m_SocketFd, m_BackLog)<0)
    {
        byt_error_log("[class:%s]CreateListen listen fd:%d err:%s\n",
                Name().c_str(), m_SocketFd,strerror(errno));
        return SOCKET_FSM_CLOSING;
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        byt_error_log("[class:%s]pEpoller is NULL",Name().c_str());
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中

    return SOCKET_FSM_INITOVER;
}
int CSocketActorListenTcp::OnInitOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenTcp::OnWaitRecv()
{
    ClearFsmNodes();
    return CSocketActorBase::OnWaitRecv();
}
int CSocketActorListenTcp::OnRecv()
{
    CEPoller* pEpoller = GetEpoller();
    if (pEpoller)
    {
        if (pEpoller->GetAttachedSocketCount() > m_attachedSocketMaxSize)
        {
            byt_error_log("attachedSocketCount has reach the max:%d/%d",
                      pEpoller->GetAttachedSocketCount(),m_attachedSocketMaxSize);
            return SOCKET_FSM_WAITRECV;
        }
    }

    struct sockaddr_in addr;
    int length = sizeof(struct sockaddr_in);
    int clientfd = accept(m_SocketFd,(struct sockaddr *)&addr,(socklen_t*)&length);
    if ( clientfd <= 0 )
    {
        byt_error_log("[class:%s]netlisten accept rtn:%d error:%s\n",
                Name().c_str(),clientfd,strerror(errno));
        return SOCKET_FSM_WAITRECV;
    }

    int flag = fcntl (clientfd, F_GETFL);
    if ( fcntl (clientfd, F_SETFL, O_NONBLOCK | flag) < 0 )
    {
        byt_error_log("[class:%s]HandleConnect set noblock socket:%d error:%s\n",
                Name().c_str(),clientfd,strerror(errno));
        close(clientfd);
        return SOCKET_FSM_WAITRECV;
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
bool CSocketActorListenTcp::IsTimeOut()
{
    //永不超时
    return false;
}
}
