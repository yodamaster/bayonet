/*=============================================================================
#
#     FileName: fsm_achieve.cpp
#         Desc: fsm实现
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-06-15 10:36:37
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-06-15 10:36:37 | initialization
#
=============================================================================*/
#include "fsm_achieve.h"

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

//=============================================================================
CFrameBase::CFrameBase () {
    m_needGCCount = 0;
    m_allActorCount = 0;
}
CFrameBase::~CFrameBase () {}

int CFrameBase::Init(const char* statPath)
{
    int ret = m_mapStat.Init(statPath);
    if (ret != 0)
    {
        return -1;
    }

    m_mapStat.Clear();
    return 0;
}

int CFrameBase::AddActor(IActor* pActor)
{
    m_listActors.push_back(pActor);
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
int CFrameBase::GetActorCount()
{
    return m_allActorCount;
}
int CFrameBase::AddNeedGCCount()
{
    m_needGCCount++;
    StatAddCount("GC","SELF",STAT_ALIVE);
    StatAddCount("GC","SELF",STAT_TOTAL);
    return 0;
}
int CFrameBase::SubNeedGCCount()
{
    m_needGCCount--;
    if (m_needGCCount < 0)
    {
        m_needGCCount = 0;
    }
    StatDecCount("GC","SELF",STAT_ALIVE);
    return 0;
}
int CFrameBase::GetNeedGCCount()
{
    return m_needGCCount;
}
void CFrameBase::GCActors()
{
    for(list<IActor*>::iterator it = m_listActors.begin(); it != m_listActors.end();)
    {
        //list<IActor*>::iterator tempIt = it;
        //it++;

        if ((*it)->GetGCMark())
        {
            it = eraseActor(it);
        }
        else
        {
            ++it;
        }
    }
}
map<int, IFsm*>* CFrameBase::GetFsmMgr()
{
    return &m_mapFsmMgr;
}
int CFrameBase::RegFsm(int state, IFsm* fsm)
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
void CFrameBase::StatChangeFsm(IFsm *fsm, IActor* pActor, const string &strFsmFunc)
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

int CFrameBase::StatAddCount(const char* key1, const char* key2, int index)
{
    /*if (!(strcmp(key1,"CSocketActorPassiveTcp") == 0 && strcmp(key2,"CSocketFsmRecving") == 0))
    {
        return 0;
    }*/

    static char szTmp[256];
    snprintf(szTmp, sizeof(szTmp), "%s*%s", key1, key2);
    return m_mapStat.AddCount(szTmp,index);
}

int CFrameBase::StatDecCount(const char* key1, const char* key2, int index)
{
    /*if (!(strcmp(key1,"CSocketActorPassiveTcp") == 0 && strcmp(key2,"CSocketFsmRecving") == 0))
    {
        return 0;
    }*/

    static char szTmp[256];
    snprintf(szTmp, sizeof(szTmp), "%s*%s", key1, key2);
    return m_mapStat.DecCount(szTmp,index);
}

list<IActor*>::iterator CFrameBase::eraseActor(list<IActor*>::iterator it)
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
    list<IActor*>::iterator next_it = m_listActors.erase(it);
    m_allActorCount--;
    return next_it;
}

//=============================================================================
CActorBase::CActorBase () {
    m_bGC = false;
    m_Fsm = NULL;
    m_ptrMapFsmMgr = NULL;
    m_pFrame = NULL;

    m_aliveTimer.Start();
}

CActorBase::~CActorBase () {}

void CActorBase::SetGCMark()
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
bool CActorBase::GetGCMark()
{
    return m_bGC;
}
int CActorBase::AttachFrame(IFrame* pFrame)
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
IFrame* CActorBase::GetFrame()
{
    return m_pFrame;
}

void CActorBase::SetFsmMgr(map<int, IFsm*> * ptrMapFsmMgr)
{
    m_ptrMapFsmMgr = ptrMapFsmMgr;
}
map<int, IFsm*>* CActorBase::GetFsmMgr()
{
    return m_ptrMapFsmMgr;
}

int CActorBase::ProcessState()
{
    if (m_ptrMapFsmMgr == NULL)
    {
        return -1;
    }
    int state = doChangeFsm(m_Fsm);
    return ChangeState(state);
}

int CActorBase::ChangeState(int destState)
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
int CActorBase::HandleEvent(IEvent* pEvent)
{
    //默认是什么也不做的，继承的类如果需要用到，就要重写
    return 0;
}

void CActorBase::ClearFsmNodes()
{
    m_vecFsmNodes.clear();
}

int CActorBase::StatFsmEntry(IFsm* fsm)
{
    StFsmNode node;
    node.fsm = fsm;
    m_vecFsmNodes.push_back(node);
    return 0;
}

int CActorBase::StatFsmExit(IFsm* fsm, int& pastTimeMs)
{
    if (m_vecFsmNodes.empty())
    {
        return -1;
    }
    StFsmNode& node = m_vecFsmNodes.back();
    if (node.fsm != fsm)
    {
        return -2;
    }

    node.Stop();
    pastTimeMs = node.GetPastTime();
    return 0;
}

int CActorBase::GetAliveTimeMs()
{
    return m_aliveTimer.GetPastTime();
}

int CActorBase::GetGCTimeMs()
{
    return m_gcTimer.GetPastTime();
}

int CActorBase::doChangeFsm(IFsm* destFsm)
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

//=============================================================================
CFsmBase::CFsmBase () {
    m_pFrame = NULL;
    m_stateID = -1;
}
CFsmBase::~CFsmBase () {}
void CFsmBase::SetFrame(IFrame* pFrame)
{
    m_pFrame = pFrame;
}

void CFsmBase::SetStateID(int state)
{
    m_stateID = state;
}

int CFsmBase::GetStateID()
{
    return m_stateID;
}

int CFsmBase::Entry(IActor* pActor)
{
    if (m_pFrame)
    {
        m_pFrame->StatChangeFsm(this, pActor, __func__);
    }
    return HandleEntry(pActor);
}

int CFsmBase::Process(IActor* pActor)
{
    if (m_pFrame)
    {
        m_pFrame->StatChangeFsm(this, pActor, __func__);
    }
    return HandleProcess(pActor);
}

int CFsmBase::Exit(IActor* pActor)
{
    if (m_pFrame)
    {
        m_pFrame->StatChangeFsm(this, pActor, __func__);
    }
    return HandleExit(pActor);
}
