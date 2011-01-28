/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_active.h
#  Description:     主动版
#  Version:         1.0
#  LastChange:      2011-01-28 11:34:59
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_ACTIVE_H_
#define _SOCKETACTOR_ACTIVE_H_
#include "socketactor_data.h"
class CSocketActorActive : public CSocketActorData
{
public:
    virtual ~CSocketActorActive () {}

protected:
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITSEND;
    }
    virtual int OnSendOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnRecvOver()
    {
        //这个时候,应该是要等数据处理的阶段
        return SOCKET_FSM_CLOSING;
    }
};
#endif
