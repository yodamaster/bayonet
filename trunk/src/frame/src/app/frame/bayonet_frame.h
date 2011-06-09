/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.h
#  Description:     异步框架,务必打开快速回收-echo 1 > /proc/sys/net/ipv4/tcp_tw_recycle 
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

using namespace std;

typedef struct _StFrameParam
{
    string ip;          //ip
    int port;           //端口
    int protoType;      //协议类型
    int backlog;        //backlog
    bool bKeepcnt;      //是否长链接(仅TCP协议有效)

    IAction* pAction;   //最开始的Action


    int epollSize;      //epoll监听的队列大小
    int epollWaitTimeMs; //epoll wait time(毫秒)
    int epollCheckTimeMs; //epoll 检查超时的时间

    int gcMaxCount;     //actorGC回收的最大值

    string infoDir;      //信息存放目录,包括 stat, log

    LogLevel iLogLevel; //log等级(LM_ALL,LM_TRACE,LM_DEBUG,LM_WARNING,LM_ERROR,LM_FATAL,LM_NONE)
    string logFileName; //log文件名
    int iLogMaxSize;    //log文件最大大小

    string statFileName; //统计文件名字

    _StFrameParam()
    {
        port = 0;
        protoType = 0;
        backlog = 10240;
        bKeepcnt = false;

        pAction = NULL;

        epollSize = EPOLL_DFT_MAXSIZE;
        epollWaitTimeMs = 10;
        epollCheckTimeMs = 10;

        gcMaxCount = 1024;

        infoDir = "./";

        iLogLevel = LM_TRACE;
        logFileName = "log";
        iLogMaxSize = LOG_DEFAULT_SIZE;

        statFileName = "stat_file";
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

        int ret;

        string statDir = param.infoDir+string("/stat/");
        string logDir = param.infoDir + string("/log/");

        mkdir(statDir.c_str(),0777);
        mkdir(logDir.c_str(),0777);

        log_init(m_StFrameParam.iLogLevel,logDir.c_str(),m_StFrameParam.logFileName.c_str(),m_StFrameParam.iLogMaxSize);

        ret = CFrameBase::Init(statDir.c_str(), param.statFileName.c_str());
        if (ret != 0)
        {
            error_log("CFrameBase init fail,ret:%d",ret);
            return -1;
        }

        m_epoller.Init(m_StFrameParam.epollSize,m_StFrameParam.epollWaitTimeMs,m_StFrameParam.epollCheckTimeMs,m_StFrameParam.gcMaxCount);
        m_epoller.AttachFrame(this);
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
        RegFsm(APP_FSM_RSP,new CAppFsmRsp());
        RegFsm(APP_FSM_FINI,new CAppFsmFini());
    }
    void RegDefaultSocketFsms()
    {
        RegFsm(SOCKET_FSM_INIT, new CSocketFsmInit());
        RegFsm(SOCKET_FSM_INITOVER, new CSocketFsmInitOver());
        RegFsm(SOCKET_FSM_FINI, new CSocketFsmFini());
        RegFsm(SOCKET_FSM_FINIOVER, new CSocketFsmFiniOver());
        RegFsm(SOCKET_FSM_WAITSEND, new CSocketFsmWaitSend());
        RegFsm(SOCKET_FSM_SENDING, new CSocketFsmSending());
        RegFsm(SOCKET_FSM_SENDOVER, new CSocketFsmSendOver());
        RegFsm(SOCKET_FSM_WAITRECV, new CSocketFsmWaitRecv());
        RegFsm(SOCKET_FSM_RECVING, new CSocketFsmRecving());
        RegFsm(SOCKET_FSM_RECVOVER, new CSocketFsmRecvOver());
        RegFsm(SOCKET_FSM_WAITCLOSE, new CSocketFsmWaitClose());
        RegFsm(SOCKET_FSM_CLOSING, new CSocketFsmClosing());
        RegFsm(SOCKET_FSM_CLOSEOVER, new CSocketFsmCloseOver());
        RegFsm(SOCKET_FSM_ERROR, new CSocketFsmError());
        RegFsm(SOCKET_FSM_TIMEOUT, new CSocketFsmTimeout());
    }

protected:
    StFrameParam m_StFrameParam;
    CEPoller m_epoller;
};
#endif
