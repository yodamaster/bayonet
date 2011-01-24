/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        actorset.h
#  Description:    
#  Version:         1.0
#  LastChange:      2011-01-24 20:05:01
#  History:         
=============================================================================*/
#ifndef _CHILDACTOR_H_
#define _CHILDACTOR_H_

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "fsm_achieve.h"

class CChildActorInfo
{
public:
    CChildActorInfo():m_pActor(NULL),m_pUpperActor(NULL) {}
    virtual ~CChildActorInfo() {}

    virtual void AttachUpperActor(IActor* pActor)
    {
        m_pUpperActor = pActor;
    }
    virtual IActor* GetUpperActor()
    {
        return m_pUpperActor;
    }

    int Process();

protected:
    IActor * m_pActor;
    IActor * m_pUpperActor;
};

class CChildActorSet
{
public:
    CChildActorSet() : m_pActor(NULL) {}
    virtual ~CChildActorSet() {}
    virtual void AttachActor(IActor* pActor)
    {
        m_pActor = pActor;
    }
    virtual IActor* GetActor()
    {
        return m_pActor;
    }
    /**
     * @brief   添加一个pChildActorInfo
     *
     * @param   pChildActorInfo
     *
     * @return  0
     */
    virtual int Add(CChildActorInfo* pChildActorInfo)
    {
        pChildActorInfo->AttachUpperActor(m_pActor);
        m_setChildActors.insert(pChildActorInfo);
        return 0;
    }

    /**
     * @brief   删除一个pChildActorInfo
     *
     * @param   pChildActorInfo
     *
     * @return  0
     */
    virtual int Del(CChildActorInfo* pChildActorInfo)
    {
        m_setChildActors.erase(pChildActorInfo);
        return 0;
    }
private:
    set<CChildActorInfo*> m_setChildActors;
    IActor * m_pActor;
};
#endif
