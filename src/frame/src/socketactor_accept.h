/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_accept.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-24 23:51:59
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACCEPT_H_
#define _SOCKETACTOR_ACCEPT_H_
#include "socketactor_base.h"
class CSocketActorAccept
{
public:
    virtual ~CSocketActorAccept () {}

protected:
    virtual int HandleEncode(
            char *buf,
            int &len)=0;
    virtual int HandleInput(
            const char *buf,
            int len)=0;
    virtual int HandleRecvOver(const char *buf, int len,IActor *pActor)=0;

    virtual int HandleSendOver()
    {
        //TODO
        //清除数据
        return SOCKET_FSM_WAITCLOSE;
    }
};
#endif
