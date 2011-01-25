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

int CSocketActorBase::SetEvent(int event)
{
    return 0;
}
int CSocketActorBase::SetSocketId(int socketId)
{
    m_SocketId = socketId;
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
