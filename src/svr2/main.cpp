#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "bayonet_frame.h"

#define APP_FSM_LOGIC1 2000

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
    // 为发送打包
    int HandleEncodeSendBuf(
            IActor* pSocketActor,
            IActor* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        strSendBuf = app_actor->m_str+"_x_";
        len = strSendBuf.size();
        return 0;
    }

    // 回应包完整性检查
    int HandleInput(
            IActor* pSocketActor,
            IActor* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    // 回应包解析
    int HandleDecodeRecvBuf(
            IActor* pSocketActor,
            IActor* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor * app_actor = new CMyActor();
        app_actor->AttachFrame(pSocketActor->GetFrame());
        app_actor->AttachCommu(pSocketActor);
        app_actor->ChangeState(APP_FSM_LOGIC1);
        trace_log("listen tcp HandleDecodeRecvBuf");
        return 0;
    }
};

class CActionGetData: public IAction
{
public:
    // 为发送打包
    int HandleEncodeSendBuf(
            IActor* pSocketActor,
            IActor* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        strSendBuf=app_actor->m_str+string("_gaoshenma");
        len = strSendBuf.size();
        return 0;
    }

    // 回应包完整性检查
    int HandleInput(
            IActor* pSocketActor,
            IActor* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    // 回应包解析
    int HandleDecodeRecvBuf(
            IActor* pSocketActor,
            IActor* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        app_actor->m_str+=string(buf);
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
        param.protoType = PROTO_TYPE_UDP;
        param.pAction = &actionGetData;
        //param.actionType = ACTIONTYPE_SENDONLY;
        param.actionType = ACTIONTYPE_SENDRECV;
        param.timeout_ms = 1000;

        CActionInfo * pActionInfo = new CActionInfo();
        pActionInfo->Init(param);
        pActionInfo->SetAppActor(pAppActor);
        pActionInfoSet->Add(pActionInfo);
        return 0;
    }
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        trace_log("HandleProcess");
        set<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(set<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            trace_log("error no:%d",(*it)->GetErrno());
        }
        return APP_FSM_RSP;//代表要回复客户端啦
    }
    virtual int HandleExit(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        return 0;
    }
};
int main(int argc, const char *argv[])
{
    CBayonetFrame srv;
    StFrameParam param;
    param.infoDir="bayonet";
    param.ip="0.0.0.0";
    param.port = 10001;
    param.bKeepcnt= true;
    //param.protoType = PROTO_TYPE_UDP;
    param.protoType = PROTO_TYPE_TCP;
    param.pAction = new CActionFirst();

    int ret = srv.Init(param);
    if (ret != 0)
    {
        return -1;
    }
    srv.RegFsm(APP_FSM_LOGIC1,new CAppFsmLogic1());
    srv.Process();
    return 0;
}
