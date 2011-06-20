/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketactor_listen_udp.h
#  Description:     被动版
#  Version:         1.0
#  LastChange:      2011-01-28 00:46:16
#  History:         
=============================================================================*/
#ifndef _SOCKETACTOR_LISTEN_UDP_H_
#define _SOCKETACTOR_LISTEN_UDP_H_
#include "socketactor_data.h"
#include "socketactor_passive.h"
class CSocketActorListenUdp : public CSocketActorData
{
public:
    CSocketActorListenUdp();
    virtual ~CSocketActorListenUdp();

    virtual int Init(string ip,int port,int timeout_ms,int protoType);

    void SetAttachedSocketMaxSize(int attachedSocketMaxSize);

    int GetAttachedSocketMaxSize();

    virtual int OnInitOver();

    virtual int OnWaitSend();

    virtual int OnSendOver();

    virtual int OnWaitRecv();

    virtual int OnRecv();

    virtual int OnRecvOver();

    virtual bool IsTimeOut(struct timeval& now_time);

protected:
    CSocketActorPassiveUdp* CreatePassiveActor();

    int m_attachedSocketMaxSize;
};
#endif
