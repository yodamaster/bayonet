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
#include "socketactor_data.h"
class CSocketActorPassive : public CSocketActorData
{
public:
    virtual ~CSocketActorPassive () {}
protected:
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnSendOver()
    {
        Clear();
        //TODO
        //这里应该有参数要标识是长连接还是短连接
        return SOCKET_FSM_CLOSING;
    }
    virtual int OnRecvOver()
    {
        //这个时候,应该是要等数据处理的阶段
        //return SOCKET_FSM_WAITCLOSE;
        return SOCKET_FSM_WAITSEND;
    }
    virtual int OnCloseOver()
    {
        return SOCKET_FSM_FINI;
    }
};
#endif
