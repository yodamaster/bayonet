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

#include "fsm_interface.h"
#include "dirstat.h"
#include "stat_def.h"
using namespace std;

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
        return 0;
    }

    int AddActor(IActor* pActor)
    {
        m_listActors.push_front(pActor);
        m_allActorCount++;
        StatAddCount("ALL","SELF",STAT_ALIVE);
        StatAddCount("ALL","SELF",STAT_TOTAL);

        StatAddCount(pActor->Name().c_str(),"SELF",STAT_ALIVE);
        StatAddCount(pActor->Name().c_str(),"SELF",STAT_TOTAL);

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
        fsm->AttachFrame(this);
        m_mapFsmMgr[state] = fsm;
        return 0;
    }
    virtual void ChangeFsmStat(IFsm *fsm, IActor* pActor, const string &strFsmFunc)
    {
        if (fsm == NULL || pActor == NULL)
        {
            return;
        }
        if (strFsmFunc == "Entry")
        {
            StatAddCount("ALL",fsm->Name().c_str(), STAT_ALIVE);
            StatAddCount("ALL",fsm->Name().c_str(), STAT_TOTAL);

            StatAddCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_ALIVE);
            StatAddCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_TOTAL);
        }
        else if (strFsmFunc == "Exit")
        {
            StatDecCount("ALL",fsm->Name().c_str(), STAT_ALIVE);
            StatDecCount(pActor->Name().c_str(),fsm->Name().c_str(), STAT_ALIVE);
        }
    }

    /**
     * @brief   统计增
     *
     * @param   key1
     * @param   key2
     * @param   index
     *
     * @return  
     */
    virtual int StatAddCount(const char* key1, const char* key2, int index)
    {
        return m_dirStat.AddCount(key1,key2,index);
    }

    /**
     * @brief   统计减
     *
     * @param   key1
     * @param   key2
     * @param   index
     *
     * @return  
     */
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
    /*
        {
        'ALL':{
            'SELF':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname1':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname2':{
                'ALIVE':count,
                'TOTAL':count,
            }
        },
        'GC':{
            'SELF':{
                'ALIVE':count,
                'TOTAL':count,
            },
        },
        'actorname':{
            'SELF':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname1':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname2':{
                'ALIVE':count,
                'TOTAL':count,
            }
        },
        'actorname2':{
            'SELF':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname1':{
                'ALIVE':count,
                'TOTAL':count,
            },
            'fsmname2':{
                'ALIVE':count,
                'TOTAL':count,
            }
        },
    }

    */
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
    }

    virtual ~CActorBase () {}

    virtual void SetGCMark()
    {
        if (m_bGC)
        {
            return;
        }
        m_bGC = true;
        if (m_pFrame)
        {
            m_pFrame->AddNeedGCCount();
        }
    }
    virtual bool GetGCMark()
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
        AttachFsmMgr(m_pFrame->GetFsmMgr());
        return 0;
    }
    IFrame* GetFrame()
    {
        return m_pFrame;
    }

    int AttachFsmMgr(map<int, IFsm*> * ptrMapFsmMgr)
    {
        m_ptrMapFsmMgr = ptrMapFsmMgr;
        return 0;
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
        m_vecStates.push_back(destState);
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
    vector<int> m_vecStates;//历史的states
};
class CFsmBase : public IFsm
{
public:
    CFsmBase () {
        m_pFrame = NULL;
    }
    virtual ~CFsmBase () {}
    int AttachFrame(IFrame* pFrame)
    {
        m_pFrame = pFrame;
        return 0;
    }

    int Entry(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->ChangeFsmStat(this, pActor, __func__);
        }
        return HandleEntry(pActor);
    }

    int Process(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->ChangeFsmStat(this, pActor, __func__);
        }
        return HandleProcess(pActor);
    }

    int Exit(IActor* pActor)
    {
        if (m_pFrame)
        {
            m_pFrame->ChangeFsmStat(this, pActor, __func__);
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
};
#endif
