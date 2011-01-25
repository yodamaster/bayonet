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
int CSocketActorBase::OnInit()
{
    return HandleInit();
}
int CSocketActorBase::OnRecv()
{
    return 0;
}

int CSocketActorBase::OnSend()
{
    return 0;
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
    return HandleRecvOver(NULL,0,m_pUpperActor);
}

int CSocketActorBase::OnSendOver()
{
    return HandleSendOver();
}

int CSocketActorBase::OnCloseOver()
{
    return HandleCloseOver();
}
