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
using namespace std;

#ifndef foreach
#define foreach(container,it) \
    for(typeof((container).begin()) it = (container).begin();it!=(container).end();++it)
#endif

#ifndef isnotlast
#define isnotlast(container,it) \
    typeof(it) tmp = it; \
    tmp++; \
    if (tmp != container.end())
#endif

class CFrameBase : public IFrame
{
public:
    CFrameBase () {
        m_needGCCount = 0;
        m_allActorCount = 0;
    }
    virtual ~CFrameBase () {}

    int AddActor(IActor* pActor)
    {
        m_listActors.push_front(pActor);
        m_allActorCount++;
        m_mapStat["ALL"]["SELF"]["ALIVE"]++;
        m_mapStat["ALL"]["SELF"]["TOTAL"]++;

        m_mapStat[pActor->Name()]["SELF"]["ALIVE"]++;
        m_mapStat[pActor->Name()]["SELF"]["TOTAL"]++;
        return 0;
    }
    int GetActorCount()
    {
        return m_allActorCount;
    }
    int AddNeedGCCount()
    {
        m_needGCCount++;
        m_mapStat["GC"]["SELF"]["ALIVE"]++;
        m_mapStat["GC"]["SELF"]["TOTAL"]++;
        return 0;
    }
    int SubNeedGCCount()
    {
        m_needGCCount--;
        if (m_needGCCount < 0)
        {
            m_needGCCount = 0;
        }
        m_mapStat["GC"]["SELF"]["ALIVE"]--;
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
            m_mapStat["ALL"][fsm->Name()]["TOTAL"]++;
            m_mapStat["ALL"][fsm->Name()]["ALIVE"]++;
            m_mapStat[pActor->Name()][fsm->Name()]["TOTAL"]++;
            m_mapStat[pActor->Name()][fsm->Name()]["ALIVE"]++;
        }
        else if (strFsmFunc == "Exit")
        {
            m_mapStat["ALL"][fsm->Name()]["ALIVE"]--;
            m_mapStat[pActor->Name()][fsm->Name()]["ALIVE"]--;
        }
    }
    virtual string GetStat()
    {
        stringstream ss;
        ss << "{" << endl;
        foreach(m_mapStat, it_a)
        {
            ss << "\t\"" << it_a->first << "\":{" << endl;
            foreach(it_a->second, it_b)
            {
                ss << "\t\t\"" << it_b->first << "\":{" << endl;
                foreach(it_b->second, it_c)
                {
                    ss << "\t\t\t\"" << it_c->first << "\":" << it_c->second;
                    isnotlast(it_b->second, it_c)
                    {
                        ss << ",";
                    }
                    ss << endl;
                }
                ss << "\t\t}";

                isnotlast(it_a->second, it_b)
                {
                    ss << ",";
                }
                ss << endl;
            }
            ss << "\t}";

            isnotlast(m_mapStat, it_a)
            {
                ss << ",";
            }
            ss << endl;
        }
        ss << "}" << endl;
        return ss.str();
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
        m_mapStat["ALL"]["SELF"]["ALIVE"]--;
        m_mapStat[(*it)->Name()]["SELF"]["ALIVE"]--;
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
    map<string, map<string, map<string, uint32_t> > > m_mapStat;
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
