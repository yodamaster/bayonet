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
#include "socketactor_passive.h"
class CSocketActorPassiveTcp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveTcp () {}
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnRecvOver()
    {
        trace_log("");
        //TODO
        //这个时候,应该是要等数据处理的阶段，现在是测试，直接回报
        return SOCKET_FSM_WAITCLOSE;
        //return 0;
        //return SOCKET_FSM_WAITSEND;
    }
};
#endif
