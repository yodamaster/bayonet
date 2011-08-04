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
#include "bayonet_frame.h"
#include "fl_log.h"
#include "epoller.h"
namespace bayonet {
CAppActorBase::CAppActorBase()
{
    m_TimeoutMs = -1;
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

void CAppActorBase::SetTimeOutMs(int timeout_ms)
{
    m_TimeoutMs = timeout_ms;
}

int CAppActorBase::CheckTimeOut()
{
    if (GetGCMark())
    {
        return 0;
    }
    if (!IsTimeOut())
    {
        return 0;
    }
    //超时了
    //这样做是为了防止core，因为这个list可能会在切换过程中被删除
    bool bDealed = false;
    list<CActionInfo*> &setAction = m_pActionInfoSet->GetActionSet();
    for(list<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
    {
        if (!((*it)->IsDealOver()))
        {
            bDealed = true;
            (*it)->SetDealOver(EAppActorTimeout,m_aliveTimer.GetPastTime(),false);
        }
    }

    if (bDealed)
    {
        ProcessState();
    }
    return 0;
}

bool CAppActorBase::IsTimeOut()
{
    if (m_TimeoutMs < 0)
    {
        //永不超时
        return false;
    }
    int pastTime = m_aliveTimer.GetPastTime();

    if (pastTime > m_TimeoutMs)
    {
        return true;
    }
    else
    {
        return false;
    }
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
    CEPoller* pEpoller = GetEpoller();
    if (pEpoller)
    {
        pEpoller->AttachAppActor(this);
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

    CEPoller* pEpoller = GetEpoller();
    if (pEpoller)
    {
        pEpoller->DetachAppActor(this);
    }
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
        byt_error_log("[class:%s] ChangeState SOCKET_FSM_WAITSEND", Name().c_str());
    }
    return 0;
}
CEPoller* CAppActorBase::GetEpoller()
{
    IFrame* pFrame = GetFrame();
    if (!pFrame)
    {
        return NULL;
    }
    CBayonetFrame* pBayonetFrame = (CBayonetFrame*)pFrame;
    CEPoller * pEpoller = pBayonetFrame->GetEpoller();
    return pEpoller;
}
}
