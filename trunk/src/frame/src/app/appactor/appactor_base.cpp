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
CAppActorBase::CAppActorBase()
{
    m_pActionInfoSet = new CActionInfoSet();
    m_pActionInfoSet->SetAppActor(this);
}

CAppActorBase::~CAppActorBase () {
    DetachCommu();
    if (m_pActionInfoSet)
    {
        delete m_pActionInfoSet;
        m_pActionInfoSet = NULL;
    }
}
CActionInfoSet* CAppActorBase::GetActionInfoSet()
{
    return m_pActionInfoSet;
}
int CAppActorBase::OnFini()
{
    SetGCMark();
    DetachCommu();
    return APP_FSM_ALLOVER;
}

int CAppActorBase::AttachCommu(CActorBase* pActor)
{
    m_pCommuSocketActorProxy = pActor->get_ptr_proxy();
    if (m_pCommuSocketActorProxy.true_ptr())
    {
        CSocketActorBase* pCommuSocketActor = (CSocketActorBase*)(m_pCommuSocketActorProxy.true_ptr());
        pCommuSocketActor->SetAppActor(this);
    }
    return 0;
}
int CAppActorBase::DetachCommu()
{
    if (m_pCommuSocketActorProxy.true_ptr())
    {
        CSocketActorBase* pCommuSocketActor = (CSocketActorBase*)(m_pCommuSocketActorProxy.true_ptr());
        pCommuSocketActor->SetAppActor(NULL);
    }
    m_pCommuSocketActorProxy = NULL;
    return 0;
}
int CAppActorBase::Send2Client()
{
    if (m_pCommuSocketActorProxy.true_ptr())
    {
        m_pCommuSocketActorProxy.true_ptr()->ChangeState(SOCKET_FSM_WAITSEND);
    }
    else
    {
        error_log("[class:%s] ChangeState SOCKET_FSM_WAITSEND", Name().c_str());
    }
    return 0;
}
