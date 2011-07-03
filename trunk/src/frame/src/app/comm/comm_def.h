/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        comm_def.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-24 23:19:55
#  History:         
=============================================================================*/
#ifndef _COMM_DEF_H_20110630001619_
#define _COMM_DEF_H_20110630001619_

//socket相关
//recv一次的buf大小
#define RECV_SINGLE_BUFSIZE             1024
//recv的所有的数据的大小，避免不停的申请内存
#define RECV_INIT_BUFSIZE               2048

//tcp listen默认的backlog大小
#define TCP_BACKLOG_SIZE                10240


//epoll相关
//epoll管理的最大大小
#define EPOLL_FD_MAXSIZE                102400
//epoll一次返回事件的大小
#define EPOLL_EVENTS_MAXSIZE            10240
//epollwait的超时时间 ms
#define EPOLL_WAIT_TIMEMS               10
//epoll检查socket超时的时间间隔 ms
#define CHECK_INTERVAL_SOCK_MS          100
//epoll检查socket超时的时间间隔 ms
#define CHECK_INTERVAL_APP_MS           500

// 最大能够attached的socket个数，如果达到，会在listen socekt中进行拒绝
#define ATTACHED_SOCKET_MAXSIZE         10240


//GC相关
//GC需要的最大个数
#define GC_MAX_COUNT                    1024


//统计,log相关
//基础目录
#define BAYONET_INFO_DIR                "./"
//统计文件名字
#define BAYONET_STATFILE_NAME           "stat_file"
//log文件名
#define BAYONET_LOGFILE_NAME            "bayonet"

//Frame的AddActor方法的actorType的值
enum PROTOTYPE
{
    PROTO_TYPE_TCP=0,
    PROTO_TYPE_UDP,
};

enum ACTIONTYPE
{
    ACTIONTYPE_SENDRECV=0,         // 一发一收
    ACTIONTYPE_SENDONLY,           // 只发不收
};

enum ErrorNo  {
    ESucc                  = 0,    // 成功
    ESocketConnect         = -1,   // 连接失败
    ESocketSend            = -2,   // send失败
    ESocketRecv            = -3,   // recv失败
    ESocketTimeout         = -4,   // 超时
    ESocketError           = -5,   // 出错
    ESocketHangup          = -6,   // Hangup事件发生
    ESocketCreate          = -7,   // 创建socket失败
    ESocketAttachPoller    = -8,   // AttachPoller失败
    ESocketShutdown        = -9,   // 对端关闭连接

    ELogicCheckPkg         = -100, // 包检查失败
    ELogicInvalidState     = -101, // 非法状态
    ELogicEncodeFail       = -102, // 编码错误
    ELogicInvalidRouteType = -103, // 非法路由配置类型

    EAppActorTimeout       = -200, // Msg处理超时
};

enum APPFSM
{
    APP_FSM_ALLOVER=-1,            // 所有流程已经走完
    APP_FSM_KEEP=0,                // 保持当前状态
    APP_FSM_RSP=1000,              // 等待socket send完成，之后会变成FINI状态
    APP_FSM_FINI,                  // 析构要调用的，内定，外面不要使用
};

enum SOCKETFSM
{
    SOCKET_FSM_ALLOVER=-1,         // 所有流程已经走完
    SOCKET_FSM_KEEP=0,             // 保持当前状态
    SOCKET_FSM_INIT=1,             // 刚初始化
    SOCKET_FSM_INITOVER,
    SOCKET_FSM_FINI,               // 负责释放(比如标记为GC)
    SOCKET_FSM_WAITSEND,
    SOCKET_FSM_SENDING,
    SOCKET_FSM_SENDOVER,
    SOCKET_FSM_WAITRECV,
    SOCKET_FSM_RECVING,
    SOCKET_FSM_RECVOVER,
    SOCKET_FSM_WAITCLOSE,
    SOCKET_FSM_CLOSING,
    SOCKET_FSM_CLOSEOVER,
    SOCKET_FSM_ERROR,
    SOCKET_FSM_TIMEOUT,
};
#endif
