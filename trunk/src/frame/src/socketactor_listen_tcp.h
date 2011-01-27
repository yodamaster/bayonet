/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_tcp.h
#  Description:     socketactor的基础类
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
    CSocketActorListenTcp():m_BackLog(10240),m_bKeepcnt(true){}
    virtual ~CSocketActorListenTcp() {}

    void SetBackLog(int backlog);

    int GetBackLog();

    void SetKeepcnt(bool bKeepcnt);

    virtual int OnInit();

    virtual int OnFini();

    virtual int OnRecv();

    virtual int OnSend();

    virtual int OnClose();

    virtual CSocketActorBase* AllocSocketActorAccept();
protected:
    int m_BackLog;
    bool m_bKeepcnt;
};
#endif
