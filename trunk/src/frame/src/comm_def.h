/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        comm_def.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-24 23:19:55
#  History:         
=============================================================================*/
#ifndef _COMM_DEF_H_
#define _COMM_DEF_H_
#include <error.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//Frame的AddActor方法的actorType的值
enum PROTOTYPE
{
    PROTO_TYPE_TCP=0,
    PROTO_TYPE_UDP,
};

enum APPFSM
{
    APP_FSM_ALLOVER=-1,//所有流程已经走完
    APP_FSM_KEEP=0,//保持当前状态
    APP_FSM_FINI=1000,//析构要调用的，内定，外面不要使用
};

enum SOCKETFSM
{
    SOCKET_FSM_ALLOVER=-1,//所有流程已经走完
    SOCKET_FSM_KEEP=0,//保持当前状态
    SOCKET_FSM_INIT=1,//刚初始化
    SOCKET_FSM_FINI,//负责释放
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
