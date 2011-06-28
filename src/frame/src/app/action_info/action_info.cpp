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
#include "socketactor_active.h"
CActionInfo::CActionInfo () {
    m_err_no = 0;
    m_timecost_ms= 0;
    m_bDealOver = false;
}
int CActionInfo::Init(StActionInfoParam param)
{
    m_stActionTypeParam = param;
    return 0;
}
int CActionInfo::GetID()
{
    return m_stActionTypeParam.id;
}

void CActionInfo::SetDealOver(int err_no,int timecost_ms, bool bProc)
{
    if (m_bDealOver)
    {
        return;
    }
    m_bDealOver = true;
    m_err_no = err_no;
    m_timecost_ms = timecost_ms;

    //Del-Begin by dantezhu in 2011-06-14 01:36:26
    //这里不直接去执行ProcessState的原因是，因为这个函数可能会导致this指针失效
    /*if (m_pAppActorProxy.true_ptr())
      {
      m_pAppActorProxy.true_ptr()->ProcessState();
      }*/
    //Del-End
    //现在又可以这样写了，因为做了优化，actioninfo会保留下来，知道最后actor被析构才会释放
    if (bProc && m_pAppActorProxy.true_ptr())
    {
        m_pAppActorProxy.true_ptr()->ProcessState();
    }
}

bool CActionInfo::IsDealOver()
{
    return m_bDealOver;
}

int CActionInfo::GetErrno()
{
    return m_err_no;
}

int CActionInfo::GetTimeCost()
{
    return m_timecost_ms;
}

void CActionInfo::SetAppActor(CActorBase* pActor)
{
    if (pActor)
    {
        m_pAppActorProxy = pActor->get_ptr_proxy();
    }
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
