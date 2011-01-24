/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        syncframe.h
#  Description:     异步框架
#  Version:         1.0
#  LastChange:      2011-01-25 00:19:28
#  History:         
=============================================================================*/
#ifndef _SYNCFRAME_H_
#define _SYNCFRAME_H_
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "comm_def.h"
#include "socketfsm_base.h"

using namespace std;
class CSyncFrame
{
public:
    static CSyncFrame * Ins()
    {
        static CSyncFrame * _ins = NULL;
        if ( _ins == NULL)
            _ins = new CSyncFrame();
        return _ins;
    }
protected:
    CSyncFrame()
    {
        static auto_ptr<CSyncFrame> _auto_ptr = auto_ptr<CSyncFrame>(this);
        RegSocketFsm();
    }
    CSyncFrame(const CSyncFrame&);

    void RegSocketFsm()
    {
        m_mapFsmMgr[SOCKET_FSM_WAITSEND] = new CSocketFsmWaitSend();
        m_mapFsmMgr[SOCKET_FSM_SENDING] = new CSocketFsmSending();
        m_mapFsmMgr[SOCKET_FSM_SENDOVER] = new CSocketFsmSendOver();
        m_mapFsmMgr[SOCKET_FSM_WAITRECV] = new CSocketFsmWaitRecv();
        m_mapFsmMgr[SOCKET_FSM_RECVING] = new CSocketFsmRecving();
        m_mapFsmMgr[SOCKET_FSM_RECVOVER] = new CSocketFsmRecvOver();
        m_mapFsmMgr[SOCKET_FSM_WAITCLOSE] = new CSocketFsmWaitClose();
        m_mapFsmMgr[SOCKET_FSM_CLOSING] = new CSocketFsmClosing();
        m_mapFsmMgr[SOCKET_FSM_CLOSEOVER] = new CSocketFsmCloseOver();
        m_mapFsmMgr[SOCKET_FSM_ERROR] = new CSocketFsmError();
        m_mapFsmMgr[SOCKET_FSM_TIMEOUT] = new CSocketFsmTimeout();
    }

    virtual ~CSyncFrame(){}
    friend class auto_ptr<CSyncFrame>;

    map<int, IFsm*> m_mapFsmMgr;
};

#endif
