/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive_tcp.h
#  Description:     被动TCP
#  Version:         1.0
#  LastChange:      2011-01-28 10:52:33
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_TCP_H_
#define _SOCKETACTOR_PASSIVE_TCP_H_
#include "socketactor_data.h"
class CSocketActorPassiveTcp : public CSocketActorData
{
public:
    virtual ~CSocketActorPassiveTcp () {}
protected:
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnSendOver()
    {
        Clear();
        if (m_bKeepcnt && m_ProtoType == PROTO_TYPE_TCP)
        {
            return SOCKET_FSM_WAITRECV;
        }
        else
        {
            return SOCKET_FSM_CLOSING;
        }
    }
    virtual int OnRecvOver()
    {
        //TODO
        //这个时候,应该是要等数据处理的阶段，现在是测试，直接回报
        //return SOCKET_FSM_WAITCLOSE;
        return SOCKET_FSM_WAITSEND;
    }
};
#endif
