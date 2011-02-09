/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appactor_base.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-02-02 19:27:08
#  History:         
=============================================================================*/
#include "appactor_base.h"
#include "socketactor_base.h"
int CAppActorBase::AttachCommu(IActor* pActor)
{
    m_pCommuSocketActor = pActor;
    if (m_pCommuSocketActor)
    {
        CSocketActorBase* pCommuSocketActor = (CSocketActorBase*)m_pCommuSocketActor;
        pCommuSocketActor->SetAppActor(this);
    }
    return 0;
}
int CAppActorBase::DetachCommu()
{
    if (m_pCommuSocketActor)
    {
        CSocketActorBase* pCommuSocketActor = (CSocketActorBase*)m_pCommuSocketActor;
        pCommuSocketActor->SetAppActor(NULL);
    }
    m_pCommuSocketActor = NULL;
    return 0;
}
int CAppActorBase::Send2Client()
{
    trace_log("");
    if (m_pCommuSocketActor)
    {
        m_pCommuSocketActor->ChangeState(SOCKET_FSM_WAITSEND);
    }
    else
    {
        error_log("ChangeState SOCKET_FSM_WAITSEND");
    }
    return 0;
}
