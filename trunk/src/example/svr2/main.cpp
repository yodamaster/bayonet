#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
//#include "bayonet_frame.h"
#include "bayonet_pub.h"

#define APP_FSM_LOGIC1 2000
#define APP_FSM_LOGIC2 2001

using bayonet::IAction;
using bayonet::CActionInfo;
using bayonet::CActionInfoSet;
using bayonet::StActionInfoParam;
using bayonet::CAppActorBase;
using bayonet::CSocketActorData;
using bayonet::CAppFsmBase;
using bayonet::CBayonetFrame;

class CMyActor : public CAppActorBase
{
public:
    CMyActor() {}
    virtual ~CMyActor() {}

    string m_str;
};

class CAppFsmLogic1;
class CActionFirst : public IAction
{
public:
    int HandleInit(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor)
    {
        return 0;
    }
    int HandleEncodeSendBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        strSendBuf = app_actor->m_str+"|msg append by CActionFirst";
        len = strSendBuf.size();
        return 0;
    }

    int HandleInput(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    int HandleDecodeRecvBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor * app_actor = new CMyActor();
        app_actor->SetTimeOutMs(100);
        app_actor->AttachFrame(pSocketActor->GetFrame());
        app_actor->AttachCommu(pSocketActor);
        app_actor->ChangeState(APP_FSM_LOGIC1);
        return 0;
    }
};

class CActionGetData: public IAction
{
public:
    int HandleInit(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor)
    {
        return 0;
    }
    int HandleEncodeSendBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        strSendBuf=app_actor->m_str+string("|msg append by CActionGetData");
        len = strSendBuf.size();
        return 0;
    }

    int HandleInput(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    int HandleDecodeRecvBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        app_actor->m_str.append("|");
        app_actor->m_str.append(buf);
        return 0;
    }
};

class CAppFsmLogic1 : public CAppFsmBase
{
public:
    virtual ~CAppFsmLogic1 () {}
    virtual int HandleEntry(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        static CActionGetData actionGetData;
        StActionInfoParam param;
        param.id = 1;
        param.ip = "0.0.0.0";
        param.port = 20000;
        param.protoType = bayonet::PROTO_TYPE_UDP;
        //param.protoType = PROTO_TYPE_TCP;
        param.pAction = &actionGetData;
        //param.actionType = ACTIONTYPE_SENDONLY;
        param.actionType = bayonet::ACTIONTYPE_SENDRECV;
        param.timeout_ms = 100;

        CActionInfo * pActionInfo = new CActionInfo();
        pActionInfo->Init(param);
        pActionInfoSet->Add(pActionInfo);

        StActionInfoParam param2;
        param2.id = 2;
        param2.ip = "0.0.0.0";
        param2.port = 20000;
        param2.protoType = bayonet::PROTO_TYPE_UDP;
        //param2.protoType = PROTO_TYPE_TCP;
        param2.pAction = &actionGetData;
        //param2.actionType = ACTIONTYPE_SENDONLY;
        param2.actionType = bayonet::ACTIONTYPE_SENDRECV;
        param2.timeout_ms = 100;

        /*CActionInfo * pActionInfo2 = new CActionInfo();
        pActionInfo2->Init(param2);
        pActionInfoSet->Add(pActionInfo2);*/
        return 0;
    }
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        list<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(list<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            byt_trace_log("id:%d,error no:%d,timecost:%u ms",(*it)->GetID(),(*it)->GetErrno(),(*it)->GetTimeCost());
        }
        //return APP_FSM_RSP;//代表要回复客户端啦
        return APP_FSM_LOGIC2;//代表要回复客户端啦
    }
    virtual int HandleExit(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};

class CAppFsmLogic2 : public CAppFsmBase
{
public:
    virtual ~CAppFsmLogic2 () {}
    virtual int HandleEntry(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        static CActionGetData actionGetData;
        StActionInfoParam param;
        param.id = 1;
        param.ip = "0.0.0.0";
        param.port = 20000;
        param.protoType = bayonet::PROTO_TYPE_UDP;
        param.pAction = &actionGetData;
        //param.actionType = ACTIONTYPE_SENDONLY;
        param.actionType = bayonet::ACTIONTYPE_SENDRECV;
        param.timeout_ms = 100;

        CActionInfo * pActionInfo = new CActionInfo();
        pActionInfo->Init(param);
        pActionInfoSet->Add(pActionInfo);

        StActionInfoParam param2;
        param2.id = 2;
        param2.ip = "0.0.0.0";
        param2.port = 20000;
        param2.protoType = bayonet::PROTO_TYPE_UDP;
        param2.pAction = &actionGetData;
        //param2.actionType = ACTIONTYPE_SENDONLY;
        param2.actionType = bayonet::ACTIONTYPE_SENDRECV;
        param2.timeout_ms = 100;

        /*CActionInfo * pActionInfo2 = new CActionInfo();
        pActionInfo2->Init(param2);
        pActionInfoSet->Add(pActionInfo2);*/
        return 0;
    }
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        /*set<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(set<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            byt_trace_log("id:%d,error no:%d,timecost:%u ms",(*it)->GetID(),(*it)->GetErrno(),(*it)->GetTimeCost());
        }*/
        return bayonet::APP_FSM_RSP;//代表要回复客户端啦
    }
    virtual int HandleExit(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
static void sighandler( int sig_no )
{
    exit(0);
}
int main(int argc, const char *argv[])
{
    signal( SIGUSR1, sighandler );

    CBayonetFrame srv;
    int ret = srv.Init("./bayonet.xml", new CActionFirst());
    if (ret != 0)
    {
        return -1;
    }
    srv.RegFsm(APP_FSM_LOGIC1,new CAppFsmLogic1());
    srv.RegFsm(APP_FSM_LOGIC2,new CAppFsmLogic2());
    srv.Process();
    return 0;
}
