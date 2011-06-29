/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        fsm.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-19 12:13:02
#  History:         
=============================================================================*/
#ifndef _FSM_H_
#define _FSM_H_
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <sstream>

#include <sys/time.h>

#include "fsm_interface.h"
#include "stat_def.h"
#include "timer.h"
#include "ptr_proxy.h"
#include "map_stat.h"
using namespace std;

/**
 * @brief   统计每个actor状态变化的路径以及经过的时间
 */
typedef struct _StFsmNode
{
    IFsm* fsm;
    struct timeval m_Start_TV;
    struct timeval m_Stop_TV;

    _StFsmNode()
    {
        m_Start_TV = CRecordTime::Ins()->get_time();
        fsm = NULL;
    }

    int Stop()
    {
        m_Stop_TV = CRecordTime::Ins()->get_time();
        return 0;
    }
    int GetPastTime()
    {
        long past_time  = 0;
        past_time = ((m_Stop_TV.tv_sec  - m_Start_TV.tv_sec ) * 1000000 + (m_Stop_TV.tv_usec - m_Start_TV.tv_usec)) / 1000;
        return past_time;
    }
} StFsmNode;

class CFrameBase : public IFrame
{
public:
    CFrameBase ();
    virtual ~CFrameBase ();

    /**
     * @brief   初始化
     *
     * @param   statPath            文件路径
     *
     * @return  0                   succ
     */
    int Init(const char* statPath);

    int AddActor(IActor* pActor);

    int GetActorCount();

    int AddNeedGCCount();

    int SubNeedGCCount();

    int GetNeedGCCount();

    void GCActors();

    map<int, IFsm*>* GetFsmMgr();

    int RegFsm(int state, IFsm* fsm);


    virtual void StatChangeFsm(IFsm *fsm, IActor* pActor, const string &strFsmFunc);

    virtual int StatAddCount(const char* key1, const char* key2, int index);

    virtual int StatDecCount(const char* key1, const char* key2, int index);

protected:
    /**
     * @brief   删除一个actor的指针
     *
     * @param   it                  actor的stl指针
     *
     * @return  0                   succ
     */
    list<IActor*>::iterator eraseActor(list<IActor*>::iterator it);

protected:
    list<IActor*> m_listActors;

    int m_needGCCount;
    int m_allActorCount;
    map<int, IFsm*> m_mapFsmMgr;

    string m_statDir;

    //统计
    CMapStat m_mapStat;
};

class CActorBase : public IActor, public IPtrProxy<CActorBase>
{
public:
    CActorBase ();
    virtual ~CActorBase ();

    int AttachFrame(IFrame* pFrame);

    IFrame* GetFrame();

    void SetGCMark();

    bool GetGCMark();

    void SetFsmMgr(map<int, IFsm*> * ptrMapFsmMgr);

    map<int, IFsm*>* GetFsmMgr();

    int ProcessState();

    int ChangeState(int destState);

    int HandleEvent(IEvent* pEvent);

    void ClearFsmNodes();

    int StatFsmEntry(IFsm* fsm);

    int StatFsmExit(IFsm* fsm, int& pastTimeMs);

    int GetAliveTimeMs();

    int GetGCTimeMs();

private:
    int doChangeFsm(IFsm* destFsm);

protected:
    bool m_bGC;
    IFsm* m_Fsm;
    map<int, IFsm*> *m_ptrMapFsmMgr;
    IFrame* m_pFrame;
    vector<StFsmNode> m_vecFsmNodes;//历史的states

    CTimer m_aliveTimer;
    CTimer m_gcTimer;
};
class CFsmBase : public IFsm
{
public:
    CFsmBase ();
    virtual ~CFsmBase ();

    void SetFrame(IFrame* pFrame);

    void SetStateID(int state);

    int GetStateID();

    int Entry(IActor* pActor);

    int Process(IActor* pActor);

    int Exit(IActor* pActor);

    /**
     * @brief   在进入这个状态的时候，pActor需要做的事情
     *
     * @param   pActor
     *
     * @return  0           succ
     *          else        fail
     */
    virtual int HandleEntry(IActor* pActor)=0;

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
    virtual int HandleProcess(IActor* pActor)=0;

    /**
     * @brief   退出这个状态时需要做的事情
     *
     * @param   pActor
     *
     * @return  0           succ
     *          else        fail
     */
    virtual int HandleExit(IActor* pActor)=0;

private:
    IFrame* m_pFrame;

    int m_stateID;
};
#endif
