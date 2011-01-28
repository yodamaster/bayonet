/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_passive.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-28 17:08:34
#  History:         
=============================================================================*/
#include "socketactor_passive.h"
#include "appactor_base.h"

int CSocketActorPassive::OnFiniOver()
{
    if (m_pAppActor)
    {
        CAppActorBase* pAppActor = (CAppActorBase*)m_pAppActor;
        pAppActor->ChangeState(APP_FSM_FINI);
        pAppActor->SetPassiveSocketActor(NULL);
        m_pAppActor = NULL;
    }

    return SOCKET_FSM_ALLOVER;
}
int CSocketActorPassive::OnSendOver()
{
    if (m_pAppActor)
    {
        CAppActorBase* pAppActor = (CAppActorBase*)m_pAppActor;
        pAppActor->ChangeState(APP_FSM_FINI);
        pAppActor->SetPassiveSocketActor(NULL);
        m_pAppActor = NULL;
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
