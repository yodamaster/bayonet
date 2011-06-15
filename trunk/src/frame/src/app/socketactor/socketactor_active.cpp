/*=============================================================================
#
#     FileName: socketactor_active.cpp
#         Desc: 
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-15 10:21:20
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-15 10:21:20 | initialization
#
=============================================================================*/
#include "socketactor_active.h"
CSocketActorActive::CSocketActorActive () {
}
CSocketActorActive::~CSocketActorActive () {}

void CSocketActorActive::SetActionInfoPtr(CActionInfo *pActionInfo)
{
    if (pActionInfo)
    {
        m_pActionInfoProxy = pActionInfo->get_ptr_proxy();
    }
}

int CSocketActorActive::CSocketActorActive::OnTimeout()
{
    SetDealOver(ESocketTimeout);
    return SOCKET_FSM_CLOSING;
}

int CSocketActorActive::OnError()
{
    SetDealOver(ESocketError);
    return SOCKET_FSM_CLOSING;
}

int CSocketActorActive::OnInitOver()
{
    return SOCKET_FSM_WAITSEND;
}
int CSocketActorActive::OnSendOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorActive::OnRecvOver()
{
    return SOCKET_FSM_CLOSING;
}
void CSocketActorActive::SetDealOver(int err_no)
{
    if (m_pActionInfoProxy.true_ptr())
    {
        m_pActionInfoProxy.true_ptr()->SetDealOver(err_no, GetAliveTimeMs());
    }
    if (m_pAppActorProxy.true_ptr())
    {
        m_pAppActorProxy.true_ptr()->ProcessState();
    }
}
//=============================================================================
CSocketActorActiveSendOnly::CSocketActorActiveSendOnly () {
}
CSocketActorActiveSendOnly::~CSocketActorActiveSendOnly () {}

int CSocketActorActiveSendOnly::OnCloseOver()
{
    int count = m_vecFsmNodes.size();
    if (!(count > 3 && m_vecFsmNodes[count-3].fsm->GetStateID() == SOCKET_FSM_SENDOVER))
    {
        //非正常关闭
        SetDealOver(ESocketHangup);
    }
    return CSocketActorActive::OnCloseOver();
}
int CSocketActorActiveSendOnly::OnSendOver()
{
    SetDealOver(0);
    return SOCKET_FSM_CLOSING;
}
int CSocketActorActiveSendOnly::OnRecvOver()
{
    return SOCKET_FSM_CLOSING;
}
//=============================================================================
CSocketActorActiveSendRecv::CSocketActorActiveSendRecv () {
}
CSocketActorActiveSendRecv::~CSocketActorActiveSendRecv () {}

int CSocketActorActiveSendRecv::OnCloseOver()
{
    int count = m_vecFsmNodes.size();
    if (!(count > 3 && m_vecFsmNodes[count-3].fsm->GetStateID() == SOCKET_FSM_RECVOVER))
    {
        //非正常关闭
        SetDealOver(ESocketHangup);
    }
    return CSocketActorActive::OnCloseOver();
}

int CSocketActorActiveSendRecv::OnSendOver()
{
    return SOCKET_FSM_WAITRECV;
}
int CSocketActorActiveSendRecv::OnRecvOver()
{
    SetDealOver(0);
    return SOCKET_FSM_CLOSING;
}
