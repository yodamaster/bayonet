/*=============================================================================
#  Author:          dantezhu - http://www.vimer.cn
#  Email:           zny2008@gmail.com
#  FileName:        action_info.h
#  Description:     组合的方式封装了socket的细节
#  Version:         1.0
#  LastChange:      2011-01-28 15:43:20
#  History:         
=============================================================================*/
#ifndef _ACTION_INFO_H_
#define _ACTION_INFO_H_
#include "appactor_base.h"
#include "socketactor_active.h"
class CActionInfo
{
public:
    virtual ~CActionInfo () {}

    /**
     * @brief   设置action指针
     *
     * @param   pAction
     *
     * @return  0
     */
    //int SetIActionPtr(IAction *pAction);

    /**
     * @brief   设置AppActor指针
     *
     * @param   pActor
     *
     * @return  0
     */
    //int SetAppActor(CAppActorBase* pActor);
};
#endif
