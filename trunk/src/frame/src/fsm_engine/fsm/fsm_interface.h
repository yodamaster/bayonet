/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        fsm_interface.h
#  Description:     公共接口
#  Version:         1.0
#  LastChange:      2011-06-30 00:19:23
#  History:         
=============================================================================*/
#ifndef _FSM_INTERFACE_H_20110630001921_
#define _FSM_INTERFACE_H_20110630001921_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <map>
#include <string>
#include "object_interface.h"
using namespace std;

class IFsm;
class IActor;

class IEvent
{
public:
    virtual ~IEvent () {}
};


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

    /**
     * @brief   当状态切换时，进行统计。包括actor在这个状态停留的时长等
     *
     * @param   fsm                 状态指针
     * @param   pActor              actor指针
     * @param   strFsmFunc          HandleEntry、HandleProcess、HandleExit
     */
    virtual void StatChangeFsm(IFsm *fsm, IActor* pActor, const string &strFsmFunc)=0;

    /**
     * @brief   统计增
     *
     * @param   key1
     * @param   key2
     * @param   index
     *
     * @return  
     */
    virtual int StatAddCount(const char* key1, const char* key2, int index)=0;

    /**
     * @brief   统计减
     *
     * @param   key1
     * @param   key2
     * @param   index
     *
     * @return  
     */
    virtual int StatDecCount(const char* key1, const char* key2, int index)=0;
};

class IActor : public IObject
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
     */
    virtual void SetFsmMgr(map<int, IFsm*> *ptrMapFsmMgr)=0;

    /**
     * @brief   获取关联状态机的管理器
     *
     * @return  map<int, IFsm*>*
     */
    virtual map<int, IFsm*>* GetFsmMgr()=0;

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

    /**
     * @brief   处理事件
     *
     * @param   pEvent
     *
     * @return  0
     */
    virtual int HandleEvent(IEvent* pEvent)=0;

    /**
     * @brief   清空状态变化历程列表
     */
    virtual void ClearFsmNodes()=0;

    /**
     * @brief   统计当进入某个fsm，会被frame调用
     *
     * @param   fsm
     *
     * @return  0
     */
    virtual int StatFsmEntry(IFsm* fsm)=0;

    /**
     * @brief   统计当退出某个fsm，会被frame调用
     *
     * @param   fsm
     *
     * @return  0
     */
    virtual int StatFsmExit(IFsm* fsm, int& pastTimeMs)=0;

    /**
     * @brief   获取整个actor存活的生命周期的时间
     *
     * @param   fsm
     *
     * @return  
     */
    virtual int GetAliveTimeMs()=0;

    /**
     * @brief   统计Actor从被标记GC到最终释放的时间
     *
     * @param   fsm
     *
     * @return  
     */
    virtual int GetGCTimeMs()=0;

};


class IFsm : public IObject
{
public:
    virtual ~IFsm() {}
    /**
     * @brief   设置Frame
     *
     * @param   pFrame
     *
     */
    virtual void SetFrame(IFrame* pFrame)=0;

    /**
     * @brief   设置state
     *
     * @param   state
     *
     */
    virtual void SetStateID(int state)=0;

    /**
     * @brief   获取StateId
     *
     * @return  state
     */
    virtual int GetStateID()=0;

    /**
     * @brief   在进入这个状态的时候，pActor需要做的事情
     *
     * @param   pActor
     *
     * @return  0           succ
     *          else        fail
     */
    virtual int Entry(IActor* pActor)=0;

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
    virtual int Exit(IActor* pActor)=0;
};

#endif
