/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appfsm_base.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-24 20:06:27
#  History:         
=============================================================================*/
#ifndef _APPFSM_BASE_H_20110630001608_
#define _APPFSM_BASE_H_20110630001608_

#include "appactor_base.h"
#include "action_info_set.h"

class CAppFsmBase : public CFsmBase
{
public:
    CAppFsmBase () {}
    virtual ~CAppFsmBase () {}
    virtual int HandleEntry(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        int ret = HandleEntry(pAppActor->GetActionInfoSet(), pAppActor);
        if (ret)
        {
            return -1;
        }
        ret = pAppActor->GetActionInfoSet()->Start();
        if (ret)
        {
            return -2;
        }
        return 0;
    }
    virtual int HandleProcess(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        if (!pAppActor->GetActionInfoSet()->IsDealOver())
        {
            //还没有做完，所以还是先返回0就行
            return 0;
        }
        return HandleProcess(pAppActor->GetActionInfoSet(), pAppActor);
    }
    virtual int HandleExit(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        int ret = HandleExit(pAppActor->GetActionInfoSet(), pAppActor);
        pAppActor->GetActionInfoSet()->Clear();
        return ret;
    }
    /**
     * @brief   Entry函数会调用，多传入几个参数
     *
     * @param   pActionInfoSet      动作集合
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleEntry(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Process函数会调用，多传入几个参数
     *
     * @param   pActionInfoSet
     * @param   pAppActor            数据
     *
     * @return  0           结束本次Process执行，不进入其他状态
     *          <0          结束整个请求（obj需要被后续删除）
     *          else        其他状态（可以返回自己，但是会造成循环，有点危险）
     */
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Exit函数会调用
     *
     * @param   pActionInfoSet
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleExit(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
class CAppFsmRsp : public CAppFsmBase
{
    int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        pAppActor->Send2Client();
        return 0;
    }
};
class CAppFsmFini : public CAppFsmBase
{
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return pAppActor->OnFini();
    }
};

#endif
