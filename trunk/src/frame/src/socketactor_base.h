/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_base.h
#  Description:     socketactor的基础类
#  Version:         1.0
#  LastChange:      2011-01-24 20:23:53
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_BASE_H_
#define _SOCKETACTOR_BASE_H_
#include "comm_def.h"
#include "fsm_achieve.h"
#include "epoller.h"

//=============================================================================
class CEPoller;
class CSocketActorBase:public CActorBase
{
public:
    CSocketActorBase ():m_SocketFd(-1),m_pEpoller(NULL) {}
    virtual ~CSocketActorBase () {}

    int Init(string ip,int port,int timeout_ms,int protoType);
    int SetProtoType(int protoType);
    int SetTimeout(int timeout_ms);

    int SetSocketFd(int socketId);
    int GetSocketFd();

    int AttachEpoller(CEPoller* epoller);
    int DetachEpoller();

    virtual int SetEvent(unsigned event);

    virtual int CheckTimeOut(struct timeval& now_time);

    virtual int OnInit();

    virtual int OnFini();

    virtual int OnRecv();

    virtual int OnRecvOver();

    virtual int OnSend();

    virtual int OnSendOver();

    virtual int OnClose();

    virtual int OnCloseOver();

    virtual int OnTimeout();

    virtual int OnError();

protected:
    int m_SocketFd;
    CEPoller* m_pEpoller;

    string m_IP;
    int m_Port;
    int m_TimeoutMs;
    int m_ProtoType;

    //业务需要继承实现
protected:
    //清理
    virtual int HandleFini()
    {
        return SOCKET_FSM_ALLOVER;
    }
    // socket close完毕
    virtual int HandleCloseOver()
    {
        return SOCKET_FSM_FINI;
    }

    virtual int HandleTimeout(
            int timeout_ms)
    {
        return SOCKET_FSM_CLOSING;
    }

    virtual int HandleError(
            int err_no)
    {
        return SOCKET_FSM_CLOSING;
    }

    //初始化
    virtual int HandleInit()=0;
    // 发送包接收完毕
    virtual int HandleSendOver()=0;
    // 回应包接受完毕
    virtual int HandleRecvOver(const char *buf, int len)=0;
};

//=============================================================================
class CSocketActorData:public CSocketActorBase
{
public:
    virtual ~CSocketActorData () {}

    virtual int OnRecv();

    virtual int OnSend();

protected:
    //业务需要继承实现
    // 为发送打包
    virtual int HandleEncode(
            char *buf,
            int &len)=0;

    // 回应包完整性检查
    virtual int HandleInput(
            const char *buf,
            int len)=0;

    //初始化
    virtual int HandleInit()=0;
    // 发送包接收完毕
    virtual int HandleSendOver()=0;
    // 回应包接受完毕
    virtual int HandleRecvOver(const char *buf, int len)=0;
};
//=============================================================================

#endif
