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
#include "socketactor_passive_udp.h"
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
        //要保证这个recv事件是有效的，貌似启动的时候也会出发recv事件
        if (m_pNetHandler->GetClientIp().size()>0 && m_pNetHandler->GetClientPort()>0)
        {
            CSocketActorPassiveUdp * pSocketActorAccept = new CSocketActorPassiveUdp();
            trace_log("%s,%d",m_pNetHandler->GetClientIp().c_str(),m_pNetHandler->GetClientPort());
            pSocketActorAccept->Init(m_pNetHandler->GetClientIp(),m_pNetHandler->GetClientPort(),m_TimeoutMs,m_ProtoType);
            pSocketActorAccept->SetIActionPtr(m_pAction);
            pSocketActorAccept->AttachFrame(m_pFrame);
            pSocketActorAccept->ChangeState(SOCKET_FSM_INIT);
        }
        return SOCKET_FSM_WAITRECV;
    }
};
#endif
