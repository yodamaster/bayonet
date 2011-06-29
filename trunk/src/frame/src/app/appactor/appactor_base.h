/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appactor_base.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-25 20:18:55
#  History:         
=============================================================================*/
#ifndef __APPACTOR_BASE_H_20110630001540__
#define __APPACTOR_BASE_H_20110630001540__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "fsm_achieve.h"
#include "action_info_set.h"

using namespace std;

class CActionInfoSet;

class CAppActorBase : public CActorBase
{
public:
    CAppActorBase();

    virtual ~CAppActorBase ();

    CActionInfoSet* GetActionInfoSet();

    void SetTimeOutMs(int timeout_ms);

    int CheckTimeOut();

    bool IsTimeOut();

    int OnFini();

    int AttachCommu(CActorBase* pActor);

    int DetachCommu();

    int Send2Client();

protected:
    virtual CEPoller* GetEpoller();

private:
    CActionInfoSet* m_pActionInfoSet;
    ptr_proxy<CActorBase> m_pCommuSocketActorProxy;

    int m_TimeoutMs;//超时时间
};
#endif
