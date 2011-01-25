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

int CSocketActorBase::AttachEpoller(CEPoller* epoller)
{
    m_epoller = epoller;
    if ( m_epoller ) m_epoller->AttachSocket(this);
        
    return 0;
}
int CSocketActorBase::DetachEpoller()
{
    if ( m_epoller ) m_epoller->DetachSocket(this);
    m_epoller = NULL;

    return 0; 
}
int CSocketActorBase::SetEvent(unsigned event)
{
    if ( !m_epoller ) return -1;

    if ( m_epoller->ModEpollIO(m_SocketFd,event) < 0 )
        return m_epoller->AddEpollIO(m_SocketFd,event);

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
