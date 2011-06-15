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

class CActionInfo;

class CActionInfoSet
{
public:
    CActionInfoSet();
    virtual ~CActionInfoSet();

    virtual void SetAppActor(CActorBase* pActor);

    virtual CActorBase* GetActor();

    /**
     * @brief   添加一个pActionInfo
     *
     * @param   pActionInfo
     *
     * @return  0
     */
    virtual int Add(CActionInfo* pActionInfo);

    /**
     * @brief   删除一个pActionInfo
     *
     * @param   pActionInfo
     *
     * @return  0
     */
    virtual int Del(CActionInfo* pActionInfo);

    bool IsDealOver();

    int Start();

    int Clear();

    set<CActionInfo*>& GetActionSet();
private:
    set<CActionInfo*> m_setActionInfos;
    ptr_proxy<CActorBase> m_pAppActorProxy;
};
#endif
