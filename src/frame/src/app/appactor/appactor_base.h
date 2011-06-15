/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appactor_base.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-25 20:18:55
#  History:         
=============================================================================*/
#ifndef _APPACTOR_BASE_H_
#define _APPACTOR_BASE_H_
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

    int OnFini();

    int AttachCommu(CActorBase* pActor);

    int DetachCommu();

    int Send2Client();

private:
    CActionInfoSet* m_pActionInfoSet;
    ptr_proxy<CActorBase> m_pCommuSocketActorProxy;
};
#endif
