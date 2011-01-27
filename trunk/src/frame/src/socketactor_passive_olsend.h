/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive_olsend.h
#  Description:     被动版，而且是不recv,直接send
#  Version:         1.0
#  LastChange:      2011-01-28 00:40:46
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_PASSIVE_OLSEND_H_
#define _SOCKETACTOR_PASSIVE_OLSEND_H_
#include "socketactor_passive.h"
class CSocketActorPassiveOLSend : public CSocketActorPassive
{
public:
    virtual ~CSocketActorPassiveOLSend () {}

    virtual int OnInitOver()
    {
        //按理说应该是返回close，等待上层唤醒
        return SOCKET_FSM_WAITSEND;
    }
};
#endif
