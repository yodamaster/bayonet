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
    int workerNum;              // 启动的子进程数目

    string ip;                  // ip
    int port;                   // 端口
    int protoType;              // 协议类型
    int backlog;                // backlog
    bool bKeepcnt;              // 是否长链接(仅TCP协议有效)
    int timeOutMs;              // 这里的超时有点意思:
                                // 当是TCP短链接orUDP的时候，代表socketpasstive的存活时间，要注意比appactor的超时时间长，否则会出现请求超时但是无返回的情况
                                // 当是TCP长链接的时候，代表没有收到请求的持续时间
                                // -1代表永不超时

    IAction* pAction;           // 最开始的Action


    int epollSize;              // epoll监听的队列大小
    int epollWaitTimeMs;        // epoll wait time(毫秒)
    int epollCheckTimeSockMs;   // epoll 检查sock超时的时间
    int epollCheckTimeAppMs;    // epoll 检查app超时的时间

    int attachedSocketMaxSize;  // 最大能够attached的socket个数，如果达到，会在listen socekt中进行拒绝

    int gcMaxCount;             // actorGC回收的最大值

    string infoDir;             // 信息存放目录,包括 stat, log

    LogLevel iLogLevel;         // log等级(LM_ALL,LM_TRACE,LM_DEBUG,LM_WARNING,LM_ERROR,LM_FATAL,LM_NONE)
    string logFileName;         // log文件名
    int iLogMaxSize;            // log文件最大大小

    string statFileName;        // 统计文件名字

    _StFrameParam()
    {
        workerNum = 1;

        port = 0;
        protoType = PROTO_TYPE_TCP;
        backlog = TCP_BACKLOG_SIZE;
        bKeepcnt = false;
        timeOutMs = -1;         // 默认就是收到链接之后就不超时

        pAction = NULL;

        epollSize = EPOLL_FD_MAXSIZE;
        epollWaitTimeMs = EPOLL_WAIT_TIMEMS;
        epollCheckTimeSockMs = CHECK_INTERVAL_SOCK_MS;
        epollCheckTimeAppMs = CHECK_INTERVAL_APP_MS;

        attachedSocketMaxSize = ATTACHED_SOCKET_MAXSIZE;

        gcMaxCount = GC_MAX_COUNT;

        infoDir = BAYONET_INFO_DIR;

        iLogLevel = LM_ERROR;
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

    /**
     * @brief               子进程需要做的事情
     *
     * @return              0       succ
     */
    int ChildWork();

    /**
     * @brief               父进程调用fork
     *
     * @return              0       succ
     */
    int ForkWork();

protected:
    StFrameParam m_StFrameParam;
    CEPoller m_epoller;
    CSocketActorBase* m_pSocketActorListen;
};
#endif
