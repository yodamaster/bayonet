/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active_sendrecv.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-30 13:56:54
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_SENDRECV_H_
#define _SOCKETACTOR_ACTIVE_SENDRECV_H_
#include "socketactor_active.h"
#include "action_info.h"
class CSocketActorActiveSendRecv : public CSocketActorActive
{
public:
    CSocketActorActiveSendRecv () {
    }
    virtual ~CSocketActorActiveSendRecv () {}

    virtual int OnClose()
    {
        int count = m_vecFsmNodes.size();
        if (!(count > 2 && m_vecFsmNodes[count-2].fsm->GetStateID() == SOCKET_FSM_RECVOVER))
        {
            //非正常关闭
            SetDealOver(ESocketHangup);
        }
        return CSocketActorActive::OnClose();
    }

    virtual int OnSendOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnRecvOver()
    {
        SetDealOver(0);
        return SOCKET_FSM_CLOSING;
    }
};
#endif
