/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_base.h
#  Description:     socketactor的基础类
#  Version:         1.0
#  LastChange:      2011-01-24 20:23:53
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_LISTEN_H_
#define _SOCKETACTOR_LISTEN_H_
#include "comm_def.h"
#include "fsm_achieve.h"
#include "socketactor_base.h"

class CSocketActorListen:public CSocketActorBase
{
public:
    virtual ~CSocketActorListen() {}

    virtual int OnInit();

    virtual int OnRecv();

    virtual int OnSend();

protected:
    //初始化
    virtual int HandleInit()
    {
        return SOCKET_FSM_WAITRECV;
    }
    // 发送包接收完毕
    virtual int HandleSendOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    // 回应包接受完毕
    virtual int HandleRecvOver(const char *buf, int len)
    {
        return SOCKET_FSM_WAITRECV;
    }
    //=============================================================================
};

#endif
