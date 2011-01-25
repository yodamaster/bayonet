/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_base.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-24 20:34:16
#  History:         
=============================================================================*/
#include "socketactor_base.h"

int CSocketActorBase::Init(string ip,int port,int timeout_ms,int protoType)
{
    m_IP = ip;
    m_Port = port;
    m_TimeoutMs = timeout_ms;
    m_ProtoType = protoType;
    return 0;
}
int CSocketActorBase::SetProtoType(int protoType)
{
    m_ProtoType = protoType;
    return 0;
}
int CSocketActorBase::SetTimeout(int timeout_ms)
{
    m_TimeoutMs = timeout_ms;
    return 0;
}

int CSocketActorBase::AttachEpoller(CEPoller* pEpoller)
{
    m_pEpoller = pEpoller;
    if ( m_pEpoller ) m_pEpoller->AttachSocket(this);
        
    return 0;
}
int CSocketActorBase::DetachEpoller()
{
    if ( m_pEpoller ) m_pEpoller->DetachSocket(this);
    m_pEpoller = NULL;

    return 0; 
}
int CSocketActorBase::SetEvent(unsigned event)
{
    if ( !m_pEpoller ) return -1;

    if ( m_pEpoller->ModEpollIO(m_SocketFd,event) < 0 )
        return m_pEpoller->AddEpollIO(m_SocketFd,event);

    return 0;
}
int CSocketActorBase::SetSocketFd(int socketFd)
{
    m_SocketFd = socketFd;
    return 0;
}
int CSocketActorBase::GetSocketFd()
{
    return m_SocketFd;
}
int CSocketActorBase::CheckTimeOut(struct timeval& now_time)
{
    return 0;
}
int CSocketActorBase::OnInit()
{
    if (m_SocketFd < 0)
    {
        
    }
    return HandleInit();
}
int CSocketActorBase::OnFini()
{
    return HandleFini();
}
int CSocketActorBase::OnRecv()
{
    return SOCKET_FSM_RECVOVER;
}

int CSocketActorBase::OnSend()
{
    return SOCKET_FSM_SENDOVER;
}
int CSocketActorBase::OnClose()
{
    return 0;
}

int CSocketActorBase::OnTimeout()
{
    return HandleTimeout(1);
}

int CSocketActorBase::OnError()
{
    return HandleError(1);
}
int CSocketActorBase::OnRecvOver()
{
    return HandleRecvOver(NULL, 0);
}

int CSocketActorBase::OnSendOver()
{
    return HandleSendOver();
}

int CSocketActorBase::OnCloseOver()
{
    return HandleCloseOver();
}
//=============================================================================
int CSocketActorData::OnRecv()
{
    return SOCKET_FSM_RECVOVER;
}
int CSocketActorData::OnSend()
{
    return SOCKET_FSM_SENDOVER;
}
//=============================================================================
