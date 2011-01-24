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

class CSocketActorBase:public CActorBase
{
public:
    CSocketActorBase () {}
    virtual ~CSocketActorBase () {}

    virtual int SetEvent(int event);

    virtual int OnRecv();

    virtual int OnRecvOver();

    virtual int OnSend();

    virtual int OnSendOver();

    virtual int OnClose();

    virtual int OnCloseOver();

    virtual int OnTimeout();

    virtual int OnError();

    //=============================================================================
    //业务需要继承实现
protected:
    // 为发送打包
    virtual int HandleEncode(
            char *buf,
            int &len)=0;

    // 回应包完整性检查
    virtual int HandleInput(
            const char *buf,
            int len)=0;

    // 发送包接收完毕
    virtual int HandleSendOver()=0;
    // 回应包接受完毕
    virtual int HandleRecvOver()=0;
    // socket close完毕
    virtual int HandleCloseOver()=0;

    virtual int HandleTimeout(
            int timeout_ms)=0;

    virtual int HandleError(
            int err_no)=0;
    //=============================================================================
};

#endif
