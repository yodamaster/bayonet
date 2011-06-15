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
#include "action.h"

using namespace std;

typedef struct _StFrameParam
{
    string ip;          //ip
    int port;           //端口
    int protoType;      //协议类型
    int backlog;        //backlog
    bool bKeepcnt;      //是否长链接(仅TCP协议有效)
    int timeOutMs;      //accept后的socket接收超时要断掉(tcp)，逻辑处理超时也要断掉,-1代表永不超时

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
        protoType = PROTO_TYPE_TCP;
        backlog = TCP_BACKLOG_SIZE;
        bKeepcnt = false;
        timeOutMs = -1;//默认就是收到链接之后就不超时

        pAction = NULL;

        epollSize = EPOLL_FD_MAXSIZE;
        epollWaitTimeMs = EPOLL_WAIT_TIMEMS;
        epollCheckTimeMs = CHECK_INTERVAL_MS;

        gcMaxCount = GC_MAX_COUNT;

        infoDir = BAYONET_INFO_DIR;

        iLogLevel = LM_TRACE;
        logFileName = BAYONET_LOGFILE_NAME;
        iLogMaxSize = LOG_DEFAULT_SIZE;

        statFileName = BAYONET_STATFILE_NAME;
    }
} StFrameParam;

class CBayonetFrame : public CFrameBase
{
public:
    CBayonetFrame();
    virtual ~CBayonetFrame();

    int Init(StFrameParam param);

    CEPoller* GetEpoller();

    //执行
    int Process();

protected:
    void RegDefaultAppFsms();

    void RegDefaultSocketFsms();

protected:
    StFrameParam m_StFrameParam;
    CEPoller m_epoller;
};
#endif
