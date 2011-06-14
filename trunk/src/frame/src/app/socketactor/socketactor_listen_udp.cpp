/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_udp.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 00:48:05
#  History:         
=============================================================================*/
#include "socketactor_listen_udp.h"
#include "appactor_base.h"

int CSocketActorListenUdp::OnInit()
{
    if (m_SocketFd <= 0)
    {
        int optval;
        int listen_fd;
        listen_fd = socket(AF_INET,SOCK_DGRAM,0);
        if(listen_fd < 0)
        {   
            error_log("[class:%s]Create socket error:%s\n",Name().c_str(),strerror(errno));
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
                    Name().c_str(),m_IP.c_str(),m_Port,listen_fd,strerror(errno));
            return SOCKET_FSM_FINI; 
        }
        m_SocketFd = listen_fd;
        int ret = m_pNetHandler->Init(m_SocketFd);
        if (ret)
        {
            error_log("[class:%s]nethandler init error:%d",Name().c_str(),ret);
            return SOCKET_FSM_CLOSING; 
        }
    }
    CEPoller* pEpoller = GetEpoller();
    if (!pEpoller)
    {
        error_log("[class:%s]pEpoller is NULL",Name().c_str());
        return SOCKET_FSM_CLOSING;
    }
    pEpoller->AttachSocket(this);//加入到epoll中

    if (m_pAction == NULL)
    {
        return SOCKET_FSM_CLOSING;
    }
    else
    {
        int ret = m_pAction->HandleInit(this, m_pAppActorProxy.true_ptr());
        if (ret != 0)
        {
            error_log("m_pAction HandleInit fail,ret:%d",ret);
            return SOCKET_FSM_CLOSING;
        }
    }

    return SOCKET_FSM_INITOVER;
}
int CSocketActorListenUdp::OnRecvOver()
{
    CreatePassiveActor();
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorListenUdp::OnWaitSend()
{
    CSocketActorPassiveUdp* pSocketActorAccept = CreatePassiveActor();
    if (pSocketActorAccept)
    {
        pSocketActorAccept->ChangeState(SOCKET_FSM_WAITSEND);
    }
    return SOCKET_FSM_WAITRECV;
}

bool CSocketActorListenUdp::IsTimeOut(struct timeval& now_time)
{
    //永不超时
    return false;
}
CSocketActorPassiveUdp* CSocketActorListenUdp::CreatePassiveActor()
{
    if (m_pNetHandler->GetClientIp().size()>0 && m_pNetHandler->GetClientPort()>0)
    {
        CSocketActorPassiveUdp * pSocketActorAccept = new CSocketActorPassiveUdp();
        pSocketActorAccept->AttachFrame(m_pFrame);
        pSocketActorAccept->SetIActionPtr(m_pAction);
        if (m_pAppActorProxy.true_ptr())
        {
            CAppActorBase* pAppActor = (CAppActorBase*)m_pAppActorProxy.true_ptr();
            pAppActor->AttachCommu(pSocketActorAccept);
        }
        //重新指向一个新的
        m_pAppActorProxy = NULL;

        trace_log("%s,%d",m_pNetHandler->GetClientIp().c_str(),m_pNetHandler->GetClientPort());
        pSocketActorAccept->Init(m_pNetHandler->GetClientIp(),m_pNetHandler->GetClientPort(),m_TimeoutMs,m_ProtoType);
        pSocketActorAccept->ChangeState(SOCKET_FSM_INIT);
        return pSocketActorAccept;
    }
    return NULL;
}
