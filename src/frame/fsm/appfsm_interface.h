/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        appfsm_interface.h
#  Description:     面向应用的状态机接口定义
#  Version:         1.0
#  LastChange:      2011-01-19 23:41:32
#  History:         
=============================================================================*/
#ifndef _APPFSM_INTERFACE_H_
#define _APPFSM_INTERFACE_H_
#include "fsm_interface.h"

class IChildActorInfo
{
public:
    virtual ~IChildActorInfo() {}

    virtual void AttachUpperActor(IActor* pActor)=0;
    virtual IActor* GetUpperActor()=0;

    virtual int Process()=0;
};
#endif
