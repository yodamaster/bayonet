/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        socketfsm_base.h
#  Description:     
#  Version:         1.0
#  LastChange:      2011-01-24 20:38:47
#  History:         
=============================================================================*/
#include "fsm_interface.h"
class CSocketFsmWaitRecv : public IFsm
{
public:
    CSocketFsmWaitRecv () {}
    virtual ~CSocketFsmWaitRecv () {}

    virtual int Init(IActor* pActor);
    virtual int Process(IActor* pActor);
    virtual int Fini(IActor* pActor);
};
class CSocketFsmRecving : public IFsm
{
public:
    CSocketFsmRecving () {}
    virtual ~CSocketFsmRecving () {}

    virtual int Init(IActor* pActor);
    virtual int Process(IActor* pActor);
    virtual int Fini(IActor* pActor);
};
class CSocketFsmRecvOver : public IFsm
{
public:
    CSocketFsmRecvOver () {}
    virtual ~CSocketFsmRecvOver () {}

    virtual int Init(IActor* pActor);
    virtual int Process(IActor* pActor);
    virtual int Fini(IActor* pActor);
};
