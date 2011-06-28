/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_tcp.h
#  Description:     tcp listen
#  Version:         1.0
#  LastChange:      2011-01-27 23:51:50
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_LISTEN_TCP_H_
#define _SOCKETACTOR_LISTEN_TCP_H_
#include "comm_def.h"
#include "fsm_achieve.h"
#include "socketactor_data.h"

class CSocketActorListenTcp:public CSocketActorBase
{
public:
    CSocketActorListenTcp();
    virtual ~CSocketActorListenTcp();

    virtual int Init(string ip,int port,int timeout_ms,int protoType);

    void SetBackLog(int backlog);

    int GetBackLog();

    void SetAttachedSocketMaxSize(int attachedSocketMaxSize);

    int GetAttachedSocketMaxSize();

    virtual int OnInit();

    virtual int OnInitOver();

    virtual int OnWaitRecv();

    virtual int OnRecv();

    virtual int OnRecvOver();

    virtual int OnWaitSend();

    virtual int OnSend();

    virtual int OnSendOver();

    virtual int OnClose();

    virtual int OnCloseOver();

    virtual bool IsTimeOut();

protected:
    int m_BackLog;

    int m_attachedSocketMaxSize;
};
#endif
