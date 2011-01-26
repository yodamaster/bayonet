/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        bayonet_frame.h
#  Description:     异步框架
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
#include "socketfsm_base.h"
#include "appfsm_base.h"
#include "epoller.h"
#include "socketactor_listen.h"

using namespace std;

typedef struct _StFrameParam
{
    string ip;
    int port;
    int protoType;
    int epollSize;
    int epollTimeoutMs;


    CSocketActorListen* pSocketActorListen;
    _StFrameParam()
    {
        port = 0;
        protoType = 0;
        epollSize = 102400;
        epollTimeoutMs = 0;//立即返回
        pSocketActorListen = NULL;
    }
} StFrameParam;
class CBayonetFrame : public CFrameBase
{
public:
    static CBayonetFrame * Ins()
    {
        static CBayonetFrame * _ins = NULL;
        if ( _ins == NULL)
            _ins = new CBayonetFrame();
        return _ins;
    }
    int Init(StFrameParam param)
    {
        m_StFrameParam = param;
        return 0;
    }

    int RegSocketFsm(int state, IFsm* fsm)
    {
        if (fsm == NULL)
        {
            return -1;
        }
        m_mapFsmMgr[state] = fsm;
        return 0;
    }
    int RegAppFsm(int state, IFsm* fsm)
    {
        if (fsm == NULL)
        {
            return -1;
        }
        m_mapAppFsmMgr[state] = fsm;
        return 0;
    }

    //执行
    int Process()
    {
        //必须要传一个socketlisten指针进来
        if (m_StFrameParam.pSocketActorListen == NULL)
        {
            return -1;
        }
        m_epoller.Create(m_StFrameParam.epollSize);

        CSocketActorListen* pSocketActorListen = m_StFrameParam.pSocketActorListen;
        pSocketActorListen->AttachFrame(this);

        pSocketActorListen->ChangeState(SOCKET_FSM_INIT);

        m_epoller.LoopForEvent(m_StFrameParam.epollTimeoutMs);

        return 0;
    }
protected:
    CBayonetFrame()
    {
        static auto_ptr<CBayonetFrame> _auto_ptr = auto_ptr<CBayonetFrame>(this);
        RegDefaultSocketFsms();
        RegDefaultAppFsms();
    }
    CBayonetFrame(const CBayonetFrame&);

    void RegDefaultAppFsms()
    {
        RegAppFsm(APP_FSM_FINI,new CAppFsmFini());
    }
    void RegDefaultSocketFsms()
    {
        RegSocketFsm(SOCKET_FSM_INIT, new CSocketFsmInit());
        RegSocketFsm(SOCKET_FSM_FINI, new CSocketFsmFini());
        RegSocketFsm(SOCKET_FSM_WAITSEND, new CSocketFsmWaitSend());
        RegSocketFsm(SOCKET_FSM_SENDING, new CSocketFsmSending());
        RegSocketFsm(SOCKET_FSM_SENDOVER, new CSocketFsmSendOver());
        RegSocketFsm(SOCKET_FSM_WAITRECV, new CSocketFsmWaitRecv());
        RegSocketFsm(SOCKET_FSM_RECVING, new CSocketFsmRecving());
        RegSocketFsm(SOCKET_FSM_RECVOVER, new CSocketFsmRecvOver());
        RegSocketFsm(SOCKET_FSM_WAITCLOSE, new CSocketFsmWaitClose());
        RegSocketFsm(SOCKET_FSM_CLOSING, new CSocketFsmClosing());
        RegSocketFsm(SOCKET_FSM_CLOSEOVER, new CSocketFsmCloseOver());
        RegSocketFsm(SOCKET_FSM_ERROR, new CSocketFsmError());
        RegSocketFsm(SOCKET_FSM_TIMEOUT, new CSocketFsmTimeout());
    }

protected:
    virtual ~CBayonetFrame(){}
    friend class auto_ptr<CBayonetFrame>;

    map<int, IFsm*> m_mapFsmMgr;
    map<int, IFsm*> m_mapAppFsmMgr;

    StFrameParam m_StFrameParam;
    CEPoller m_epoller;
};
#endif
