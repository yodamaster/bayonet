/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appfsm_base.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-24 20:06:27
#  History:         
=============================================================================*/
#include "appactor_base.h"
#include "action_info_set.h"

class CAppFsmBase : public CFsmBase
{
public:
    CAppFsmBase () {}
    virtual ~CAppFsmBase () {}
    virtual int Entry(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleEntry(pAppActor->GetActionInfoSet(), pAppActor);
    }
    virtual int Process(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleProcess(pAppActor->GetActionInfoSet(), pAppActor);
    }
    virtual int Exit(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleExit(pAppActor->GetActionInfoSet(), pAppActor);
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
};
class CAppFsmFini : public CAppFsmBase
{
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return pAppActor->OnFini();
    }
};
