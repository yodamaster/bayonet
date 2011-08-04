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

using bayonet::IAction;
using bayonet::CActionInfo;
using bayonet::CActionInfoSet;
using bayonet::StActionInfoParam;
using bayonet::CAppActorBase;
using bayonet::CSocketActorData;
using bayonet::CAppFsmBase;
using bayonet::CBayonetFrame;

class CAppFsmLogic1;
class CActionFirst : public IAction
{
public:
    int HandleEncodeSendBuf(
            CSocketActorData* pSocketActor,
            CAppActorBase* pAppActor,
            string & strSendBuf,
            int &len)
    {
        byt_trace_log("send");
        strSendBuf="msg from svr";
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
        CAppActorBase * app_actor = new CAppActorBase();
        app_actor->AttachFrame(pSocketActor->GetFrame());
        app_actor->AttachCommu(pSocketActor);
        app_actor->ChangeState(APP_FSM_LOGIC1);
        byt_trace_log("listen tcp HandleDecodeRecvBuf");
        return 0;
    }
};

class CAppFsmLogic1 : public CAppFsmBase
{
public:
    virtual ~CAppFsmLogic1 () {}
    virtual int HandleEntry(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        static CActionFirst actionFirst;
        StActionInfoParam param;
        param.id = 1;
        param.ip = "127.0.0.1";
        param.port = 100;
        param.protoType = bayonet::PROTO_TYPE_UDP;
        param.pAction = &actionFirst;
        param.actionType = bayonet::ACTIONTYPE_SENDONLY;
        param.timeout_ms = 1000;

        /*CActionInfo * pActionInfo = new CActionInfo();
        pActionInfo->Init(param);
        pActionInfo->SetAppActor(pAppActor);
        pActionInfoSet->Add(pActionInfo);*/
        return 0;
    }
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        byt_trace_log("HandleProcess");
        /*set<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(set<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            byt_trace_log("error no:%d",(*it)->GetErrno());
        }*/
        return bayonet::APP_FSM_RSP;//代表要回复客户端啦
    }
    virtual int HandleExit(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
int main(int argc, const char *argv[])
{
    CBayonetFrame srv;
    int ret = srv.Init("./bayonet.xml", new CActionFirst());
    if (ret != 0)
    {
        return -1;
    }
    srv.RegFsm(APP_FSM_LOGIC1,new CAppFsmLogic1());
    srv.Process();
    return 0;
}
