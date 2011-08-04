/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive.h
#  Description:     被动TCP
#  Version:         1.0
#  LastChange:      2011-01-28 17:00:16
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_H_20110630001756_
#define _SOCKETACTOR_PASSIVE_H_20110630001756_

#include "socketactor_data.h"
namespace bayonet {
class CSocketActorPassive : public CSocketActorData
{
public:
    virtual ~CSocketActorPassive ();
    virtual int OnInitOver()=0;

    virtual int OnWaitRecv();

    virtual int OnRecvOver()=0;

    virtual int OnSendOver();

    virtual int OnCloseOver();

protected:
    void NotifyAppActor();
};
class CSocketActorPassiveTcp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveTcp ();
    virtual bool IsTimeOut();

    virtual int OnInitOver();
    virtual int OnWaitRecv();
    virtual int OnRecvOver();

protected:
    CTimer m_idleTimer; //没有收到任何请求的时间，在每个waitrecv时启动
};
class CSocketActorPassiveUdp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveUdp ();
    virtual int OnInitOver();
    virtual int OnRecvOver();
};
}
#endif
