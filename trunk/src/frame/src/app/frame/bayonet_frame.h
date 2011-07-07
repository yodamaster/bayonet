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

    string ip;                       // ip
    int port;                        // 端口
    int proto_type;                  // 协议类型
    int backlog;                     // backlog
    bool keep_cnt;                   // 是否长链接(仅TCP协议有效)
    int timeout_ms;                  // 这里的超时有点意思:
                                     // 当是TCP短链接orUDP的时候，代表socketpasstive的存活时间，要注意比appactor的超时时间长，否则会出现请求超时但是无返回的情况
                                     // 当是TCP长链接的时候，代表没有收到请求的持续时间
                                     // -1代表永不超时
    int attached_socket_maxsize;     // 最大能够attached的socket个数，如果达到，会在listen socekt中进行拒绝


    IAction* action;                // 最开始的Action

    int worker_num;                  // 启动的子进程数目

    string info_dir;                 // 信息存放目录,包括 stat, log

    int time_accuracy;               // 时间精度，0：低，1：高。
                                     // 低精度采用record_time一次时间，之后都用记录的这个时间
                                     // 高精度采用每个都真实取时间

    int epoll_size;                  // epoll监听的队列大小
    int epoll_wait_time_ms;          // epoll wait time(毫秒)

    int check_sock_interval_time_ms; // 检查sock超时的时间间隔
    int check_app_interval_time_ms;  // 检查app超时的时间间隔

    int gc_maxcount;                 // actorGC回收的最大值

    int log_level;                   // log等级(LM_ALL,LM_TRACE,LM_DEBUG,LM_WARNING,LM_ERROR,LM_FATAL,LM_NONE)
    string log_filename;             // log文件名
    int log_maxsize;                 // log文件最大大小

    string stat_filename;            // 统计文件名字
    int stat_level;                  // 统计级别(EnumStatLevelNone,EnumStatLevelBrief,EnumStatLevelFull)

    _StFrameParam()
    {

        port = 0;
        proto_type = PROTO_TYPE_TCP;
        backlog = TCP_BACKLOG_SIZE;
        keep_cnt = false;
        timeout_ms = -1;         // 默认就是收到链接之后就不超时
        attached_socket_maxsize = ATTACHED_SOCKET_MAXSIZE;

        action = NULL;

        worker_num = 1;

        info_dir = BAYONET_INFO_DIR;

        time_accuracy = 0; // 默认低精度

        epoll_size = EPOLL_FD_MAXSIZE;
        epoll_wait_time_ms = EPOLL_WAIT_TIMEMS;

        check_sock_interval_time_ms = CHECK_SOCK_INTERVAL_TIMEMS;
        check_app_interval_time_ms = CHECK_APP_INTERVAL_TIMEMS;

        gc_maxcount = GC_MAX_COUNT;

        log_level = LM_ERROR;
        log_filename = BAYONET_LOGFILE_NAME;
        log_maxsize = LOG_DEFAULT_SIZE;

        stat_filename = BAYONET_STATFILE_NAME;
        stat_level = EnumStatLevelFull;
    }
} StFrameParam;

class CBayonetFrame : public CFrameBase
{
public:
    CBayonetFrame();
    virtual ~CBayonetFrame();

    int Init(const StFrameParam& param);

    int Init(const char* conf_path, IAction* action);

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
