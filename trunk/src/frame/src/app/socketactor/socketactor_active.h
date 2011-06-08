/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-28 11:34:59
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_H_
#define _SOCKETACTOR_ACTIVE_H_
#include "socketactor_data.h"
#include "action_info.h"
#include "timer.h"
class CSocketActorActive : public CSocketActorData
{
public:
    CSocketActorActive () {
        m_pActionInfo = NULL;
        m_Timer.Start();
    }
    virtual ~CSocketActorActive () {}

    void SetActionInfoPtr(CActionInfo *pActionInfo)
    {
        m_pActionInfo = pActionInfo;
    }

    virtual int OnTimeout()
    {
        SetDealOver(ESocketTimeout);
        return SOCKET_FSM_CLOSING;
    }

    virtual int OnError()
    {
        SetDealOver(ESocketError);
        return SOCKET_FSM_CLOSING;
    }

protected:
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITSEND;
    }
    virtual int OnSendOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnRecvOver()
    {
        return SOCKET_FSM_CLOSING;
    }
    virtual void SetDealOver(int err_no)
    {
        if (m_pActionInfo)
        {
            m_pActionInfo->SetDealOver(err_no, m_Timer.GetPastTime());
        }
    }
protected:
    CActionInfo *m_pActionInfo;
    CTimer m_Timer;
};
#endif
