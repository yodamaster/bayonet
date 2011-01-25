/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen.cpp
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-25 17:04:07
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_LISTEN_CPP_
#define _SOCKETACTOR_LISTEN_CPP_
int CSocketActorListen::OnRecv()
{
    return SOCKET_FSM_RECVOVER;
}
int CSocketActorListen::OnSend()
{
    return SOCKET_FSM_SENDOVER;
}
#endif
