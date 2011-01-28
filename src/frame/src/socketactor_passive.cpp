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
    }

    return SOCKET_FSM_ALLOVER;
}
