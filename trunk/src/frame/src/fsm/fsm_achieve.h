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

#include "fsm_interface.h"
using namespace std;
class CFrameBase : public IFrame
{
public:
    virtual ~CFrameBase () {}

    int AddActor(IActor* pActor)
    {
        m_listActors.push_front(pActor);
        return 0;
    }
    int DelActor(IActor* pActor)
    {
        m_listActors.remove(pActor);
        return 0;
    }
    void GCActors()
    {
        for(list<IActor*>::iterator it = m_listActors.begin(); it != m_listActors.end(); ++it)
        {
            list<IActor*>::iterator tempIt = it;
            it++;

            if ((*tempIt)->GetGCMark())
            {
                delete (*tempIt);
                m_listActors.erase(tempIt);
            }
        }
    }

protected:
    list<IActor*> m_listActors;
};

class CActorBase : public IActor
{
public:
    CActorBase () : m_bGC(false),m_Fsm(NULL),m_ptrMapFsmMgr(NULL),m_pUpperActor(NULL),m_pFrame(NULL) {}

    virtual ~CActorBase () {}

    virtual void SetGCMark(bool bGC=true)
    {
        m_bGC = bGC;
    }
    virtual bool GetGCMark()
    {
        return m_bGC;
    }
    int AttachFrame(IFrame* pFrame)
    {
        m_pFrame = pFrame;
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
    int AttachUpperActor(IActor* pActor)
    {
        m_pUpperActor = pActor;
        return 0;
    }
    IActor* GetUpperActor()
    {
        return m_pUpperActor;
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
                m_Fsm->Fini(this);
                m_Fsm = NULL;
            }
            return destState;
        }
        IFsm * destFsm = NULL;
        destFsm = (*m_ptrMapFsmMgr)[destState];
        int state = doChangeFsm(destFsm);
        return ChangeState(state);
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
                m_Fsm->Fini(this);
            }
            m_Fsm = destFsm;
            m_Fsm->Init(this);
        }
        return m_Fsm->Process(this);
    }


protected:
    bool m_bGC;
    IFsm* m_Fsm;
    map<int, IFsm*> *m_ptrMapFsmMgr;
    IActor* m_pUpperActor;
    IFrame* m_pFrame;
};
#endif
