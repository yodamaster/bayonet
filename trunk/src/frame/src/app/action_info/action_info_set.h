/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info_set.h
#  Description:    
#  Version:         1.0
#  LastChange:      2011-01-28 16:02:28
#  History:         
=============================================================================*/
#ifndef _ACTION_INFO_SET_H_20110630001532_
#define _ACTION_INFO_SET_H_20110630001532_

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "fsm_achieve.h"
#include "action_info.h"

namespace bayonet {
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

    bool IsDealOver();

    int Start();

    int Clear();

    list<CActionInfo*>& GetActionSet();

    /**
     * @brief   在析构的时候，清理掉所有存储的actioninfo
     */
    void FreeAllHistoryActionInfos();
private:
    list<CActionInfo*> m_setActionInfos;

    //为了避免actioninfo在changestate过程中被删除
    list<CActionInfo*> m_setHistorActionInfos;
    ptr_proxy<CActorBase> m_pAppActorProxy;
};
}
#endif
