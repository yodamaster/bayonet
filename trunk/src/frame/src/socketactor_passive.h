/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive.h
#  Description:     被动版
#  Version:         1.0
#  LastChange:      2011-01-25 12:45:09
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_H_
#define _SOCKETACTOR_PASSIVE_H_
#include "socketactor_base.h"
class CSocketActorPassive
{
public:
    virtual ~CSocketActorPassive () {}

protected:
    virtual int HandleInit()
    {
        return SOCKET_FSM_WAITSEND;
    }
    virtual int HandleSendOver()
    {
        //TODO
        //清除数据
        return SOCKET_FSM_WAITRECV;
    }
    virtual int HandleEncode(
            char *buf,
            int &len)=0;
    virtual int HandleInput(
            const char *buf,
            int len)=0;
    virtual int HandleRecvOver(const char *buf, int len)=0;
};
#endif
