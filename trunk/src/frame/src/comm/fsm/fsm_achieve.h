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
#include "dirstat.h"
#include "stat_def.h"
#include "timer.h"
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
        gettimeofday(&m_Start_TV, NULL);
        fsm = NULL;
    }

    int Stop()
    {
        gettimeofday(&m_Stop_TV, NULL);
        return 0;
    }
    int GetPastTime()
    {
        long past_time  = 0;
        past_time = ((m_Stop_TV.tv_sec  - m_Start_TV.tv_sec ) * 1000000 + (m_Stop_TV.tv_usec - m_Start_TV.tv_usec)) / 1000;
        return past_time;
    }
} StFsmNode;

/**
* @brief    映射时间到统计
*
* @param    msec
* @param    baseLine
*
* @return   
*/
static int MapTime2StatIndex(int msec,int baseLine)
{
    int statTimeIndex;
    if(msec<5)
        statTimeIndex = baseLine;
    else if(msec<10)
        statTimeIndex = baseLine+1;
    else if(msec<50)
        statTimeIndex = baseLine+2;
    else if(msec<100)
        statTimeIndex = baseLine+3;
    else if(msec<200)
        statTimeIndex = baseLine+4;
    else if(msec<500)
        statTimeIndex = baseLine+5;
    else if(msec<1000)
        statTimeIndex = baseLine+6;
    else if(msec<2000)
        statTimeIndex = baseLine+7;
    else if(msec<3000)
        statTimeIndex = baseLine+8;
    else if(msec<5000)
        statTimeIndex = baseLine+9;
    else
        statTimeIndex = baseLine+10;
    return statTimeIndex;
}

class CFrameBase : public IFrame
{
public:
    CFrameBase () {
        m_needGCCount = 0;
        m_allActorCount = 0;
    }
    virtual ~CFrameBase () {}

    int Init(const char* statDir, const char* statFileName)
    {
        int ret = m_dirStat.Init(statDir, statFileName,stat_desc, STAT_OVER);
        if (ret != 0)
        {
            return -1;
        }

        m_dirStat.ResetStat();
        return 0;
    }

    int AddActor(IActor* pActor)
    {
        m_listActors.push_front(pActor);
        m_allActorCount++;
        StatAddCount("ALL","SELF",STAT_ALIVE);
        StatAddCount("ALL","SELF",STAT_TOTAL);

        StatAddCount("ALL","VALID",STAT_ALIVE);
        StatAddCount("ALL","VALID",STAT_TOTAL);

        StatAddCount(pActor->Name().c_str(),"SELF",STAT_ALIVE);
        StatAddCount(pActor->Name().c_str(),"SELF",STAT_TOTAL);

        StatAddCount(pActor->Name().c_str(),"VALID",STAT_ALIVE);
        StatAddCount(pActor->Name().c_str(),"VALID",STAT_TOTAL);

        return 0;
    }
    int GetActorCount()
    {
        return m_allActorCount;
    }
    int AddNeedGCCount()
    {
        m_needGCCount++;
        StatAddCount("GC","SELF",STAT_ALIVE);
        StatAddCount("GC","SELF",STAT_TOTAL);
        return 0;
    }
    int SubNeedGCCount()
    {
        m_needGCCount--;
        if (m_needGCCount < 0)
        {
            m_needGCCount = 0;
        }
        StatDecCount("GC","SELF",STAT_ALIVE);
        return 0;
    }
    int GetNeedGCCount()
    {
        return m_needGCCount;
    }
    void GCActors()
    {
        for(list<IActor*>::iterator it = m_listActors.begin(); it != m_listActors.end(); ++it)
        {
            list<IActor*>::iterator tempIt = it;
            it++;

            if ((*tempIt)->GetGCMark())
            {
                eraseActor(tempIt);
            }
        }
    }
    map<int, IFsm*>* GetFsmMgr()
    {
        return &m_mapFsmMgr;
    }
    int RegFsm(int state, IFsm* fsm)
    {
        if (state <=0 || fsm == NULL)
        {
            return -1;
        }
        fsm->SetFrame(this);
        fsm->SetStateID(state);
        m_mapFsmMgr[state] = fsm;
        return 0;
    }
    virtual void StatChangeFsm(IFsm *fsm, IActor* pActor, const string &strFsmFunc)
    {
        if (fsm == NULL || pActor == NULL)
        {
            return;
        }
        if (strFsmFunc == "Entry")
        {
            pActor->StatFsmEntry(fsm);

            StatAddCount("ALL",fsm->Name().c_str(), STAT_ALIVE);
            StatAddCount("ALL",fsm->Name().c_str(), STAT_TOTAL);

            StatAddCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_ALIVE);
            StatAddCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_TOTAL);

        }
        else if (strFsmFunc == "Exit")
        {
            int pastTimeMs = 0;
            pActor->StatFsmExit(fsm,pastTimeMs);

            int statTimeIndex = MapTime2StatIndex(pastTimeMs, STAT_5MS_REQ);
            //统计fsm的时间
            StatAddCount("ALL",fsm->Name().c_str(), statTimeIndex);
            StatAddCount(pActor->Name().c_str(),fsm->Name().c_str(), statTimeIndex);

            StatDecCount("ALL",fsm->Name().c_str(), STAT_ALIVE);
            StatDecCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_ALIVE);
        }
    }

    virtual int StatAddCount(const char* key1, const char* key2, int index)
    {
        return m_dirStat.AddCount(key1,key2,index);
    }

    virtual int StatDecCount(const char* key1, const char* key2, int index)
    {
        return m_dirStat.DecCount(key1,key2,index);
    }

