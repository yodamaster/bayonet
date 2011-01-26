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
    //必须要传一个socketlisten指针进来
    if (m_StFrameParam.pSocketActorListen == NULL)
    {
        return -1;
    }
    m_epoller.Create(m_StFrameParam.epollSize);

    CSocketActorListen* pSocketActorListen = m_StFrameParam.pSocketActorListen;
    pSocketActorListen->Init(m_StFrameParam.ip,m_StFrameParam.port,-1,m_StFrameParam.protoType);
    pSocketActorListen->AttachFrame(this);

    pSocketActorListen->ChangeState(SOCKET_FSM_INIT);

    m_epoller.LoopForEvent(m_StFrameParam.epollTimeoutMs);

    return 0;
}
