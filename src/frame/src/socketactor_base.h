/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_base.h
#  Description:     socketactor的基础类
#  Version:         1.0
#  LastChange:      2011-01-24 20:23:53
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_BASE_H_
#define _SOCKETACTOR_BASE_H_
#include "comm_def.h"
#include "fsm_achieve.h"
#include "epoller.h"
#include "fl_log.h"
#include "net_handler.h"
//#include "appactor_base.h"

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
    CSocketActorBase () {
        m_SocketFd = -1;
        m_Port = 0;
        m_TimeoutMs = -1;
        m_ProtoType = 0;
        m_pAction = NULL;
        m_bKeepcnt = false;
        //m_pAppActor = NULL;
    }
    virtual ~CSocketActorBase () {}

    virtual int HandleEvent(IEvent* pEvent);

    //virtual int SetAppActor(CAppActorBase* pActor);

    virtual int Init(string ip,int port,int timeout_ms,int protoType);

    virtual int Init(int socketFd,int timeout_ms,int protoType);
    
    virtual int SetIActionPtr(IAction *pAction);

    virtual int GetSocketFd();

    virtual void SetKeepcnt(bool bKeepcnt);

    virtual int CheckTimeOut(struct timeval& now_time);

    virtual int SetEvent(unsigned event);

    virtual int OnTimeout();

    virtual int OnError();


    virtual int OnInit()=0;

    virtual int OnFini()=0;

    virtual int OnRecv()=0;

    virtual int OnSend()=0;

    virtual int OnClose()=0;

protected:
    virtual CEPoller* GetEpoller();

protected:
    int m_SocketFd;

    string m_IP;
    int m_Port;
    int m_TimeoutMs;
    int m_ProtoType;

    IAction* m_pAction;
    bool m_bKeepcnt;

    //CAppActorBase* m_pAppActor;
};
#endif
