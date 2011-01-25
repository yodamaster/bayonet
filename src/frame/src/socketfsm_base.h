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
#include "fsm_interface.h"
#include "socketactor_base.h"

class CSocketFsmBase:public IFsm
{
public:
    CSocketFsmBase () {}
    virtual ~CSocketFsmBase () {}

    virtual int Init(IActor* pActor)
    {
        return 0;
    }
    virtual int Process(IActor* pActor)
    {
        return 0;
    }
    virtual int Fini(IActor* pActor)
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

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnInit();
    }
};
class CSocketFsmFini: public CSocketFsmBase
{
public:
    CSocketFsmFini () {}
    virtual ~CSocketFsmFini () {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnFini();
    }
};
//=============================================================================
class CSocketFsmWaitSend : public CSocketFsmBase
{
public:
    CSocketFsmWaitSend () {}
    virtual ~CSocketFsmWaitSend () {}

    virtual int Init(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(1);
        return 0;
    }
};
class CSocketFsmSending : public CSocketFsmBase
{
public:
    CSocketFsmSending () {}
    virtual ~CSocketFsmSending () {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnSend();
    }
};
class CSocketFsmSendOver : public CSocketFsmBase
{
public:
    CSocketFsmSendOver () {}
    virtual ~CSocketFsmSendOver () {}

    virtual int Init(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(1);
        return 0;
    }
    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnSendOver();
    }
};
//=============================================================================

class CSocketFsmWaitRecv : public CSocketFsmBase
{
public:
    CSocketFsmWaitRecv () {}
    virtual ~CSocketFsmWaitRecv () {}

    virtual int Init(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(1);
        return 0;
    }
};
class CSocketFsmRecving : public CSocketFsmBase
{
public:
    CSocketFsmRecving () {}
    virtual ~CSocketFsmRecving () {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnRecv();
    }
};
class CSocketFsmRecvOver : public CSocketFsmBase
{
public:
    CSocketFsmRecvOver () {}
    virtual ~CSocketFsmRecvOver () {}

    virtual int Init(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(1);
        return 0;
    }
    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->OnRecvOver();
        return 0;
    }
};
//=============================================================================

class CSocketFsmWaitClose : public CSocketFsmBase
{
public:
    CSocketFsmWaitClose () {}
    virtual ~CSocketFsmWaitClose () {}

    virtual int Init(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        pSocketActor->SetEvent(1);
        return 0;
    }
};
class CSocketFsmClosing : public CSocketFsmBase
{
public:
    CSocketFsmClosing () {}
    virtual ~CSocketFsmClosing () {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnClose();
    }
};
class CSocketFsmCloseOver : public CSocketFsmBase
{
public:
    CSocketFsmCloseOver () {}
    virtual ~CSocketFsmCloseOver () {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnCloseOver();
    }
};
//=============================================================================
class CSocketFsmError: public CSocketFsmBase
{
public:
    CSocketFsmError() {}
    virtual ~CSocketFsmError() {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnError();
    }
};
//=============================================================================
class CSocketFsmTimeout: public CSocketFsmBase
{
public:
    CSocketFsmTimeout() {}
    virtual ~CSocketFsmTimeout() {}

    virtual int Process(IActor* pActor)
    {
        CSocketActorBase* pSocketActor = (CSocketActorBase*)pActor;
        return pSocketActor->OnTimeout();
    }
};
//=============================================================================
#endif
