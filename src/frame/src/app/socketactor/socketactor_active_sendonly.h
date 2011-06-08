/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active_sendonly.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-30 13:58:20
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_SENDONLY_H_
#define _SOCKETACTOR_ACTIVE_SENDONLY_H_
#include "socketactor_data.h"
#include "action_info.h"
#include "timer.h"
class CSocketActorActiveSendOnly : public CSocketActorActive
{
public:
    CSocketActorActiveSendOnly () {
    }
    virtual ~CSocketActorActiveSendOnly () {}
protected:
    virtual int OnClose()
    {
        int count = m_vecStates.size();
        if (!(count > 2 && m_vecStates[count-2] == SOCKET_FSM_SENDOVER))
        {
            //非正常关闭
            SetDealOver(ESocketHangup);
        }
        return CSocketActorActive::OnClose();
    }
    virtual int OnSendOver()
    {
        SetDealOver(0);
        return SOCKET_FSM_CLOSING;
    }
    virtual int OnRecvOver()
    {
        return SOCKET_FSM_CLOSING;
    }
};
#endif
