/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.h
#  Description:     异步框架
#  Version:         1.0
#  LastChange:      2011-01-26 17:12:23
#  History:         
=============================================================================*/
#ifndef _BAYONET_FRAME_H_
#define _BAYONET_FRAME_H_
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "comm_def.h"
#include "fl_log.h"
#include "socketfsm_base.h"
#include "appfsm_base.h"
#include "epoller.h"
#include "socketactor_listen.h"

using namespace std;

typedef struct _StFrameParam
{
    string ip;          //ip
    int port;           //端口
    int protoType;      //协议类型


    int epollSize;      //epoll监听的队列大小
    int epollWaitTimeMs; //epoll wait time(毫秒)
    int epollCheckTimeMs; //epoll 检查超时的时间

    int gcMaxCount;     //actorGC回收的最大值

    LogLevel iLogLevel; //log等级(LM_ALL,LM_TRACE,LM_DEBUG,LM_WARNING,LM_ERROR,LM_FATAL,LM_NONE)
    string logDir;      //log目录
    string logFileName; //log文件名
    int iLogMaxSize;    //log文件最大大小

    CSocketActorListen* pSocketActorListen;
    _StFrameParam()
    {
        port = 0;
        protoType = 0;

        epollSize = EPOLL_DFT_MAXSIZE;
        epollWaitTimeMs = 0;//立即返回
        epollCheckTimeMs = 10;

        gcMaxCount = 1024;

        pSocketActorListen = NULL;

        iLogLevel = LM_TRACE;
        logDir = "./";
        logFileName = "log";
        iLogMaxSize = LOG_DEFAULT_SIZE;
    }
} StFrameParam;

class CBayonetFrame : public CFrameBase
{
public:
    CBayonetFrame()
    {
        RegDefaultSocketFsms();
        RegDefaultAppFsms();
    }
    virtual ~CBayonetFrame(){}

    int Init(StFrameParam param)
    {
        m_StFrameParam = param;
        log_init(m_StFrameParam.iLogLevel,m_StFrameParam.logDir.c_str(),m_StFrameParam.logFileName.c_str(),m_StFrameParam.iLogMaxSize);
        m_epoller.Init(m_StFrameParam.epollSize,m_StFrameParam.epollWaitTimeMs,m_StFrameParam.epollCheckTimeMs,m_StFrameParam.gcMaxCount);
        return 0;
    }

    int RegSocketFsm(int state, IFsm* fsm)
    {
        if (fsm == NULL)
        {
            return -1;
        }
        m_mapFsmMgr[state] = fsm;
        return 0;
    }
    int RegAppFsm(int state, IFsm* fsm)
    {
        if (fsm == NULL)
        {
            return -1;
        }
        m_mapAppFsmMgr[state] = fsm;
        return 0;
    }

    CEPoller* GetEpoller()
    {
        return &m_epoller;
    }

    //执行
    int Process();

protected:
    void RegDefaultAppFsms()
    {
        RegAppFsm(APP_FSM_FINI,new CAppFsmFini());
    }
    void RegDefaultSocketFsms()
    {
        RegSocketFsm(SOCKET_FSM_INIT, new CSocketFsmInit());
        RegSocketFsm(SOCKET_FSM_FINI, new CSocketFsmFini());
        RegSocketFsm(SOCKET_FSM_WAITSEND, new CSocketFsmWaitSend());
        RegSocketFsm(SOCKET_FSM_SENDING, new CSocketFsmSending());
        RegSocketFsm(SOCKET_FSM_SENDOVER, new CSocketFsmSendOver());
        RegSocketFsm(SOCKET_FSM_WAITRECV, new CSocketFsmWaitRecv());
        RegSocketFsm(SOCKET_FSM_RECVING, new CSocketFsmRecving());
        RegSocketFsm(SOCKET_FSM_RECVOVER, new CSocketFsmRecvOver());
        RegSocketFsm(SOCKET_FSM_WAITCLOSE, new CSocketFsmWaitClose());
        RegSocketFsm(SOCKET_FSM_CLOSING, new CSocketFsmClosing());
        RegSocketFsm(SOCKET_FSM_CLOSEOVER, new CSocketFsmCloseOver());
        RegSocketFsm(SOCKET_FSM_ERROR, new CSocketFsmError());
        RegSocketFsm(SOCKET_FSM_TIMEOUT, new CSocketFsmTimeout());
    }

protected:
    map<int, IFsm*> m_mapFsmMgr;
    map<int, IFsm*> m_mapAppFsmMgr;

    StFrameParam m_StFrameParam;
    CEPoller m_epoller;
};
#endif
