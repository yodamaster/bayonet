/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_base.h
#  Description:     socketactor的基础类
#  Version:         1.0
#  LastChange:      2011-01-24 20:23:53
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_BASE_H_20110630001720_
#define _SOCKETACTOR_BASE_H_20110630001720_

#include "comm_def.h"
#include "fsm_achieve.h"
#include "epoller.h"
#include "fl_log.h"
#include "net_handler.h"
//#include "appactor_base.h"
namespace bayonet {
class IAction;

class CEpollEvent : public IEvent
{
public:
    CEpollEvent () {
        evt = 0;
    }
    virtual ~CEpollEvent () {}

    unsigned evt;
};

class CEPoller;
class CSocketActorBase:public CActorBase
{
public:
    CSocketActorBase ();
    virtual ~CSocketActorBase ();

    virtual int HandleEvent(IEvent* pEvent);

    virtual void SetAppActor(CActorBase* pActor);

    virtual int Init(string ip,int port,int timeout_ms,int protoType);

    virtual int Init(int socketFd,int timeout_ms,int protoType);
    
    virtual void SetIActionPtr(IAction *pAction);

    virtual int GetSocketFd();

    virtual void SetKeepcnt(bool bKeepcnt);

    virtual void SetTimeOutMs(int timeout_ms);

    virtual int CheckTimeOut();

    virtual bool IsTimeOut();

    virtual int SetEvent(unsigned event);

    virtual int OnTimeout();

    virtual int OnError();

    virtual int OnInit()=0;

    virtual int OnInitOver()=0;

    virtual int OnFini();

    virtual int OnWaitRecv();

    virtual int OnRecv()=0;

    virtual int OnRecvOver()=0;

    virtual int OnWaitSend();

    virtual int OnSend()=0;

    virtual int OnSendOver()=0;

    virtual int OnWaitClose();

    virtual int OnClose()=0;

    virtual int OnCloseOver()=0;

protected:
    virtual CEPoller* GetEpoller();

    int DetachFromEpoller();

protected:
    int m_SocketFd;

    string m_IP;
    int m_Port;
    int m_TimeoutMs;
    int m_ProtoType;

    IAction* m_pAction;
    bool m_bKeepcnt;

    ptr_proxy<CActorBase> m_pAppActorProxy;
};
}
#endif
