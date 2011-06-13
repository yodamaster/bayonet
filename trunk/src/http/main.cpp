#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
#include "bayonet_frame.h"

#define APP_FSM_PROXY 2000
#define APP_FSM_LOGIC2 2001

class CMyActor : public CAppActorBase
{
public:
    CMyActor() {}
    virtual ~CMyActor() {}

    string m_req;
    string m_rsp;
};

class CAppFsmProxy;
class CActionFirst : public IAction
{
public:
    // 为发送打包
    int HandleEncodeSendBuf(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        if (app_actor == NULL)
        {
            return -1;
        }
        strSendBuf = app_actor->m_rsp;
        len = strSendBuf.size();
        return 0;
    }

    // 回应包完整性检查
    int HandleInput(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    // 接受包解析
    int HandleDecodeRecvBuf(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor * app_actor = new CMyActor();
        app_actor->AttachFrame(pSocketActor->GetFrame());
        app_actor->AttachCommu(pSocketActor);
        app_actor->ChangeState(APP_FSM_PROXY);

        app_actor->m_req = string(buf,len);
        return 0;
    }
};

class CActionGetData: public IAction
{
public:
    // 为发送打包
    int HandleEncodeSendBuf(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            string & strSendBuf,
            int &len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        if (app_actor == NULL)
        {
            return -1;
        }
        strSendBuf=app_actor->m_req;
        len = strSendBuf.size();
        return 0;
    }

    // 回应包完整性检查
    int HandleInput(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            const char *buf,
            int len)
    {
        return len;
    }

    // 回应包解析
    int HandleDecodeRecvBuf(
            CActorBase* pSocketActor,
            CActorBase* pAppActor,
            const char *buf, 
            int len)
    {
        CMyActor* app_actor = (CMyActor*)pAppActor;
        //因为很有可能，appactor已经由于commu超时的原因被析构掉了
        if (app_actor == NULL)
        {
            return -1;
        }
        app_actor->m_rsp = string(buf,len);
        return 0;
    }
};

class CAppFsmProxy : public CAppFsmBase
{
public:
    virtual ~CAppFsmProxy () {}
    virtual int HandleEntry(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        static CActionGetData actionGetData;
        StActionInfoParam param;
        param.id = 1;
        param.ip = "10.6.207.189";
        param.port = 80;
        param.protoType = PROTO_TYPE_TCP;
        param.pAction = &actionGetData;
        param.actionType = ACTIONTYPE_SENDRECV;
        param.timeout_ms = 1000;

        CActionInfo * pActionInfo = new CActionInfo();
        pActionInfo->Init(param);
        pActionInfoSet->Add(pActionInfo);

        return 0;
    }
    virtual int HandleProcess(CActionInfoSet *pActionInfoSet, CAppActorBase* pAppActor)
    {
        set<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(set<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            trace_log("id:%d,error no:%d,timecost:%u ms",(*it)->GetID(),(*it)->GetErrno(),(*it)->GetTimeCost());
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
    param.port = 20001;
    //param.bKeepcnt= true;
    //param.protoType = PROTO_TYPE_UDP;
    param.protoType = PROTO_TYPE_TCP;
    param.pAction = new CActionFirst();
    param.gcMaxCount = 10;

    int ret = srv.Init(param);
    if (ret != 0)
    {
        return -1;
    }
    srv.RegFsm(APP_FSM_PROXY,new CAppFsmProxy());
    srv.Process();
    return 0;
}
