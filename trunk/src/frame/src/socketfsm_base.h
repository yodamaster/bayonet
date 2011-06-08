/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketfsm_base.h
#  Description:     waitsend sending sendover waitrecv recving recvover waitclose closing closeover error timeout
#  Version:         1.0
#  LastChange:      2011-01-24 20:38:47
#  History:         
=============================================================================*/
#ifndef _SOCKETFSM_BASE_H_
#define _SOCKETFSM_BASE_H_
#include "comm_def.h"
#include "fsm_interface.h"
#include "fsm_achieve.h"
#include "socketactor_base.h"

class CSocketFsmBase:public CFsmBase
{
public:
    CSocketFsmBase () {}
    virtual ~CSocketFsmBase () {}

    virtual int HandleEntry(IActor* pActor)
    {
        return 0;
    }
    virtual int HandleProcess(IActor* pActor)
    {
        return 0;
    }
    virtual int HandleExit(IActor* pActor)
    {
        return 0;
    }

};
//=============================================================================
class CSocketFsmInit: public CSocketFsmBase
{
public:
    CSocketFsmInit () {}
    virtual ~CSocketFsmInit () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnInit();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmInit";
    }
};
class CSocketFsmInitOver: public CSocketFsmBase
{
public:
    CSocketFsmInitOver () {}
    virtual ~CSocketFsmInitOver () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnInitOver();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmInitOver";
    }
};
class CSocketFsmFini: public CSocketFsmBase
{
public:
    CSocketFsmFini () {}
    virtual ~CSocketFsmFini () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnFini();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmFini";
    }
};
class CSocketFsmFiniOver: public CSocketFsmBase
{
public:
    CSocketFsmFiniOver () {}
    virtual ~CSocketFsmFiniOver () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnFiniOver();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmFiniOver";
    }
};
//=============================================================================
class CSocketFsmWaitSend : public CSocketFsmBase
{
public:
    CSocketFsmWaitSend () {}
    virtual ~CSocketFsmWaitSend () {}

    virtual int HandleEntry(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(EPOLLOUT|EPOLLHUP|EPOLLERR);
        return 0;
    }
    virtual const string FsmName()
    {
        return "CSocketFsmWaitSend";
    }
};
class CSocketFsmSending : public CSocketFsmBase
{
public:
    CSocketFsmSending () {}
    virtual ~CSocketFsmSending () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnSend();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmSending";
    }
};
class CSocketFsmSendOver : public CSocketFsmBase
{
public:
    CSocketFsmSendOver () {}
    virtual ~CSocketFsmSendOver () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnSendOver();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmSendOver";
    }
};
//=============================================================================

class CSocketFsmWaitRecv : public CSocketFsmBase
{
public:
    CSocketFsmWaitRecv () {}
    virtual ~CSocketFsmWaitRecv () {}

    virtual int HandleEntry(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(EPOLLIN|EPOLLHUP|EPOLLERR);
        return 0;
    }
    virtual const string FsmName()
    {
        return "CSocketFsmWaitRecv";
    }
};
class CSocketFsmRecving : public CSocketFsmBase
{
public:
    CSocketFsmRecving () {}
    virtual ~CSocketFsmRecving () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnRecv();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmRecving";
    }
};
class CSocketFsmRecvOver : public CSocketFsmBase
{
public:
    CSocketFsmRecvOver () {}
    virtual ~CSocketFsmRecvOver () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnRecvOver();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmRecvOver";
    }
};
//=============================================================================

class CSocketFsmWaitClose : public CSocketFsmBase
{
public:
    CSocketFsmWaitClose () {}
    virtual ~CSocketFsmWaitClose () {}

    virtual int HandleEntry(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(EPOLLHUP|EPOLLERR);
        return 0;
    }
    virtual const string FsmName()
    {
        return "CSocketFsmWaitClose";
    }
};
class CSocketFsmClosing : public CSocketFsmBase
{
public:
    CSocketFsmClosing () {}
    virtual ~CSocketFsmClosing () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnClose();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmClosing";
    }
};
class CSocketFsmCloseOver : public CSocketFsmBase
{
public:
    CSocketFsmCloseOver () {}
    virtual ~CSocketFsmCloseOver () {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnCloseOver();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmCloseOver";
    }
};
//=============================================================================
class CSocketFsmError: public CSocketFsmBase
{
public:
    CSocketFsmError() {}
    virtual ~CSocketFsmError() {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnError();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmError";
    }
};
//=============================================================================
class CSocketFsmTimeout: public CSocketFsmBase
{
public:
    CSocketFsmTimeout() {}
    virtual ~CSocketFsmTimeout() {}

    virtual int HandleProcess(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnTimeout();
    }
    virtual const string FsmName()
    {
        return "CSocketFsmTimeout";
    }
};
//=============================================================================
#endif
