/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info_set.h
#  Description:    
#  Version:         1.0
#  LastChange:      2011-01-28 16:02:28
#  History:         
=============================================================================*/
#ifndef _ACTION_INFO_SET_H_
#define _ACTION_INFO_SET_H_
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "fsm_achieve.h"
#include "action_info.h"

class CActionInfoSet
{
public:
    CActionInfoSet() {
    }
    virtual ~CActionInfoSet() {}
    virtual void SetAppActor(CActorBase* pActor)
    {
        if (pActor)
        {
            m_pAppActorProxy = pActor->get_ptr_proxy();
        }
    }
    virtual CActorBase* GetActor()
    {
        return m_pAppActorProxy.true_ptr();
    }
    /**
     * @brief   添加一个pActionInfo
     *
     * @param   pActionInfo
     *
     * @return  0
     */
    virtual int Add(CActionInfo* pActionInfo)
    {
        pActionInfo->SetAppActor(m_pAppActorProxy.true_ptr());
        m_setActionInfos.insert(pActionInfo);
        return 0;
    }

    /**
     * @brief   删除一个pActionInfo
     *
     * @param   pActionInfo
     *
     * @return  0
     */
    virtual int Del(CActionInfo* pActionInfo)
    {
        m_setActionInfos.erase(pActionInfo);
        return 0;
    }
    bool IsDealOver()
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
    int Start()
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
    int Clear()
    {
        for(set<CActionInfo*>::iterator it = m_setActionInfos.begin(); it != m_setActionInfos.end(); ++it)
        {
            if (*it)
            {
                delete (*it);
            }
        }
        m_setActionInfos.clear();
        return 0;
    }
    set<CActionInfo*>& GetActionSet()
    {
        return m_setActionInfos;
    }
private:
    set<CActionInfo*> m_setActionInfos;
    ptr_proxy<CActorBase> m_pAppActorProxy;
};
#endif
