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
enum APPFSM
{
    APP_FSM_ALLOVER=-1,//所有流程已经走完
    APP_FSM_KEEP=0,//保持当前状态
};

enum SOCKETFSM
{
    SOCKET_FSM_ALLOVER=-1,//所有流程已经走完
    SOCKET_FSM_KEEP=0,//保持当前状态
    SOCKET_FSM_INIT=1,//刚accept的状态
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
