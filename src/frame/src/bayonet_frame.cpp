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
int CBayonetFrame::Process()
{
    CSocketActorListenTcp* pSocketActorListen = new CSocketActorListenTcp();
    pSocketActorListen->Init(m_StFrameParam.ip,m_StFrameParam.port,-1,m_StFrameParam.protoType);
    pSocketActorListen->SetBackLog(m_StFrameParam.backlog);
    pSocketActorListen->SetKeepcnt(m_StFrameParam.bKeepcnt);
    pSocketActorListen->SetIActionPtr(m_StFrameParam.pAction);
    pSocketActorListen->AttachFrame(this);

    pSocketActorListen->ChangeState(SOCKET_FSM_INIT);

    m_epoller.LoopForEvent();

    return 0;
}
