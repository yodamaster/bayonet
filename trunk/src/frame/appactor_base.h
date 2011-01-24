/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appactor_base.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-24 20:06:18
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
#include "actorset.h"

using namespace std;

class CAppActorBase : public CActorBase
{
public:
    CAppActorBase()
    {
        m_ChildActorSet.AttachActor(this);
    }
    virtual ~CAppActorBase () {}

    CChildActorSet* GetChildActorSet()
    {
        return &m_ChildActorSet;
    }

private:
    CChildActorSet m_ChildActorSet;
};
#endif
