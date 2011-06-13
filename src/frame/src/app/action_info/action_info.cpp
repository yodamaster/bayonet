/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 18:33:54
#  History:         
=============================================================================*/
#include "action_info.h"
#include "socketactor_active_sendrecv.h"
#include "socketactor_active_sendonly.h"
#include "socketactor_active.h"
int CActionInfo::Init(StActionInfoParam param)
{
    m_stActionTypeParam = param;
    return 0;
}
int CActionInfo::HandleStart()
{
    switch (m_stActionTypeParam.actionType)
    {
        case ACTIONTYPE_SENDRECV:
            m_pSocketActorActiveProxy = new CSocketActorActiveSendRecv();
            break;
        case ACTIONTYPE_SENDONLY:
            m_pSocketActorActiveProxy = new CSocketActorActiveSendOnly();
            break;
        default:
            error_log("actionType error:%d",m_stActionTypeParam.actionType);
            return -1;
    }
    if (m_pAppActorProxy.is_null())
    {
        error_log("m_pAppActor is null");
        return -1;
    }
    CSocketActorActive* pSocketActorActive = (CSocketActorActive*)(m_pSocketActorActiveProxy.true_ptr());
    pSocketActorActive->AttachFrame(m_pAppActorProxy.true_ptr()->GetFrame());
    pSocketActorActive->SetAppActor(m_pAppActorProxy.true_ptr());
    pSocketActorActive->SetIActionPtr(m_stActionTypeParam.pAction);
    pSocketActorActive->SetActionInfoPtr(this);
    pSocketActorActive->Init(m_stActionTypeParam.ip,m_stActionTypeParam.port,m_stActionTypeParam.timeout_ms,m_stActionTypeParam.protoType);
    pSocketActorActive->ChangeState(SOCKET_FSM_INIT);
    return 0;
}
