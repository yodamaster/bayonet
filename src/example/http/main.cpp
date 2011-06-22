#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "bayonet_frame.h"
using namespace std;

#define APP_FSM_PROXY 2000

/**
 * @brief   获取ContentLen的数字的起始和长度
 *
 * @param   strHttpBuf
 * @param   len
 *
 * @return  
 */
size_t GetContentLenPos(const string& strHttpBuf, int& len)
{
    string strContentLenKey = "Content-Length";

    size_t pos;
    size_t end_pos;
    pos = strHttpBuf.find(strContentLenKey);
    if (pos == string::npos)
    {
        return pos;
    }
    pos += strContentLenKey.size();

    pos = strHttpBuf.find(":", pos);
    if (pos == string::npos)
    {
        return pos;
    }
    pos += 1;

    end_pos = strHttpBuf.find("\r\n", pos);
    if (end_pos == string::npos)
    {
        return end_pos;
    }

    pos = strHttpBuf.find_first_not_of(" ", pos);//只能查找字符
    if (pos == string::npos || pos >= end_pos)
    {
        return pos;
    }
    len = end_pos - pos;
    return pos;
}

int HttpHandleInput(const char* buf, int len)
{
    string strHttpBuf(buf,len);

    int contentLenLen = 0;
    size_t contentLenPos = GetContentLenPos(strHttpBuf,contentLenLen);
    if (contentLenPos == string::npos)
    {
        //说明直接接收完了
        return len;
    }

    //获取原来的content-len的值
    string lenNum = strHttpBuf.substr(contentLenPos, contentLenLen);
    int iContentLen = atoi(lenNum.c_str());

    //接下来我们要看看当前接受的buf大小是否等于 head + content len

    string spStr = "\r\n\r\n";
    size_t spPos = strHttpBuf.find(spStr);
    if (spPos == string::npos)
    {
        //没接收完，继续接收
        return 0;
    }
    int headLen = spPos+spStr.size();

    int realLen = headLen + iContentLen;
    if (realLen > len)
    {
        return 0;
    }

    return realLen;
}

class CMyActor : public CAppActorBase
{
public:
    CMyActor() {}
    virtual ~CMyActor() {}

    string m_req;
    string m_rsp;
};

class CActionFirst : public IAction
{
public:
    int HandleInit(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor)
    {
        int fd = pSocketActor->GetSocketFd();
        if (fd >= 0)
        {
            int rcvbuf; 
            socklen_t rcvbufsize=sizeof(int); 

            if(getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*) 
                          &rcvbuf,&rcvbufsize)!=-1) 
            { 
                if(rcvbuf<65536) 
                {
                    rcvbuf=65536; 
                    setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*) 
                               &rcvbuf,rcvbufsize); 
                }
            } 

            if(getsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*) 
                          &rcvbuf,&rcvbufsize)!=-1) 
            { 
                if(rcvbuf<65536) 
                {
                    rcvbuf=65536; 
                    setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*) 
                               &rcvbuf,rcvbufsize); 
                }
            }  
        }
        pSocketActor->ResizeRecvBuf(4096,4096);
        return 0;
    }
    int HandleEncodeSendBuf(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
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

    int HandleInput(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
        const char *buf,
        int len)
    {
        return HttpHandleInput(buf,len);
    }

    int HandleDecodeRecvBuf(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
        const char *buf, 
        int len)
    {
        CMyActor * app_actor = new CMyActor();
        app_actor->AttachFrame(pSocketActor->GetFrame());
        app_actor->AttachCommu(pSocketActor);

        app_actor->m_req = string(buf,len);

        //转化状态操作一定要放在最后一步
        app_actor->ChangeState(APP_FSM_PROXY);
        return 0;
    }
};

class CActionGetData: public IAction
{
public:
    // 为发送打包
    int HandleEncodeSendBuf(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
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
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
        const char *buf,
        int len)
    {
        return HttpHandleInput(buf,len);
    }

    // 回应包解析
    int HandleDecodeRecvBuf(
        CSocketActorData* pSocketActor,
        CAppActorBase* pAppActor,
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
        /*set<CActionInfo*> &setAction = pActionInfoSet->GetActionSet();
        for(set<CActionInfo*>::iterator it = setAction.begin(); it != setAction.end(); ++it)
        {
            trace_log("id:%d,error no:%d,timecost:%u ms",(*it)->GetID(),(*it)->GetErrno(),(*it)->GetTimeCost());
        }*/
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
