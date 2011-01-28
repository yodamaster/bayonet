/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive.h
#  Description:     被动TCP
#  Version:         1.0
#  LastChange:      2011-01-28 17:00:16
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
    virtual int OnInitOver()=0;
    virtual int OnRecvOver()=0;

    virtual int OnFiniOver();
    virtual int OnSendOver()
    {
        if (m_pAppActor)
        {
            m_pAppActor->ChangeState(APP_FSM_FINI);
        }
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
};
#endif
