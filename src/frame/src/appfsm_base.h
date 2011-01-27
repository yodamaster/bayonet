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

class CAppFsmBase : public CFsmBase
{
public:
    CAppFsmBase () {}
    virtual ~CAppFsmBase () {}
    virtual int Init(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleInit(pAppActor->GetSocketActorSet(), pAppActor);
    }
    virtual int Process(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleProcess(pAppActor->GetSocketActorSet(), pAppActor);
    }
    virtual int Fini(IActor* obj)
    {
        CAppActorBase * pAppActor = (CAppActorBase*)obj;
        return HandleFini(pAppActor->GetSocketActorSet(), pAppActor);
    }
    /**
     * @brief   Init函数会调用，多传入几个参数
     *
     * @param   pSocketActorSet      动作集合
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleInit(CSocketActorSet *pSocketActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Process函数会调用，多传入几个参数
     *
     * @param   pSocketActorSet
     * @param   pAppActor            数据
     *
     * @return  0           结束本次Process执行，不进入其他状态
     *          <0          结束整个请求（obj需要被后续删除）
     *          else        其他状态（可以返回自己，但是会造成循环，有点危险）
     */
    virtual int HandleProcess(CSocketActorSet *pSocketActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }

    /**
     * @brief   Fini函数会调用
     *
     * @param   pSocketActorSet
     * @param   pAppActor            数据
     *
     * @return  0               succ
     *          else            fail
     */
    virtual int HandleFini(CSocketActorSet *pSocketActorSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
class CAppFsmFini : public CAppFsmBase
{
    /**
     * @brief   Process函数会调用，多传入几个参数
     *
     * @param   pSocketActorSet
     * @param   pAppActor            数据
     *
     * @return  0           结束本次Process执行，不进入其他状态
     *          <0          结束整个请求（obj需要被后续删除）
     *          else        其他状态（可以返回自己，但是会造成循环，有点危险）
     */
    virtual int HandleProcess(CSocketActorSet *pSocketActorSet, CAppActorBase* pAppActor)
    {
        //把passive的socket变成waitsend状态
        return 0;
    }
};
