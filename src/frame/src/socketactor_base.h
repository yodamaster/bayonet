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
#include "fsm_achieve.h"

class CSocketActorBase
{
public:
    CSocketActorBase () {}
    virtual ~CSocketActorBase () {}

    virtual int OnRecv();

    virtual int OnSend();

    virtual int OnTimeout();

    virtual int OnError();

    //=============================================================================
    //业务需要继承实现

    // 为发送打包
    virtual int HandleEncode(
            char *buf,
            int &len)
     {
         len = 0;
         return 0;
     }

    // 回应包完整性检查
    virtual int HandleInput(
            const char *buf,
            int len)
    {
        return 0;
    }

    // 回应包处理
    virtual int HandleProcess(
            const char *buf,
            int len)
    {
        return 0;
    }

    virtual int HandleError(
            int timeout_ms)
    {   
        return 0;
    }   

    virtual int HandleTimeout(
            int err_no)
    {   
        return 0;
    } 
    //=============================================================================
};

#endif
