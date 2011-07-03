/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-28 11:34:59
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_H_20110630001712_
#define _SOCKETACTOR_ACTIVE_H_20110630001712_

#include "socketactor_data.h"
#include "action_info.h"
#include "timer.h"
class CSocketActorActive : public CSocketActorData
{
public:
    CSocketActorActive ();
    virtual ~CSocketActorActive ();

    void SetActionInfoPtr(CActionInfo *pActionInfo);

    virtual int OnTimeout();

    virtual int OnError();

    virtual int OnInitOver();
    virtual int OnSendOver();
    virtual int OnRecvOver();

    virtual void SetDealOver(int err_no);
protected:
    ptr_proxy<CActionInfo> m_pActionInfoProxy;
};

class CSocketActorActiveSendOnly : public CSocketActorActive
{
public:
    CSocketActorActiveSendOnly ();
    virtual ~CSocketActorActiveSendOnly ();

    virtual int OnCloseOver();
    virtual int OnSendOver();
    virtual int OnRecvOver();
};

class CSocketActorActiveSendRecv : public CSocketActorActive
{
public:
    CSocketActorActiveSendRecv ();
    virtual ~CSocketActorActiveSendRecv ();

    virtual int OnCloseOver();
    virtual int OnSendOver();
    virtual int OnRecvOver();
};
#endif
