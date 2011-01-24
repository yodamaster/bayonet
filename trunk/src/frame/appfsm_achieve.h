/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appfsm_achieve.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-20 17:17:28
#  History:         
=============================================================================*/
#ifndef _APPFSM_ACHIEVE_H_
#define _APPFSM_ACHIEVE_H_
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "fsm_interface.h"
#include "fsm_achieve.h"

#include "appfsm_interface.h"

using namespace std;

class CChildActorInfo : public IChildActorInfo
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

    int Process()
    {
        return 0;
    }

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
    virtual int Add(IChildActorInfo* pChildActorInfo)
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
    virtual int Del(IChildActorInfo* pChildActorInfo)
    {
        m_setChildActors.erase(pChildActorInfo);
        return 0;
    }
private:
    set<IChildActorInfo*> m_setChildActors;
    IActor * m_pActor;
};

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

class CAppFsm : public IFsm
{
public:
    CAppFsm () {}
    virtual ~CAppFsm () {}
    virtual int Init(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleInit(pAppActor->GetChildActorSet(), pAppActor);
    }
    virtual int Process(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleProcess(pAppActor->GetChildActorSet(), pAppActor);
    }
    virtual int Fini(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleFini(pAppActor->GetChildActorSet(), pAppActor);
    }
    /**
     * @brief   Init函数会调用，多传入几个参数
     *
     * @param   pChildActorSet      动作集合
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleInit(CChildActorSet *pChildActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Process函数会调用，多传入几个参数
     *
     * @param   pChildActorSet
     * @param   pAppActor            数据
     *
     * @return  0           结束本次Process执行，不进入其他状态
     *          <0          结束整个请求（obj需要被后续删除）
     *          else        其他状态（可以返回自己，但是会造成循环，有点危险）
     */
    virtual int HandleProcess(CChildActorSet *pChildActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Fini函数会调用
     *
     * @param   pChildActorSet
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleFini(CChildActorSet *pChildActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
#endif
