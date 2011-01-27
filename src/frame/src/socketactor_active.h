/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-25 11:53:08
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_H_
#define _SOCKETACTOR_ACTIVE_H_
#include "socketactor_base.h"
class CSocketActorActive : public CSocketActorData
{
public:
    virtual ~CSocketActorActive () {}

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
            string & strSendBuf,
            int &len)=0;
    virtual int HandleInput(
            const char *buf,
            int len)=0;
    virtual int HandleRecvOver(const char *buf, int len)=0;
};
#endif
