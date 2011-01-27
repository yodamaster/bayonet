/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-26 17:27:51
#  History:         
=============================================================================*/
#include "bayonet_frame.h"
#include "socketactor_listen_tcp.h"
#include "socketactor_listen_udp.h"
int CBayonetFrame::Process()
{
    CSocketActorListenTcp* pSocketActorListenTcp = NULL;
    CSocketActorListenUdp* pSocketActorListenUdp = NULL;
    switch(m_StFrameParam.protoType)
    {
        case PROTO_TYPE_TCP:
            pSocketActorListenTcp = new CSocketActorListenTcp();
            pSocketActorListenTcp->Init(m_StFrameParam.ip,m_StFrameParam.port,-1,m_StFrameParam.protoType);
            pSocketActorListenTcp->SetBackLog(m_StFrameParam.backlog);
            pSocketActorListenTcp->SetKeepcnt(m_StFrameParam.bKeepcnt);
            pSocketActorListenTcp->SetIActionPtr(m_StFrameParam.pAction);
            pSocketActorListenTcp->AttachFrame(this);
            pSocketActorListenTcp->ChangeState(SOCKET_FSM_INIT);
            break;
        case PROTO_TYPE_UDP:
            pSocketActorListenUdp = new CSocketActorListenUdp();
            pSocketActorListenUdp->Init(m_StFrameParam.ip,m_StFrameParam.port,-1,m_StFrameParam.protoType);
            pSocketActorListenUdp->SetIActionPtr(m_StFrameParam.pAction);
            pSocketActorListenUdp->AttachFrame(this);
            pSocketActorListenUdp->ChangeState(SOCKET_FSM_INIT);
            break;
        default:
            return -1;
    }

    m_epoller.LoopForEvent();

    return 0;
}
