/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive_udp.h
#  Description:     被动UDP版，而且是不recv,直接send
#  Version:         1.0
#  LastChange:      2011-01-28 10:53:19
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_UDP_H_
#define _SOCKETACTOR_PASSIVE_UDP_H_
#include "socketactor_passive.h"
class CSocketActorPassiveUdp : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveUdp () {}

    virtual int OnInitOver()
    {
        //按理说应该是返回close，等待上层唤醒
        return SOCKET_FSM_WAITSEND;
    }
    virtual int OnRecvOver()
    {
        //不会走到这一步
        return SOCKET_FSM_WAITSEND;
    }
};
#endif
