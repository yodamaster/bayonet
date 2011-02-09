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

class CAppActorBase : public CActorBase
{
public:
    CAppActorBase()
    {
        m_ActionInfoSet.AttachActor(this);
        m_pCommuSocketActor = NULL;
    }
    virtual ~CAppActorBase () {}

    CActionInfoSet* GetActionInfoSet()
    {
        return &m_ActionInfoSet;
    }
    int OnFini()
    {
        SetGCMark();
        return APP_FSM_ALLOVER;
    }

    int AttachCommu(IActor* pActor);

    int DetachCommu();

    int Send2Client();

private:
    CActionInfoSet m_ActionInfoSet;
    IActor* m_pCommuSocketActor;
};
#endif
