/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_set.h
#  Description:    
#  Version:         1.0
#  LastChange:      2011-01-25 20:16:46
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_SET_H_
#define _SOCKETACTOR_SET_H_

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "fsm_achieve.h"
#include "socketactor_base.h"

class CSocketActorSet
{
public:
    CSocketActorSet() : m_pActor(NULL) {}
    virtual ~CSocketActorSet() {}
    virtual void AttachActor(IActor* pActor)
    {
        m_pActor = pActor;
    }
    virtual IActor* GetActor()
    {
        return m_pActor;
    }
    /**
     * @brief   添加一个pSocketActor
     *
     * @param   pSocketActor
     *
     * @return  0
     */
    virtual int Add(CSocketActorBase* pSocketActor)
    {
        pSocketActor->AttachUpperActor(m_pActor);
        m_setSocketActors.insert(pSocketActor);
        return 0;
    }

    /**
     * @brief   删除一个pSocketActor
     *
     * @param   pSocketActor
     *
     * @return  0
     */
    virtual int Del(CSocketActorBase* pSocketActor)
    {
        m_setSocketActors.erase(pSocketActor);
        return 0;
    }
private:
    set<CSocketActorBase*> m_setSocketActors;
    IActor * m_pActor;
};
#endif
