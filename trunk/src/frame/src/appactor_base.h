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
#include "socketactor_set.h"

using namespace std;

class CAppActorBase : public CActorBase
{
public:
    CAppActorBase()
    {
        m_SocketActorSet.AttachActor(this);
    }
    virtual ~CAppActorBase () {}

    CSocketActorSet* GetSocketActorSet()
    {
        return &m_SocketActorSet;
    }

private:
    CSocketActorSet m_SocketActorSet;
};
#endif
