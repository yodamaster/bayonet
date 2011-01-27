/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        fsm_interface.h
#  Description:     公共接口
#  Version:         1.0
#  LastChange:      2011-01-24 19:36:15
#  History:         
=============================================================================*/
#ifndef _INTERFACES_H_
#define _INTERFACES_H_
#include <iostream>
#include <map>
using namespace std;

class IFsm;
class IActor;

class IFrame
{
public:
    virtual ~IFrame () {}

    /**
     * @brief   添加一个actor,将Actor加入到Frame的垃圾管理中去
     *
     * @param   pActor
     *
     * @return  0
     */
    virtual int AddActor(IActor* pActor)=0;

    /**
     * @brief   删除一个actor，一般不需要调用，Frame会统一用GC来调用析构
     *
     * @param   pActor
     *
     * @return  0
     */
    virtual int DelActor(IActor* pActor)=0;

    /**
     * @brief   获取所有actor的个数,不用list的size是因为太慢了
     *
     * @return  actor count
     */
    virtual int GetActorCount()=0;

    /**
     * @brief   添加待GC的个数,actor在SetGCMark时调用
     *
     * @return  0
     */
    virtual int AddNeedGCCount()=0;

    /**
     * @brief   减少待GC的个数,actor在析构的时候调用
     *
     * @return  0
     */
    virtual int SubNeedGCCount()=0;

    /**
     * @brief   获取待GC的个数
     *
     * @return  gc count
     */
    virtual int GetNeedGCCount()=0;

    /**
     * @brief   循环删除所有标记为GC的actor
     */
    virtual void GCActors()=0;

    /**
     * @brief   获取状态机
     *
     * @return  
     */
    virtual map<int, IFsm*>* GetFsmMgr()=0;

    /**
     * @brief   设置Fsm映射
     *
     * @param   state
     * @param   fsm
     *
     * @return  0
     */
    virtual int RegFsm(int state, IFsm* fsm)=0;
};

class IActor
{
public:
    virtual ~IActor() {}

    /**
     * @brief   设置需要回收
     *
     */
    virtual void SetGCMark()=0;

    /**
     * @brief   获取需要回收
     *
     * @return  
     */
    virtual bool GetGCMark()=0;

    /**
     * @brief   设置Frame
     *
     * @param   pFrame
     *
     * @return  0
     */
    virtual int AttachFrame(IFrame* pFrame)=0;

    /**
     * @brief   返回关联的框架
     *
     * @return  frame
     */
    virtual IFrame* GetFrame()=0;

    /**
     * @brief   关联状态机的管理器
     *
     * @param   ptrMapFsmMgr
     *
     * @return  0
     */
    virtual int AttachFsmMgr(map<int, IFsm*> *ptrMapFsmMgr)=0;

    /**
     * @brief   获取关联状态机的管理器
     *
     * @return  map<int, IFsm*>*
     */
    virtual map<int, IFsm*>* GetFsmMgr()=0;

    /**
     * @brief   关联上层的actor
     *
     * @param   pActor
     *
     * @return  0
     */
    virtual int AttachUpperActor(IActor* pActor)=0;

    /**
     * @brief   获取上层actor
     *
     * @return  actor
     */
    virtual IActor* GetUpperActor()=0;

    /**
     * @brief   执行当前状态
     *
     * @return  0
     */
    virtual int ProcessState()=0;

    /**
     * @brief   改变状态
     *
     * @param   destState
     *
     * @return  下一个状态
     */
    virtual int ChangeState(int destState)=0;
};


class IFsm
{
public:
    virtual ~IFsm() {}
    /**
     * @brief   设置Frame
     *
     * @param   pFrame
     *
     * @return  0
     */
    virtual int AttachFrame(IFrame* pFrame)=0;
    /**
     * @brief   在进入这个状态的时候，pActor需要做的事情
     *
     * @param   pActor
     *
     * @return  0           succ
     *          else        fail
     */
    virtual int Init(IActor* pActor)=0;

    /**
     * @brief   执行这个状态该做的事情
     *
     * @param   pActor
     *
     * @return  应该进入的另一个状态
     *          0           结束本次Process执行，不进入其他状态
     *          <0          结束整个请求（pActor需要被后续删除）
     *          else        其他状态（可以返回自己，但是会造成循环，有点危险）
     *          
     */
    virtual int Process(IActor* pActor)=0;

    /**
     * @brief   退出这个状态时需要做的事情
     *
     * @param   pActor
     *
     * @return  0           succ
     *          else        fail
     */
    virtual int Fini(IActor* pActor)=0;
};

#endif