protected:
    /**
     * @brief   删除一个actor的指针
     *
     * @param   it
     *
     * @return  
     */
    int eraseActor(list<IActor*>::iterator it)
    {
        StatDecCount("ALL","SELF",STAT_ALIVE);
        StatDecCount((*it)->Name().c_str(),"SELF",STAT_ALIVE);

        //统计存活时间和GC时间
        int statTimeIndex = MapTime2StatIndex((*it)->GetAliveTimeMs(), STAT_5MS_REQ);
        StatAddCount("ALL","SELF",statTimeIndex);
        StatAddCount((*it)->Name().c_str(),"SELF",statTimeIndex);

        statTimeIndex = MapTime2StatIndex((*it)->GetGCTimeMs(),STAT_5MS_REQ);
        StatAddCount("GC","SELF",statTimeIndex);


        SubNeedGCCount();

        delete (*it);
        m_listActors.erase(it);
        m_allActorCount--;
        return 0;
    }
protected:
    list<IActor*> m_listActors;

    int m_needGCCount;
    int m_allActorCount;
    map<int, IFsm*> m_mapFsmMgr;

    string m_statDir;

    //统计
    CDirStat m_dirStat;
};

class CActorBase : public IActor
{
public:
    CActorBase () {
        m_bGC = false;
        m_Fsm = NULL;
        m_ptrMapFsmMgr = NULL;
        m_pFrame = NULL;

        m_aliveTimer.Start();
    }

    virtual ~CActorBase () {
    }

    void SetGCMark()
    {
        if (m_bGC)
        {
            return;
        }
        m_bGC = true;
        m_gcTimer.Start();
        if (m_pFrame)
        {
            m_pFrame->AddNeedGCCount();

            //统计有效的数量
            m_pFrame->StatDecCount("ALL","VALID",STAT_ALIVE);
            m_pFrame->StatDecCount(Name().c_str(),"VALID",STAT_ALIVE);

            int pastTimeMs = m_aliveTimer.GetPastTime();
            int statTimeIndex = MapTime2StatIndex(pastTimeMs, STAT_5MS_REQ);
            m_pFrame->StatAddCount("ALL","VALID",statTimeIndex);
            m_pFrame->StatAddCount(Name().c_str(),"VALID",statTimeIndex);
        }
    }
    bool GetGCMark()
    {
        return m_bGC;
    }
    int AttachFrame(IFrame* pFrame)
    {
        if (pFrame == NULL)
        {
            return -1;
        }
        m_pFrame = pFrame;
        m_pFrame->AddActor(this);
        SetFsmMgr(m_pFrame->GetFsmMgr());
        return 0;
    }
    IFrame* GetFrame()
    {
        return m_pFrame;
    }

    void SetFsmMgr(map<int, IFsm*> * ptrMapFsmMgr)
    {
        m_ptrMapFsmMgr = ptrMapFsmMgr;
    }
    map<int, IFsm*>* GetFsmMgr()
    {
        return m_ptrMapFsmMgr;
    }

    int ProcessState()
    {
        if (m_ptrMapFsmMgr == NULL)
        {
            return -1;
        }
        int state = doChangeFsm(m_Fsm);
        return ChangeState(state);
    }

    int ChangeState(int destState)
    {
        if (m_ptrMapFsmMgr == NULL)
        {
            return -1;
        }

        if (0 == destState)
        {
            //此次处理结束,但是并没有退出这个状态
            return 0;
        }
        else if (destState < 0)
        {
            //需要关闭整个请求
            if (m_Fsm)
            {
                m_Fsm->Exit(this);
                m_Fsm = NULL;
            }
            return destState;
        }
        IFsm * destFsm = NULL;
        destFsm = (*m_ptrMapFsmMgr)[destState];
        int state = doChangeFsm(destFsm);
        return ChangeState(state);
    }
    int HandleEvent(IEvent* pEvent)
    {
        //默认是什么也不做的，继承的类如果需要用到，就要重写
        return 0;
    }

    virtual int StatFsmEntry(IFsm* fsm)
    {
        StFsmNode node;
        node.fsm = fsm;
        m_vecFsmNodes.push_back(node);
        return 0;
    }

    virtual int StatFsmExit(IFsm* fsm, int& pastTimeMs)
    {
        if (m_vecFsmNodes.empty())
        {
            return -1;
        }
        StFsmNode& node = m_vecFsmNodes[m_vecFsmNodes.size()-1];
        if (node.fsm != fsm)
        {
            return -2;
        }

        node.Stop();
        pastTimeMs = node.GetPastTime();
        return 0;
    }

    int GetAliveTimeMs()
    {
        return m_aliveTimer.GetPastTime();
    }

    int GetGCTimeMs()
    {
        return m_gcTimer.GetPastTime();
    }

private:
    int doChangeFsm(IFsm* destFsm)
    {
        if (destFsm == NULL)
        {
            return 0;
        }

        if (m_Fsm != destFsm)
        {
            if (m_Fsm != NULL)
            {
                m_Fsm->Exit(this);
            }
            m_Fsm = destFsm;
            m_Fsm->Entry(this);
        }
        return m_Fsm->Process(this);
    }

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
    CFsmBase () {
        m_pFrame = NULL;
        m_stateID = -1;
    }
    virtual ~CFsmBase () {}
    void SetFrame(IFrame* pFrame)
    {
        m_pFrame = pFrame;
    }

    void SetStateID(int state)
    {
        m_stateID = state;
    }

    int GetStateID()
    {
        return m_stateID;
    }

    int Entry(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->StatChangeFsm(this, pActor, __func__);
        }
        return HandleEntry(pActor);
    }

    int Process(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->StatChangeFsm(this, pActor, __func__);
        }
        return HandleProcess(pActor);
    }

    int Exit(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->StatChangeFsm(this, pActor, __func__);
        }
        return HandleExit(pActor);
    }

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
