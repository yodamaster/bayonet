/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive.h
#  Description:     被动TCP
#  Version:         1.0
#  LastChange:      2011-01-28 17:00:16
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_H_
#define _SOCKETACTOR_PASSIVE_H_
#include "socketactor_data.h"
class CSocketActorPassive : public CSocketActorData
{
public:
    virtual ~CSocketActorPassive ();
    virtual int OnInitOver()=0;

    virtual int OnWaitRecv();

    virtual int OnRecvOver()=0;

    virtual int OnFiniOver();
    virtual int OnSendOver();

    virtual int OnCloseOver();

protected:
    void NotifyAppActor();
};
class CSocketActorPassiveTcp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveTcp ();
    virtual int OnInitOver();
    virtual int OnRecvOver();
};
class CSocketActorPassiveUdp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveUdp ();
    virtual int OnInitOver();
    virtual int OnRecvOver();
};
#endif
