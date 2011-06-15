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
#include "bayonet_frame.h"

int CSocketActorBase::HandleEvent(IEvent* pEvent)
{
    CEpollEvent* pEpollEvent = (CEpollEvent*)pEvent;
    unsigned evt = pEpollEvent->evt;
    int ret = 0;
    if ( evt&EPOLLIN )
    {
        ret = ChangeState(SOCKET_FSM_RECVING);                         
    }
    else if ( evt&EPOLLOUT )
    {
        ret = ChangeState(SOCKET_FSM_SENDING);
    }
    else if ( evt&EPOLLHUP )
    {
        ret = ChangeState(SOCKET_FSM_CLOSING);
    }
    else if ( evt&EPOLLERR )
    {
        ret = ChangeState(SOCKET_FSM_ERROR);
    }
    else
    {
        ret = ChangeState(SOCKET_FSM_ERROR);
    }
    return ret;
}
void CSocketActorBase::SetAppActor(CActorBase* pActor)
{
    if (pActor)
    {
        m_pAppActorProxy = pActor->get_ptr_proxy();
    }
}

int CSocketActorBase::Init(string ip,int port,int timeout_ms,int protoType)
{
    m_IP = ip;
    m_Port = port;
    m_TimeoutMs = timeout_ms;
    m_ProtoType = protoType;
    return 0;
}
int CSocketActorBase::Init(int socketFd,int timeout_ms,int protoType)
{
    m_SocketFd = socketFd;
    m_TimeoutMs = timeout_ms;
    m_ProtoType = protoType;
    return 0;
}
void CSocketActorBase::SetIActionPtr(IAction *pAction)
{
    m_pAction = pAction;
}

int CSocketActorBase::SetEvent(unsigned event)
{
    CEPoller * pEpoller = GetEpoller();
    if (!pEpoller) 
    {
        error_log("[class:%s]pEpoller is NULL",Name().c_str());
        return -1;
    }

    /*if ( pEpoller->ModEpollIO(m_SocketFd,event) < 0 )
    {
        return pEpoller->AddEpollIO(m_SocketFd,event);
    }*/

    return pEpoller->SetEpollIO(m_SocketFd,event);
}
int CSocketActorBase::GetSocketFd()
{
    return m_SocketFd;
}
void CSocketActorBase::SetKeepcnt(bool bKeepcnt)
{
    m_bKeepcnt = bKeepcnt;
}

int CSocketActorBase::ResizeRecvBuf(int singleBufSize, int initBufSize) 
{
    return 0;
}

int CSocketActorBase::CheckTimeOut(struct timeval& now_time)
{
    if (GetGCMark())
    {
        return 0;
    }
    //默认是永不超时的
    if (IsTimeOut(now_time))
    {
        ChangeState(SOCKET_FSM_TIMEOUT);
    }
    return 0;
}
bool CSocketActorBase::IsTimeOut(struct timeval& now_time)
{
    if (m_TimeoutMs < 0)
    {
        //永不超时
        return false;
    }

    if (m_ProtoType == PROTO_TYPE_TCP && m_bKeepcnt)
    {
        //长连接
        return false;
    }

    int pastTime = m_aliveTimer.GetPastTime();

    if (pastTime > m_TimeoutMs)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int CSocketActorBase::OnTimeout()
{
    return SOCKET_FSM_CLOSING;
}

int CSocketActorBase::OnError()
{
    return SOCKET_FSM_CLOSING;
}

int CSocketActorBase::OnWaitRecv() {
    SetEvent(EPOLLIN|EPOLLHUP|EPOLLERR);
    return 0;
}

int CSocketActorBase::OnWaitSend() {
    SetEvent(EPOLLOUT|EPOLLHUP|EPOLLERR);
    return 0;
}

int CSocketActorBase::OnWaitClose() {
    SetEvent(EPOLLHUP|EPOLLERR);
    return 0;
}

CEPoller* CSocketActorBase::GetEpoller()
{
    IFrame* pFrame = GetFrame();
    if (!pFrame)
    {
        return NULL;
    }
    CBayonetFrame* pBayonetFrame = (CBayonetFrame*)pFrame;
    CEPoller * pEpoller = pBayonetFrame->GetEpoller();
    return pEpoller;
}
int CSocketActorBase::DetachFromEpoller()
{
    CEPoller* pEpoller = GetEpoller();
    if (pEpoller)
    {
        pEpoller->DetachSocket(this);
        return 0;
    }
    return 1;
}

int CSocketActorBase::ActionHandleInit()
{
    if (m_pAction == NULL)
    {
        error_log("m_pAction is null");
        return -1;
    }
    return m_pAction->HandleInit(this,(CAppActorBase*)m_pAppActorProxy.true_ptr());
}
int CSocketActorBase::ActionHandleEncodeSendBuf(
        string & strSendBuf,
        int &len)
{
    if (m_pAction == NULL)
    {
        error_log("m_pAction is null");
        return -1;
    }
    return m_pAction->HandleEncodeSendBuf(this,(CAppActorBase*)m_pAppActorProxy.true_ptr(),strSendBuf,len);
}
int CSocketActorBase::ActionHandleInput(
        const char *buf,
        int len)
{
    if (m_pAction == NULL)
    {
        error_log("m_pAction is null");
        return -1;
    }
    return m_pAction->HandleInput(this,(CAppActorBase*)m_pAppActorProxy.true_ptr(),buf,len);
}
int CSocketActorBase::ActionHandleDecodeRecvBuf(const char *buf, int len)
{
    if (m_pAction == NULL)
    {
        error_log("m_pAction is null");
        return -1;
    }
    return m_pAction->HandleDecodeRecvBuf(this,(CAppActorBase*)m_pAppActorProxy.true_ptr(),buf,len);
}
