/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_udp.h
#  Description:     被动版
#  Version:         1.0
#  LastChange:      2011-01-28 00:46:16
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_LISTEN_UDP_H_
#define _SOCKETACTOR_LISTEN_UDP_H_
#include "socketactor_data.h"
#include "socketactor_passive_olsend.h"
class CSocketActorListenUdp : public CSocketActorData
{
public:
    virtual ~CSocketActorListenUdp () {}
    virtual int OnInit();
protected:
    virtual int OnInitOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnSendOver()
    {
        return SOCKET_FSM_WAITRECV;
    }
    virtual int OnRecvOver()
    {
        CSocketActorPassiveOLSend * pSocketActorAccept = new CSocketActorPassiveOLSend();
        pSocketActorAccept->Init(m_pNetHandler->GetClientIp(),m_pNetHandler->GetClientPort(),m_TimeoutMs,m_ProtoType);
        pSocketActorAccept->SetIActionPtr(m_pAction);
        pSocketActorAccept->AttachFrame(m_pFrame);
        pSocketActorAccept->ChangeState(SOCKET_FSM_INIT);
        return SOCKET_FSM_WAITRECV;
    }
};
#endif
