/*=============================================================================
#
#     FileName: action_info_set.cpp
#         Desc: 
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-15 02:22:33
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-15 02:22:33 | initialization
#
=============================================================================*/
#include "action_info_set.h"

CActionInfoSet::CActionInfoSet() {
}
CActionInfoSet::~CActionInfoSet() {
    FreeAllHistoryActionInfos();
}
void CActionInfoSet::SetAppActor(CActorBase* pActor)
{
    if (pActor)
    {
        m_pAppActorProxy = pActor->get_ptr_proxy();
    }
}
CActorBase* CActionInfoSet::GetActor()
{
    return m_pAppActorProxy.true_ptr();
}
int CActionInfoSet::Add(CActionInfo* pActionInfo)
{
    pActionInfo->SetAppActor(m_pAppActorProxy.true_ptr());
    m_setActionInfos.insert(pActionInfo);

    m_setHistorActionInfos.insert(pActionInfo);
    return 0;
}

bool CActionInfoSet::IsDealOver()
{
    for(set<CActionInfo*>::iterator it = m_setActionInfos.begin(); it != m_setActionInfos.end(); ++it)
    {
        if (!(*it)->IsDealOver())
        {
            return false;
        }
    }
    return true;
}
int CActionInfoSet::Start()
{
    int ret = 0;
    for(set<CActionInfo*>::iterator it = m_setActionInfos.begin(); it != m_setActionInfos.end(); ++it)
    {
        ret = (*it)->HandleStart();
        if (ret)
        {
            error_log("CActionInfo HandleStart error:%d", ret);
            return -1;
        }
    }
    return 0;
}
int CActionInfoSet::Clear()
{
    m_setActionInfos.clear();
    return 0;
}
set<CActionInfo*>& CActionInfoSet::GetActionSet()
{
    return m_setActionInfos;
}

void CActionInfoSet::FreeAllHistoryActionInfos()
{
    for(set<CActionInfo*>::iterator it = m_setHistorActionInfos.begin(); it != m_setHistorActionInfos.end(); ++it)
    {
        if (*it)
        {
            delete (*it);
        }
    }
}
