/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.h
#  Description:     异步框架,务必打开快速回收-echo 1 > /proc/sys/net/ipv4/tcp_tw_recycle 
#  Version:         1.0
#  LastChange:      2011-01-26 17:12:23
#  History:         
=============================================================================*/
#ifndef _BAYONET_FRAME_H_20110630001658_
#define _BAYONET_FRAME_H_20110630001658_

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

    string ip;                  // ip
    int port;                   // 端口
    int protoType;              // 协议类型
    int backlog;                // backlog
    bool bKeepcnt;              // 是否长链接(仅TCP协议有效)
    int timeOutMs;              // 这里的超时有点意思:
                                // 当是TCP短链接orUDP的时候，代表socketpasstive的存活时间，要注意比appactor的超时时间长，否则会出现请求超时但是无返回的情况
                                // 当是TCP长链接的时候，代表没有收到请求的持续时间
                                // -1代表永不超时
    int attachedSocketMaxSize;  // 最大能够attached的socket个数，如果达到，会在listen socekt中进行拒绝


    IAction* pAction;           // 最开始的Action

    int workerNum;              // 启动的子进程数目

    string infoDir;             // 信息存放目录,包括 stat, log

    int timeAccuracy;           // 时间精度，0：低，1：高。
                                // 低精度采用record_time一次时间，之后都用记录的这个时间
                                // 高精度采用每个都真实取时间

    int epollSize;              // epoll监听的队列大小
    int epollWaitTimeMs;        // epoll wait time(毫秒)

    int checkIntervalTimeSockMs;// 检查sock超时的时间间隔
    int checkIntervalTimeAppMs; // 检查app超时的时间间隔

    int gcMaxCount;             // actorGC回收的最大值

    int iLogLevel;              // log等级(LM_ALL,LM_TRACE,LM_DEBUG,LM_WARNING,LM_ERROR,LM_FATAL,LM_NONE)
    string logFileName;         // log文件名
    int iLogMaxSize;            // log文件最大大小

    string statFileName;        // 统计文件名字
    int statLevel;              // 统计级别(EnumStatLevelNone,EnumStatLevelBrief,EnumStatLevelFull)

    _StFrameParam()
    {

        port = 0;
        protoType = PROTO_TYPE_TCP;
        backlog = TCP_BACKLOG_SIZE;
        bKeepcnt = false;
        timeOutMs = -1;         // 默认就是收到链接之后就不超时
        attachedSocketMaxSize = ATTACHED_SOCKET_MAXSIZE;

        pAction = NULL;

        workerNum = 1;

        infoDir = BAYONET_INFO_DIR;

        timeAccuracy = 0; // 默认低精度

        epollSize = EPOLL_FD_MAXSIZE;
        epollWaitTimeMs = EPOLL_WAIT_TIMEMS;

        checkIntervalTimeSockMs = CHECK_INTERVAL_SOCK_MS;
        checkIntervalTimeAppMs = CHECK_INTERVAL_APP_MS;

        gcMaxCount = GC_MAX_COUNT;

        iLogLevel = LM_ERROR;
        logFileName = BAYONET_LOGFILE_NAME;
        iLogMaxSize = LOG_DEFAULT_SIZE;

        statFileName = BAYONET_STATFILE_NAME;
        statLevel = EnumStatLevelFull;
    }
} StFrameParam;

class CBayonetFrame : public CFrameBase
{
public:
    CBayonetFrame();
    virtual ~CBayonetFrame();

    int Init(const StFrameParam& param);

    int Init(const char* conf_path, IAction* pAction);

    CEPoller* GetEpoller();

    //执行
    int Process();

protected:
    int ParseConf(const char* conf_path, StFrameParam& param);

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
