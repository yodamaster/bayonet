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
    }
    virtual ~CSocketActorActive () {}

    void SetActionInfoPtr(CActionInfo *pActionInfo)
    {
        if (pActionInfo)
        {
            m_pActionInfoProxy = pActionInfo->get_ptr_proxy();
        }
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
        if (m_pActionInfoProxy.true_ptr())
        {
            m_pActionInfoProxy.true_ptr()->SetDealOver(err_no, GetAliveTimeMs());
        }
    }
protected:
    ptr_proxy<CActionInfo> m_pActionInfoProxy;
};
#endif
