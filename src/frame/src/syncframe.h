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
#include "appfsm_base.h"

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
    /**
     * @brief   初始化监听
     *
     * @param   ip          ip
     * @param   port        port
     * @param   protoType   协议
     *
     * @return  0           succ
     *          else        fail
     */
    int Init(string ip, int port, int protoType);

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

    //执行监听
    int ServeForEver()
    {
        return 0;
    }
protected:
    CSyncFrame()
    {
        static auto_ptr<CSyncFrame> _auto_ptr = auto_ptr<CSyncFrame>(this);
        RegDefaultSocketFsms();
        RegDefaultAppFsms();
    }
    CSyncFrame(const CSyncFrame&);

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
    virtual ~CSyncFrame(){}
    friend class auto_ptr<CSyncFrame>;

    map<int, IFsm*> m_mapFsmMgr;
    map<int, IFsm*> m_mapAppFsmMgr;
};

#endif
